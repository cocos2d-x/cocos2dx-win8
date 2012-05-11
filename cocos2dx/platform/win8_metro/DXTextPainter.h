

#pragma once

#include "pch.h"
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <shcore.h>
#include "DirectXHelper.h"


enum class TextAlignment
{
    TextAlignmentLeft = 0,
    TextAlignmentCenter,
    TextAlignmentRight,
};

ref class DXTextPainter
{
public:
	DXTextPainter(){};

	void Initialize(
		_In_ ID2D1DeviceContext*  d2dContext,
		_In_ IWICImagingFactory2*  wicFactory,
		_In_ IDWriteFactory1*      dwriteFactory
		);

	bool				    SetFont(Platform::String^ fontName, UINT nSize);
	Platform::Array<byte>^  DrawTextToImage(Platform::String ^text, Windows::Foundation::Size* tSize, TextAlignment alignment);

protected:
	// Direct2D Objects
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>				m_d2dContext;
	Microsoft::WRL::ComPtr<IDWriteFactory1>					m_dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>				m_wicFactory;
	// DirectWrite & Windows Imaging Component Objects

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			m_whiteBrush;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>					m_d2dTargetBitmap;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>				m_textLayout;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>				m_TextFormat;

	Platform::String^										m_fontName;
	Platform::String^										m_locale;
	UINT													m_fontSize;

private:
	// Save render target bitmap to a stream using WIC.
	void SaveBitmapToStream(
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>d2dBitmap,
		Microsoft::WRL::ComPtr<IWICImagingFactory2>wicFactory2,
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>d2dContext,
		REFGUID wicFormat,
		IStream* stream
		);
	bool PrepareBitmap(UINT nWidth, UINT nHeight);
};
