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

#pragma once

#include "pch.h"
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <shcore.h>
#include "DirectXHelper.h"

class FontLoader;

enum class TextAlignment
{
    TextAlignmentLeft = 0,
    TextAlignmentCenter,
    TextAlignmentRight,
};

ref class DXTextPainter
{
	~DXTextPainter();
public:
	internal:
		DXTextPainter();

	void Initialize(
		_In_ ID2D1DeviceContext*  d2dContext,
		_In_ IWICImagingFactory2*  wicFactory,
		_In_ IDWriteFactory1*      dwriteFactory
		);

	bool				    SetFont(Platform::String^ fontName, UINT nSize);
	Platform::Array<byte>^  DrawTextToImage(Platform::String ^text, Windows::Foundation::Size* tSize, TextAlignment alignment);

private:
	// Direct2D Objects
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>				m_d2dContext;
	Microsoft::WRL::ComPtr<IDWriteFactory1>					m_dwriteFactory;
	// Loader of font files accessible within this document
    Microsoft::WRL::ComPtr<FontLoader>						m_fontLoader;
	// Collection of all document fonts
	Microsoft::WRL::ComPtr<IDWriteFontCollection>			m_fontCollection;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>				m_wicFactory;
	// DirectWrite & Windows Imaging Component Objects

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			m_whiteBrush;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>					m_d2dTargetBitmap;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>				m_textLayout;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>				m_TextFormat;

	Platform::String^										m_fontName;
	Platform::String^										m_locale;
	UINT													m_fontSize;
	bool													m_bUseCustomFont;
	bool													m_bIsFontChanged;

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