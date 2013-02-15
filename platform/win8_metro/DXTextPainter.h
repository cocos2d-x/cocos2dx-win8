/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* 
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0 
* 
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include "pch.h"

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#else
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include "dwrite.h"
#endif

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
internal:
	DXTextPainter(){};

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#else
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
#endif
};
