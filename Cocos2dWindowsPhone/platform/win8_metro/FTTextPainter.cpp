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

#include "pch.h"
#include "FTTextPainter.h"


using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace cocos2d;


const CHAR TAG_NAME_A[] = "name";
const WCHAR TAG_NAME_W[] = L"name";
const CHAR DEFAULT_FONT_A[] = "Segoe WP";
const WCHAR DEFAULT_FONT_W[] = L"Segoe WP";
const CHAR DEFAULT_FONT_FILE_A[] = "C:\\windows\\Fonts\\SegoeWP.ttf";
const WCHAR DEFAULT_FONT_FILE_W[] = L"C:\\windows\\Fonts\\SegoeWP.ttf";


FTTextPainter::FTTextPainter() : m_textLibrary(0), m_fontSize(10), m_fontName(ref new Platform::String(DEFAULT_FONT_W))
{
	if(0 != FT_Init_FreeType(&m_textLibrary)) m_textLibrary = nullptr;
	BuildFontsInformation();
}

Platform::Array<byte>^ FTTextPainter::DrawTextToImage(Platform::String^ text, Windows::Foundation::Size* tSize, TextAlignment alignment)
{
	if(text->Length() == 0) return nullptr;

	const UINT PIXEL_WIDTH = 4;
	const UINT LINE_SPACING = 1;
	const UINT BLANK_SPACE = m_fontSize / 4;

	CC_ASSERT(m_textLibrary != nullptr);
	CC_ASSERT(m_fontFace != nullptr);

	FT_Error error = 0;
	FT_GlyphSlot slot = m_fontFace->glyph;
	INT addHeight = 0;

	// estimate bitmap dimensions
	for (UINT n = 0; n < text->Length(); n++ )
	{
		error = FT_Load_Char(m_fontFace, text->Data()[n], FT_LOAD_RENDER);
		if (error) continue;

		FT_Bitmap glyphBmp = slot->bitmap;

		tSize->Width += (LINE_SPACING + (glyphBmp.rows * glyphBmp.width > 0 ? glyphBmp.width : BLANK_SPACE));
		tSize->Height = tSize->Height < glyphBmp.rows ? glyphBmp.rows : tSize->Height;
		addHeight = addHeight < ((INT)tSize->Height - slot->bitmap_top) ? ((INT)tSize->Height - slot->bitmap_top) : addHeight;
	}

	tSize->Height += addHeight;
	Platform::Array<byte>^ pixelBuffer = ref new Platform::Array<byte>((UINT)tSize->Width * (UINT)tSize->Height * PIXEL_WIDTH);
	memset(pixelBuffer->Data, 0, pixelBuffer->Length);

	INT buffRow = 0;
	INT buffCol = 0;

	// render each glyph to bitmap
	for (UINT n = 0; n < text->Length(); n++ )
	{
		error = FT_Load_Char(m_fontFace, text->Data()[n], FT_LOAD_RENDER);
		if (error) continue;

		FT_Bitmap glyphBmp = slot->bitmap;

		if(glyphBmp.rows * glyphBmp.width > 0)
		{
			buffRow = (INT)tSize->Height - addHeight - slot->bitmap_top;
			buffRow = (buffRow < 0)? 0 : buffRow;

			for(int row = 0; row < glyphBmp.rows; row++)
			{
				int buffCol2 = buffCol;

				for(int col = 0; col < glyphBmp.width; col++)
				{
					char pix = glyphBmp.buffer[(row * glyphBmp.width) + col];
					unsigned char pixelValNew[PIXEL_WIDTH] = { 255, pix, pix, pix };

					memcpy(pixelBuffer->Data + ((buffRow * (UINT)tSize->Width * PIXEL_WIDTH) + buffCol2), pixelValNew, PIXEL_WIDTH);
					buffCol2 += PIXEL_WIDTH;
				}

				buffRow++;
			}

			buffCol += ((LINE_SPACING + glyphBmp.width) * PIXEL_WIDTH);
		}
		else
		{
			buffCol += ((LINE_SPACING + BLANK_SPACE) * PIXEL_WIDTH);
		}
	}

	return pixelBuffer;
}

bool FTTextPainter::SetFont(Platform::String^ fontName , UINT nSize)
{
	bool bRetVal = true;
	FT_Error error = 0;

	if(nSize > 0 && nSize != m_fontSize)
	{
		m_fontSize = nSize;
	}

	// check if we have ttf file for requested font
	FONTS_MAP_CITR itr = m_fontMap.find(fontName);

	if(m_fontMap.end() != itr)
	{
		m_fontName = fontName;
		Platform::String^ fontFile = itr->second;
		error = FT_New_Face( m_textLibrary, cocos2d::CCUnicodeToUtf8(fontFile->Data()).c_str(), 0, &m_fontFace );
	}else
	{
		// try to load new font		
		const char* fullPath = CCFileUtils::fullPathFromRelativePath(CCUnicodeToUtf8(fontName->Data()).c_str());
		Platform::String^ fullPathRefStr = ref new Platform::String(CCUtf8ToUnicode(fullPath).c_str());

		if(fontName->Length() > 0)
		{
			m_fontMap.insert(std::pair<Platform::String^, Platform::String^>(fontName, fullPathRefStr));
			m_fontName = fontName;
		}
		error = FT_New_Face( m_textLibrary, fullPath, 0, &m_fontFace );
	}

	// set default font
	if(error)
	{
		m_fontName = ref new Platform::String(DEFAULT_FONT_W);
		error = FT_New_Face( m_textLibrary, DEFAULT_FONT_FILE_A, 0, &m_fontFace );
	}

	if(!error)
	{
		error = FT_Set_Char_Size(m_fontFace, 0, m_fontSize * 64, 96, 0);
	}
	else
	{
		bRetVal = false;
	}

	return bRetVal;
}

