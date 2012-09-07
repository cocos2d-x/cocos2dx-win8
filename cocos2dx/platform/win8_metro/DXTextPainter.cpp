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
#include "DXTextPainter.h"
#include "FontLoader.h"
#include "CCCommon.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::Storage::Streams;

// helper functions to get the proper resolution scale
// (even if invalid value is received)
static int GetResolutionScaleInt()
{
    int resolutionScale = (int)Windows::Graphics::Display::DisplayProperties::ResolutionScale;
	// if <= - consider as 100%
	if (resolutionScale <= 0)
		resolutionScale = 100;
	return resolutionScale;
}

static float GetResolutionScale()
{
	return (float)GetResolutionScaleInt() / 100.0;
}

DXTextPainter::DXTextPainter()
: m_fontLoader()
, m_dwriteFactory()
, m_fontCollection()
, m_bUseCustomFont(false)
, m_bIsFontChanged(false)
{

}

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
	m_bUseCustomFont = false;
	m_bIsFontChanged = false;

	wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};
	GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	m_locale = ref new Platform::String(localeName);
}


bool DXTextPainter::SetFont(Platform::String^ fontName , UINT nSize)
{
	bool bReCreate = false;
	std::wstring wstrFontName(fontName->Data());
	std::wstring suffix = wstrFontName.substr(wstrFontName.rfind(L'.') == std::wstring::npos ? wstrFontName.length() : wstrFontName.rfind(L'.') + 1);


	if(nSize!=0 && nSize != m_fontSize)
	{
		m_fontSize = nSize;
		bReCreate = true;
	}

	FLOAT fntSize = (FLOAT)m_fontSize;
	fntSize /= GetResolutionScale();

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};
    GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	Platform::String^ refLocalName = ref new String(localeName);
	if(m_locale != refLocalName)
	{
		m_locale = ref new Platform::String(localeName);
		bReCreate = true;
	}


	

	if(suffix == L"" && (suffix.length() == 0))
	{
		//use sysstem font
		m_bUseCustomFont = false;
		m_fontName = ref new Platform::String(wstrFontName.c_str());
		
	}else{
		// use custom font
		m_bUseCustomFont = true;
		int pos = wstrFontName.find_last_of(L'/')+1;
		std::wstring wfontName(wstrFontName.substr(pos,wstrFontName.length()-pos-4));
		
		std::wstring oldFontName(m_fontName->Data());
		if( oldFontName != wstrFontName )
		{
			m_bIsFontChanged = true;
		}
		m_fontName = ref new String(wfontName.c_str());



		//create FontLoader if not exsit
		if(nullptr == m_fontLoader){
			m_fontLoader = new FontLoader(m_dwriteFactory.Get());
			m_fontLoader->LoadFont(wstrFontName);
			DX::ThrowIfFailed(
				m_dwriteFactory->RegisterFontFileLoader(m_fontLoader.Get())
				);
			DX::ThrowIfFailed(
				m_dwriteFactory->RegisterFontCollectionLoader(m_fontLoader.Get())
				);
		}else{
			if(m_bIsFontChanged)
			{
				// Unregister the font loader from DirectWrite factory
				DX::ThrowIfFailed(
					m_dwriteFactory->UnregisterFontCollectionLoader(m_fontLoader.Get())
					);

				DX::ThrowIfFailed(
					m_dwriteFactory->UnregisterFontFileLoader(m_fontLoader.Get())
					);
				m_fontLoader = nullptr;
				m_fontCollection = nullptr;
				m_fontLoader = new FontLoader(m_dwriteFactory.Get());
				m_fontLoader->LoadFont(wstrFontName);
				DX::ThrowIfFailed(
					m_dwriteFactory->RegisterFontFileLoader(m_fontLoader.Get())
					);
				DX::ThrowIfFailed(
					m_dwriteFactory->RegisterFontCollectionLoader(m_fontLoader.Get())
					);
			}
		}

		size_t fontCollectionKey = 0;

		DX::ThrowIfFailed(
			m_dwriteFactory->CreateCustomFontCollection(
			m_fontLoader.Get(),
			&fontCollectionKey,
			sizeof(size_t),
			&m_fontCollection
			)
			);

	}

	if(bReCreate && m_TextFormat){
		//release old one
		m_TextFormat = nullptr;
	}

	if(m_TextFormat == nullptr)
	{
		DX::ThrowIfFailed(
			m_dwriteFactory->CreateTextFormat(
			m_fontName->Data(),
			nullptr,  // Proper font collection will be set upon drawing
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			m_locale->Data(),
			&m_TextFormat
			)
			);
	}

	return true;
}

