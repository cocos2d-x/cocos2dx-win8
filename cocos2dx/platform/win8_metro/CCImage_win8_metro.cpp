/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#include "DirectXRender.h"

NS_CC_BEGIN;

bool CCImage::initWithString(
	const char *    pText, 
	int             nWidth/* = 0*/, 
	int             nHeight/* = 0*/,
	ETextAlign      eAlignMask/* = kAlignCenter*/,
	const char *    pFontName/* = nil*/,
	int             nSize/* = 0*/)
{
	bool bRet = false;
	unsigned char * pImageData = 0;

	do{
		CC_BREAK_IF(! pText);  

		//D2DWraper& wraper = sharedD2DWraper();
		DXTextPainter^ painter = DirectXRender::SharedDXRender()->m_textPainter;
		std::wstring wStrFontName = CCUtf8ToUnicode(pFontName);
		bool isSuccess = painter->SetFont(ref new Platform::String(wStrFontName.c_str()), nSize);
		if (! isSuccess)
        {
            CCLog("Can't found font(%s), use system default", pFontName);
        }

		
		Windows::Foundation::Size size((float)nWidth, (float)nHeight);

		TextAlignment alignmet = TextAlignment::TextAlignmentCenter;
		DWORD dwHoriFlag = eAlignMask & 0x0f;
		//set text alignment
		switch (dwHoriFlag)
		{
		case 1: // left
				alignmet = TextAlignment::TextAlignmentLeft;
			break;
		case 2: // right
			alignmet = TextAlignment::TextAlignmentRight;
			break;
		case 3: // center
			alignmet = TextAlignment::TextAlignmentCenter;
			break;
		}

		Platform::Array<byte>^ pixelData;
		std::wstring wStrText = CCUtf8ToUnicode(pText);
		pixelData = painter->DrawTextToImage(ref new Platform::String(wStrText.c_str()), &size, alignmet);
		if(pixelData == nullptr)
		{
			break;
		}

		pImageData = new unsigned char[pixelData->Length];
		memcpy(pImageData,pixelData->Data,pixelData->Length);

		CC_BREAK_IF(! pImageData);

		m_nWidth    = (short)size.Width;
		m_nHeight   = (short)size.Height;
		m_bHasAlpha = true;
		m_bPreMulti = false;
		m_pData     = pImageData;
		pImageData  = 0;
		m_nBitsPerComponent = 8;

		bRet = true;
	}while(0);

	return bRet;
}


NS_CC_END;