Platform::String^ FTTextPainter::GetFontStore()
{
	Platform::String^ fontStore;

	// TODO proper method to get fonts folder
	fontStore = "C:\\Windows\\Fonts\\";

	return fontStore;
}

bool FTTextPainter::BuildFontsInformation()
{
	bool bRet = false;

	m_fontMap.clear();

	Platform::String^ fontStore = GetFontStore();
	Platform::String^ srchStrg = "*.*";

	WIN32_FIND_DATAW fndData = { 0 }; 
	HANDLE hFindFile = FindFirstFileExW((fontStore + srchStrg)->Data(), FindExInfoStandard, &fndData, FindExSearchNameMatch, NULL, 0);

	// search for all ttf files and extract font names from them
	if(INVALID_HANDLE_VALUE != hFindFile)
	{
		do
		{
			Platform::String^ filePath = ref new Platform::String(fndData.cFileName);
			filePath = Platform::String::Concat(fontStore, filePath);

			if(wcscmp(fndData.cFileName, L".") != 0 && wcscmp(fndData.cFileName, L"..") != 0)
			{
				bRet = true;
				Platform::String^ fontName = GetFontNameFromFile(filePath);

				if(fontName->Length() > 0)
				{
					m_fontMap.insert(std::pair<Platform::String^, Platform::String^>(fontName, filePath));
				}
			}
		}
		while(FindNextFileW(hFindFile, &fndData));
	}

	return bRet;
}

Platform::String^ FTTextPainter::GetFontNameFromFile(Platform::String^ filePath)
{
	Platform::String^ fontName = "";

	HANDLE hFile = CreateFile2(filePath->Data(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	DWORD bytesRead = GetLastError();

	if(INVALID_HANDLE_VALUE != hFile)
	{
		TTF_OFFSET_TABLE tabOffset = { 0 };
		ReadFile(hFile, &tabOffset, sizeof(TTF_OFFSET_TABLE), &bytesRead, NULL);

		tabOffset.uNumOfTables = SWAPWORD(tabOffset.uNumOfTables);
		tabOffset.uMajorVersion = SWAPWORD(tabOffset.uMajorVersion);
		tabOffset.uMinorVersion = SWAPWORD(tabOffset.uMinorVersion);

		TTF_TABLE_DIRECTORY tblDir = { 0 };;
		bool bValid = false;

		do
		{
			ReadFile(hFile, &tblDir, sizeof(TTF_TABLE_DIRECTORY), &bytesRead, NULL);

			if(strncmp(tblDir.szTag, TAG_NAME_A, 4) == 0)
			{
				bValid = true;
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				break;
			}
		}
		while (bytesRead > 0);

		if(bValid)
		{
			LARGE_INTEGER seek = { 0 };
			seek.QuadPart = tblDir.uOffset;
			SetFilePointerEx(hFile, seek, NULL, FILE_BEGIN);

			TTF_NAME_TABLE_HEADER tabHeader = { 0 };
			ReadFile(hFile, &tabHeader, sizeof(TTF_NAME_TABLE_HEADER), &bytesRead, NULL);

			tabHeader.uNRCount = SWAPWORD(tabHeader.uNRCount);
			tabHeader.uStorageOffset = SWAPWORD(tabHeader.uStorageOffset);

			TTF_NAME_RECORD tabRecord = { 0 };
			bValid = false;

			do
			{
				ReadFile(hFile, &tabRecord, sizeof(TTF_NAME_RECORD), &bytesRead, NULL);
				tabRecord.uNameID = SWAPWORD(tabRecord.uNameID);

				if(tabRecord.uNameID == 1)
				{
					tabRecord.uStringLength = SWAPWORD(tabRecord.uStringLength);
					tabRecord.uStringOffset = SWAPWORD(tabRecord.uStringOffset);

					LARGE_INTEGER oldPointer = { 0 };
					seek.QuadPart = 0;
					SetFilePointerEx(hFile, seek, &oldPointer, FILE_CURRENT);

					seek.QuadPart = tblDir.uOffset + tabRecord.uStringOffset + tabHeader.uStorageOffset;
					SetFilePointerEx(hFile, seek, NULL, FILE_BEGIN);

					CHAR* lpszNameBuf = new CHAR[tabRecord.uStringLength + 1];
					//TCHAR* lpszNameBuf = new TCHAR[tabRecord.uStringLength + 1];
					ZeroMemory(lpszNameBuf, tabRecord.uStringLength + 1);

					ReadFile(hFile, lpszNameBuf, tabRecord.uStringLength, &bytesRead, NULL);
					//ReadFile(hFile, lpszNameBuf, tabRecord.uStringLength * 2, &bytesRead, NULL);

					if(strnlen_s(lpszNameBuf, tabRecord.uStringLength) > 0)
						//if(wcslen(lpszNameBuf) > 0)
					{
						fontName = ref new Platform::String(cocos2d::CCUtf8ToUnicode(lpszNameBuf).c_str());
						//fontName = ref new Platform::String(lpszNameBuf);
						CC_SAFE_DELETE_ARRAY(lpszNameBuf);
						bValid = true;
						break;
					}

					SetFilePointerEx(hFile, oldPointer, NULL, FILE_BEGIN);
				}
			}
			while (bytesRead > 0);
		}

		CloseHandle(hFile);
	}

	return fontName;
}