Platform::Array<byte>^  DXTextPainter::DrawTextToImage(Platform::String^ text, Windows::Foundation::Size* tSize, TextAlignment alignment)
{
	if(text->Length() == 0){
		return nullptr;
	}

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
		tSize->Width = FLT_MAX;
		isShouldAdjustBounds = true;
	}

	if(m_textLayout)
	{
		//release old one
		m_textLayout = nullptr;
	}



	DX::ThrowIfFailed(
		m_dwriteFactory->CreateTextLayout(
		text->Data(),
		text->Length(),
		m_TextFormat.Get(),
		tSize->Width,
		FLT_MAX,
		&m_textLayout
		)
		);



	//Here invalid!
	DWRITE_TEXT_RANGE fullRange = {0, text->Length()};
	DX::ThrowIfFailed(
		m_textLayout->SetFontFamilyName(m_fontName->Data(),fullRange)
		);
	if(m_bUseCustomFont){
		DX::ThrowIfFailed(
			m_textLayout->SetFontCollection(m_fontCollection.Get(),fullRange)
			);
	}
	m_bIsFontChanged = false;

	DWRITE_TEXT_METRICS metrics;
	m_textLayout->GetMetrics(&metrics);
	Windows::Foundation::Size newSize(metrics.width, metrics.height);
	// layoutTextSize is used to contain text size without ResolutionScale
	// otherwise it is possible that due to floating point accuracy problem
	// the provided size to the layout will be less than required
	// and the text will be divided in 2 lines instead 1, for example.
	Windows::Foundation::Size layoutTextSize(metrics.width, metrics.height);

	// allow resolution scale usage
	newSize.Width *= GetResolutionScale();
	newSize.Height *= GetResolutionScale();

	if(isShouldAdjustBounds)
	{
		tSize->Width = newSize.Width;
		tSize->Height =  newSize.Height;
	}
	else
	{
		// Originally provided tSize (provided with an original resolution)
		// will be used - so layoutTextSize should be calculated.
		layoutTextSize.Width = tSize->Width / GetResolutionScale();
		layoutTextSize.Height = tSize->Height / GetResolutionScale();
	}

	if(tSize->Height <=0)
	{
		tSize->Height =  newSize.Height;
	}
	else
	{
		layoutTextSize.Height = tSize->Height / GetResolutionScale();
	}

	m_textLayout->SetMaxWidth(layoutTextSize.Width);
	m_textLayout->SetMaxHeight(layoutTextSize.Height);

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
	DX::ThrowIfFailed(
		wicFormatConverter->GetSize(&uiWidth, &uiHeight)
		);

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

		// ensure that last bottom/right lines will not be lost - return the same size
		// (which is used to draw text)
		tSize->Width = (float)rc.Width;
		tSize->Height = (float)rc.Height;
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

DXTextPainter::~DXTextPainter()
{
	if (m_fontLoader != nullptr)
	{
		// Unregister the font loader from DirectWrite factory
		DX::ThrowIfFailed(
			m_dwriteFactory->UnregisterFontCollectionLoader(m_fontLoader.Get())
			);

		DX::ThrowIfFailed(
			m_dwriteFactory->UnregisterFontFileLoader(m_fontLoader.Get())
			);
	}
}