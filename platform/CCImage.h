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

#ifndef __CC_IMAGE_H_YANGWS_20110115__
#define __CC_IMAGE_H_YANGWS_20110115__

#include "CCCommon.h"
//#include <wincodec.h>
#include "png.h"
//#include "wincodec_h.h"

NS_CC_BEGIN;

class CC_DLL CCImage
{
public:
    CCImage();
    ~CCImage();

    typedef enum
    {
        kFmtJpg = 0,
        kFmtPng,
		kFmtRawData,
		kFmtUnKnown
    }EImageFormat;

    typedef enum
    {
        kAlignCenter        = 0x33, ///< Horizontal center and vertical center.
        kAlignTop           = 0x13, ///< Horizontal center and vertical top.
        kAlignTopRight      = 0x12, ///< Horizontal right and vertical top.
        kAlignRight         = 0x32, ///< Horizontal right and vertical center.
        kAlignBottomRight   = 0x22, ///< Horizontal right and vertical bottom.
        kAlignBottom        = 0x23, ///< Horizontal center and vertical bottom.
        kAlignBottomLeft    = 0x21, ///< Horizontal left and vertical bottom.
        kAlignLeft          = 0x31, ///< Horizontal left and vertical center.
        kAlignTopLeft       = 0x11, ///< Horizontal left and vertical top.
    }ETextAlign;

    /** 
    @brief  Load the image from the specified path. 
    @param strPath   the absolute file path
    @param imageType the type of image, now only support tow types.
    @return  true if load correctly
    */
    bool initWithImageFile(const char * strPath, EImageFormat imageType = kFmtPng);

	/**
	 @brief The same meaning as initWithImageFile, but it is thread safe. It is casued by
	        loadImage() in CCTextureCache.cpp.
	 @param fullpath  full path of the file   
     @param imageType the type of image, now only support tow types.
     @return  true if load correctly
	 */
	bool initWithImageFileThreadSafe(const char *fullpath, EImageFormat imageType = kFmtPng);

    /**
    @brief  Load image from stream buffer.

	@warning kFmtRawData only support RGBA8888
    @param pBuffer  stream buffer that hold the image data
    @param nLength  the length of data(managed in byte)
	@param nWidth, nHeight, nBitsPerComponent are used for kFmtRawData
    @return true if load correctly
    */
    bool initWithImageData(void * pData, 
						   int nDataLen, 
						   EImageFormat eFmt = kFmtPng,
						   int nWidth = 0,
						   int nHeight = 0,
						   int nBitsPerComponent = 8);

    /**
    @brief	Create image with specified string.
    @param  pText       the text which the image show, nil cause init fail
    @param  nWidth      the image width, if 0, the width match the text's width
    @param  nHeight     the image height, if 0, the height match the text's height
    @param  eAlignMask  the test Alignment
    @param  pFontName   the name of the font which use to draw the text. If nil, use the default system font.
    @param  nSize       the font size, if 0, use the system default size.
    */
    bool initWithString(
        const char *    pText, 
        int             nWidth = 0, 
        int             nHeight = 0,
        ETextAlign      eAlignMask = kAlignCenter,
        const char *    pFontName = 0,
        int             nSize = 0);

    unsigned char *   getData()               { return m_pData; }
    int         getDataLen()            { return m_nWidth * m_nHeight; }

    bool hasAlpha()                     { return m_bHasAlpha; }
    bool isPremultipliedAlpha()         { return m_bPreMulti; }

    void release();

    /**
    @brief	Save the CCImage data to specified file with specified format.
	@param	pszFilePath		the file's absolute path, including file subfix
	@param	bIsToRGB		if the image is saved as RGB format
    */
    bool saveToFile(const char *pszFilePath, bool bIsToRGB = true);

    CC_SYNTHESIZE_READONLY(short,   m_nWidth,       Width);
    CC_SYNTHESIZE_READONLY(short,   m_nHeight,      Height);
    CC_SYNTHESIZE_READONLY(int,     m_nBitsPerComponent,   BitsPerComponent);

protected:
    bool _initWithJpgData(void *pData, int nDatalen);
    bool _initWithPngData(void *pData, int nDatalen);

	// @warning kFmtRawData only support RGBA8888
	bool _initWithRawData(void *pData, int nDatalen, int nWidth, int nHeight, int nBitsPerComponent);

	bool _saveImageToPNG(const char *pszFilePath, bool bIsToRGB = true);
	bool _saveImageToJPG(const char *pszFilePath);

    unsigned char *m_pData;
    bool m_bHasAlpha;
    bool m_bPreMulti;

private:
    // noncopyable
    CCImage(const CCImage&    rImg);
    CCImage & operator=(const CCImage&);
};

NS_CC_END;

#endif	// __CC_IMAGE_H_YANGWS_20110115__
