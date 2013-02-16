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
#include <d3d11_1.h>
#include <shcore.h>
#include <map>
#include "CCCommon.h"
#include "CCDrawingPrimitives.h"
#include "DirectXHelper.h"
#include "CCFileUtils.h"
#include "./freetype/ft2build.h"
#include FT_FREETYPE_H


enum class TextAlignment
{
	TextAlignmentLeft = 0,
	TextAlignmentCenter,
	TextAlignmentRight,
};

typedef struct tagTTF_OFFSET_TABLE{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
}TTF_OFFSET_TABLE;

typedef struct tagTTF_TABLE_DIRECTORY{
	char	szTag[4];
	ULONG	uCheckSum;
	ULONG	uOffset;
	ULONG	uLength;
}TTF_TABLE_DIRECTORY;

typedef struct tagTTF_NAME_TABLE_HEADER{
	USHORT	uFSelector;
	USHORT	uNRCount;
	USHORT	uStorageOffset;
}TTF_NAME_TABLE_HEADER;

typedef struct tagTTF_NAME_RECORD{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;
}TTF_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

typedef std::multimap<Platform::String^, Platform::String^>	FONTS_MAP;
typedef std::multimap<Platform::String^, Platform::String^>::const_iterator	FONTS_MAP_CITR;

ref class FTTextPainter
{
public:
	virtual ~FTTextPainter()
    {
	    m_fontMap.clear();
	    if(m_fontFace != nullptr) FT_Done_Face(m_fontFace);
	    if(m_textLibrary != nullptr) FT_Done_FreeType(m_textLibrary);
    }

internal:
	FTTextPainter();
	bool SetFont(Platform::String^ fontName, UINT nSize);
	Platform::Array<byte>^  DrawTextToImage(Platform::String ^text, Windows::Foundation::Size* tSize, TextAlignment alignment);

private:
	FT_Library				m_textLibrary;
	FT_Face					m_fontFace;

	Platform::String^		m_fontName;
	Platform::String^		m_locale;
	UINT					m_fontSize;

	FONTS_MAP				m_fontMap;

private:
	Platform::String^ GetFontStore();
	Platform::String^ GetFontNameFromFile(Platform::String^ filePath);
	bool BuildFontsInformation();

};
