/****************************************************************************
Copyright (c) 2012 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include <ppltasks.h>
#include "DXTextPainter.h"
#include "FontLoader.h"
#include "../CCCommon.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::Storage::Streams;
using namespace Concurrency;

void DXTextPainter:: Initialize(
	_In_ ID2D1DeviceContext*  d2dContext,
	_In_ IWICImagingFactory2*  wicFactory,
	_In_ IDWriteFactory1*      dwriteFactory
	)
{
	m_d2dContext = d2dContext;
    m_wicFactory = wicFactory;
    m_dwriteFactory = dwriteFactory;

	m_fontName = L"Arial";
	m_fontSize = 24;
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};
	GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	m_locale = ref new Platform::String(localeName);

	m_CustomFontContext = new CustomFontContext(dwriteFactory);

	m_CustomFontContext->LoadFont( std::wstring(m_fontName->Data()) );
}


bool DXTextPainter::SetFont(Platform::String^ fontName , UINT nSize)
{
	bool bReCreate = false;

	if(nSize!=0 && nSize != m_fontSize)
	{
		m_fontSize = nSize;
		bReCreate = true;
	}

	FLOAT fntSize = (FLOAT)m_fontSize;

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};
    GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	Platform::String^ refLocalName = ref new String(localeName);
	if(m_locale != refLocalName)
	{
		m_locale = ref new Platform::String(localeName);
		bReCreate = true;
	}


	if(bReCreate && m_TextFormat){
		//release old one
		m_TextFormat = nullptr;
	}
	
	m_CustomFontContext->LoadFont( std::wstring(fontName->Data()) );

	std::wstring wfontNameTmp( fontName->Data() ); 
	int pos = wfontNameTmp.find_last_of(L'/')+1;
	std::wstring wfontName(wfontNameTmp.substr(pos,wfontNameTmp.length()-pos-4));
	m_fontName = fontName = ref new String(wfontName.c_str());
	
	if(m_TextFormat == nullptr)
	{
		DX::ThrowIfFailed(
			m_dwriteFactory->CreateTextFormat(
			fontName->Data(), // 
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,   //fntSize
			m_locale->Data(),//L"en-US",
			&m_TextFormat
			)
			);
	}



	return true;
}


Platform::Array<byte>^  DXTextPainter::DrawTextToImage(Platform::String^ text, Windows::Foundation::Size* tSize, TextAlignment alignment)
{
	//set text alignment and paragraph alignment
	switch (alignment)
	{
	case TextAlignment::TextAlignmentLeft: // left
		DX::ThrowIfFailed(
			m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
			);
		DX::ThrowIfFailed(
			m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
			);
		break;
	case TextAlignment::TextAlignmentRight: // right
		DX::ThrowIfFailed(
			m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
			);
		DX::ThrowIfFailed(
			m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
			);
		break;
	case TextAlignment::TextAlignmentCenter: // center
		DX::ThrowIfFailed(
			m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
			);
		DX::ThrowIfFailed(
			m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
			);
		break;
	}

	bool isShouldAdjustBounds = false;
	if(tSize->Width <= 0)
	{
		tSize->Width = 4096;
		isShouldAdjustBounds = true;
	}

	if(m_textLayout)
	{
		//release old one
		m_textLayout = nullptr;
	}

	IDWriteFontCollection* fontCollection = NULL;
	m_CustomFontContext->GetFontCollection(&fontCollection);
	

// 	DX::ThrowIfFailed(
// 		m_dwriteFactory->CreateTextFormat(
// 		m_fontName->Data(), // 
// 		fontCollection,
// 		DWRITE_FONT_WEIGHT_LIGHT,
// 		DWRITE_FONT_STYLE_NORMAL,
// 		DWRITE_FONT_STRETCH_NORMAL,
// 		m_fontSize,   //fntSize
// 		m_locale->Data(),//L"en-US",
// 		&m_TextFormat
// 		)
// 		);

	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextLayout(
		text->Data(),
		text->Length(),
		m_TextFormat.Get(),
		tSize->Width,
		4096.0f,
		&m_textLayout
		)
		);
	//Here invalid!
	DWRITE_TEXT_RANGE fullRange = {0, text->Length()};
	m_textLayout->SetFontFamilyName(m_fontName->Data(),fullRange);
	m_textLayout->SetFontCollection(fontCollection,fullRange);

	DWRITE_TEXT_METRICS metrics;
	m_textLayout->GetMetrics(&metrics);
	Windows::Foundation::Size newSize(metrics.width, metrics.height);

	if(isShouldAdjustBounds)
	{
		tSize->Width = newSize.Width;
		tSize->Height =  newSize.Height;
	}

	if(tSize->Height <=0)
	{
		tSize->Height =  newSize.Height;
	}

	m_textLayout->SetMaxWidth(tSize->Width);
	m_textLayout->SetMaxHeight(tSize->Height);

	if(m_whiteBrush == nullptr){
		DX::ThrowIfFailed(
			m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &m_whiteBrush)
			);
	}


	if(!PrepareBitmap((UINT)ceilf(tSize->Width),(UINT)ceilf(tSize->Height)))
	{
		return nullptr;
	}

	//Render
	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());

	m_d2dContext->BeginDraw();
	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

	m_d2dContext->DrawTextLayout(
		Point2F(0, 0),
		m_textLayout.Get(),
		m_whiteBrush.Get()
		);

	// We ignore the HRESULT returned as we want to application to handle the 
	// error when it uses Direct2D next.
	m_d2dContext->EndDraw();

	if ( m_d2dTargetBitmap == nullptr)
	{
		return nullptr;
	}

	ComPtr<IStream> stream;
	auto refStream = ref new InMemoryRandomAccessStream();
	DX::ThrowIfFailed(
		CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(refStream) ,IID_PPV_ARGS(&stream))
		);
	GUID wicFormat = GUID_ContainerFormatPng;
	SaveBitmapToStream(m_d2dTargetBitmap, m_wicFactory, m_d2dContext, wicFormat, stream.Get());

	refStream->Seek(0);
	ComPtr<IStream> stream2;
	DX::ThrowIfFailed(
		CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(refStream) ,IID_PPV_ARGS(&stream2))
		);
	ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
	DX::ThrowIfFailed(
		m_wicFactory->CreateDecoderFromStream(stream2.Get(),nullptr,WICDecodeMetadataCacheOnDemand,&wicBitmapDecoder)
		);

	ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
	DX::ThrowIfFailed(
		wicBitmapDecoder->GetFrame(0, &wicBitmapFrame)
		);

	ComPtr<IWICFormatConverter> wicFormatConverter;
	DX::ThrowIfFailed(
		m_wicFactory->CreateFormatConverter(&wicFormatConverter)
		);

	DX::ThrowIfFailed(
		wicFormatConverter->Initialize(
		wicBitmapFrame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom  // the BGRA format has no palette so this value is ignored
		)
		);

	double dpiX = 96.0f;
	double dpiY = 96.0f;
	DX::ThrowIfFailed(
		wicFormatConverter->GetResolution(&dpiX, &dpiY)
		);

	UINT uiWidth = (UINT)tSize->Width;
	UINT uiHeight = (UINT)tSize->Height;

	UINT cbStride = uiWidth * 4;
	UINT cbBufferSize = cbStride * uiHeight;
	Platform::Array<byte>^ pixelBuffer = ref new Platform::Array<byte>(cbBufferSize);

	if (true)
	{    
		WICRect rc;
		rc.X = 0;
		rc.Y = 0;
		rc.Width = uiWidth;
		rc.Height = uiHeight;

		DX::ThrowIfFailed(
			wicFormatConverter->CopyPixels(&rc, cbStride, cbBufferSize,pixelBuffer->Data)
			);
	}

	return pixelBuffer;

}

bool DXTextPainter::PrepareBitmap(UINT nWidth, UINT nHeight)
{
	if((nWidth == 0) || (nHeight == 0))
	{
		return false;
	}
	ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
	double dpiX = 96.0f;
	double dpiY = 96.0f;
	D2D1_SIZE_U size = D2D1::SizeU(nWidth,nHeight);

	BYTE* initData = new BYTE[nWidth * nHeight * 4 ];
	ZeroMemory(initData,nWidth * nHeight * 4);

	if(m_d2dTargetBitmap)
	{
		// release old one
		m_d2dTargetBitmap = nullptr;
	}

	DX::ThrowIfFailed(
		m_d2dContext->CreateBitmap(size,
		initData,
		nWidth*4,
		BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
		PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_PREMULTIPLIED
		),
		static_cast<float>(dpiX),
		static_cast<float>(dpiY),
		0
		),
		&m_d2dTargetBitmap
		)
		);
	//release initData
	if(initData){
			delete[]initData;
	}
	initData = 0;


	return true;
}

void DXTextPainter::SaveBitmapToStream( _In_ ComPtr<ID2D1Bitmap1> d2dBitmap, _In_ ComPtr<IWICImagingFactory2> wicFactory2, _In_ ComPtr<ID2D1DeviceContext> d2dContext, _In_ REFGUID wicFormat, _In_ IStream* stream )
{
	// Create and initialize WIC Bitmap Encoder.
	ComPtr<IWICBitmapEncoder> wicBitmapEncoder;
	DX::ThrowIfFailed(
		wicFactory2->CreateEncoder(
		wicFormat,
		nullptr,    // No preferred codec vendor.
		&wicBitmapEncoder
		)
		);

	DX::ThrowIfFailed(
		wicBitmapEncoder->Initialize(
		stream,
		WICBitmapEncoderNoCache
		)
		);

	// Create and initialize WIC Frame Encoder.
	ComPtr<IWICBitmapFrameEncode> wicFrameEncode;
	DX::ThrowIfFailed(
		wicBitmapEncoder->CreateNewFrame(
		&wicFrameEncode,
		nullptr     // No encoder options.
		)
		);

	DX::ThrowIfFailed(
		wicFrameEncode->Initialize(nullptr)
		);

	// Retrieve D2D Device.
	ComPtr<ID2D1Device> d2dDevice;
	d2dContext->GetDevice(&d2dDevice);

	// Create IWICImageEncoder.
	ComPtr<IWICImageEncoder> imageEncoder;
	DX::ThrowIfFailed(
		wicFactory2->CreateImageEncoder(
		d2dDevice.Get(),
		&imageEncoder
		)
		);

	DX::ThrowIfFailed(
		imageEncoder->WriteFrame(
		d2dBitmap.Get(),
		wicFrameEncode.Get(),
		nullptr     // Use default WICImageParameter options.
		)
		);

	DX::ThrowIfFailed(
		wicFrameEncode->Commit()
		);

	DX::ThrowIfFailed(
		wicBitmapEncoder->Commit()
		);

	// Flush all memory buffers to the next-level storage object.
	DX::ThrowIfFailed(
		stream->Commit(STGC_DEFAULT)
		);
}
