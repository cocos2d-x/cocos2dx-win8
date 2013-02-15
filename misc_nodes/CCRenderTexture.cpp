/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009      Jason Booth
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

#include "CCConfiguration.h"
#include "CCRenderTexture.h"
#include "CCDirector.h"
#include "platform/platform.h"
#include "CCImage.h"
#include "support/ccUtils.h"
#include "CCTextureCache.h"
#include "CCFileUtils.h"
#include "CCGL.h"

namespace cocos2d { 

// implementation CCRenderTexture
CCRenderTexture::CCRenderTexture()
: m_pSprite(NULL)
, m_uFBO(0)
, m_nOldFBO(0)
, m_pTexture(0)
, m_pUITextureImage(NULL)
, m_ePixelFormat(kCCTexture2DPixelFormat_RGBA8888)
{
}

CCRenderTexture::~CCRenderTexture()
{
	Shutdown();
    removeAllChildrenWithCleanup(true);
    //ccglDeleteFramebuffers(1, &m_uFBO);

	CC_SAFE_DELETE(m_pUITextureImage);
}

CCSprite * CCRenderTexture::getSprite()
{
	return m_pSprite;
}
void CCRenderTexture::setSprite(CCSprite* var)
{
	m_pSprite = var;
}

CCRenderTexture * CCRenderTexture::renderTextureWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat)
{
    CCRenderTexture *pRet = new CCRenderTexture();

    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCRenderTexture * CCRenderTexture::renderTextureWithWidthAndHeight(int w, int h)
{
	CCRenderTexture *pRet = new CCRenderTexture();

    if(pRet && pRet->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet)
	return NULL;
}

bool CCRenderTexture::initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat)
{
	// If the gles version is lower than GLES_VER_1_0, 
	// some extended gles functions can't be implemented, so return false directly.
	if (CCConfiguration::sharedConfiguration()->getGlesVersion() <= GLES_VER_1_0)
	{
		return false;
	}

    bool bRet = false;
    do 
    {
		w *= (int)CC_CONTENT_SCALE_FACTOR();
		h *= (int)CC_CONTENT_SCALE_FACTOR();

        // textures must be power of two squared
        unsigned int powW = w;//=ccNextPOT(w);
        unsigned int powH = h;//=ccNextPOT(h);

        void *data = malloc((int)(powW * powH * 4));
        CC_BREAK_IF(! data);

        memset(data, 0, (int)(powW * powH * 4));
        m_ePixelFormat = eFormat;

        m_pTexture = new CCTexture2D();
        CC_BREAK_IF(! m_pTexture);

		m_pTexture->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, CCSizeMake((float)w, (float)h));
		free( data );

		Initialize(CCID3D11Device, powW, powH);

        m_pTexture->setAliasTexParameters();

        m_pSprite = CCSprite::spriteWithTexture(m_pTexture);

		m_pTexture->release();
		//=m_pSprite->setScaleY(-1);
		this->addChild(m_pSprite);

        ccBlendFunc tBlendFunc = {CC_ONE, CC_ONE_MINUS_SRC_ALPHA };
        m_pSprite->setBlendFunc(tBlendFunc);

        bRet = true;
    } while (0);
    return bRet;
    
}

void CCRenderTexture::begin()
{
	// Save the current matrix
	CCD3DCLASS->D3DPushMatrix();
	SetRenderTarget(CCD3DCLASS->GetDeviceContext(), CCD3DCLASS->GetDepthStencilView());

	const CCSize& texSize = m_pTexture->getContentSizeInPixels();

	// Calculate the adjustment ratios based on the old and new projections
	CCSize size = CCDirector::sharedDirector()->getDisplaySizeInPixels();
	float widthRatio = size.width / texSize.width;
	float heightRatio = size.height / texSize.height;

	// Adjust the orthographic propjection and viewport
	CCD3DCLASS->D3DOrtho((float)-1.0 / widthRatio,  (float)1.0 / widthRatio, (float)-1.0 / heightRatio, (float)1.0 / heightRatio, -1,1);
	CCD3DCLASS->D3DViewport(0, 0, (int)texSize.width, (int)texSize.height);

}

void CCRenderTexture::beginWithClear(float r, float g, float b, float a)
{
    this->begin();

	float	clearColor[4];
	CCD3DCLASS->GetClearColor(clearColor);
	CCD3DCLASS->D3DClearColor(r, g, b, a);
	ClearRenderTarget(CCD3DCLASS->GetDeviceContext(), CCD3DCLASS->GetDepthStencilView());
	CCD3DCLASS->D3DClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}

void CCRenderTexture::end(bool bIsTOCacheTexture)
{
	
	// Restore the original matrix and viewport
	CCD3DCLASS->D3DPopMatrix();
	CCD3DCLASS->SetBackBufferRenderTarget();

	CCSize size = CCDirector::sharedDirector()->getDisplaySizeInPixels();

	CCDirector::sharedDirector()->getOpenGLView()->setViewPortInPoints(0, 0, size.width, size.height);

#if CC_ENABLE_CACHE_TEXTTURE_DATA
	if (bIsTOCacheTexture)
	{
		CC_SAFE_DELETE(m_pUITextureImage);

		// to get the rendered texture data
		const CCSize& s = m_pTexture->getContentSizeInPixels();
		int tx = (int)s.width;
		int ty = (int)s.height;
		m_pUITextureImage = new CCImage;
		if (true == getUIImageFromBuffer(m_pUITextureImage, 0, 0, tx, ty))
		{
			VolatileTexture::addDataTexture(m_pTexture, m_pUITextureImage->getData(), kTexture2DPixelFormat_RGBA8888, s);
		} 
		else
		{
			CCLOG("Cache rendertexture failed!");
		}
	}
#endif
}

void CCRenderTexture::clear(float r, float g, float b, float a)
{
	this->beginWithClear(r, g, b, a);
	this->end();
}

bool CCRenderTexture::saveBuffer(const char *szFilePath, int x, int y, int nWidth, int nHeight)
{
	bool bRet = false;

	CCImage *pImage = new CCImage();
	if (pImage != NULL && getUIImageFromBuffer(pImage, x, y, nWidth, nHeight))
	{
		bRet = false;//pImage->saveToFile(szFilePath);
	}

	CC_SAFE_DELETE(pImage);
	return bRet;
}
bool CCRenderTexture::saveBuffer(int format, const char *fileName, int x, int y, int nWidth, int nHeight)
{
	bool bRet = false;
	CCAssert(format == kCCImageFormatJPG || format == kCCImageFormatPNG,
			 "the image can only be saved as JPG or PNG format");

	CCImage *pImage = new CCImage();
	if (pImage != NULL && getUIImageFromBuffer(pImage, x, y, nWidth, nHeight))
	{
		std::string fullpath = CCFileUtils::getWriteablePath() + fileName;
		
		bRet = false;//pImage->saveToFile(fullpath.c_str());
	}

	CC_SAFE_DELETE(pImage);

	return bRet;
}

/* get buffer as UIImage */
bool CCRenderTexture::getUIImageFromBuffer(CCImage *pImage, int x, int y, int nWidth, int nHeight)
{
	if (NULL == pImage || NULL == m_pTexture)
	{
		return false;
	}

	const CCSize& s = m_pTexture->getContentSizeInPixels();
	int tx = (int)s.width;
	int ty = (int)s.height;

	if (x < 0 || x >= tx || y < 0 || y >= ty)
	{
		return false;
	}

	if (nWidth < 0 
		|| nHeight < 0
		|| (0 == nWidth && 0 != nHeight)
		|| (0 == nHeight && 0 != nWidth))
	{
		return false;
	}
	
	// to get the image size to save
	//		if the saving image domain exeeds the buffer texture domain,
	//		it should be cut
	int nSavedBufferWidth = nWidth;
	int nSavedBufferHeight = nHeight;
	if (0 == nWidth)
	{
		nSavedBufferWidth = tx;
	}
	if (0 == nHeight)
	{
		nSavedBufferHeight = ty;
	}
	nSavedBufferWidth = x + nSavedBufferWidth > tx ? (tx - x): nSavedBufferWidth;
	nSavedBufferHeight = y + nSavedBufferHeight > ty ? (ty - y): nSavedBufferHeight;

	CCubyte *pBuffer = NULL;
	CCubyte *pTempData = NULL;
	bool bRet = false;

	do
	{
		CCAssert(m_ePixelFormat == kCCTexture2DPixelFormat_RGBA8888, "only RGBA8888 can be saved as image");

		CC_BREAK_IF(! (pBuffer = new CCubyte[nSavedBufferWidth * nSavedBufferHeight * 4]));

		// On some machines, like Samsung i9000, Motorola Defy,
		// the dimension need to be a power of 2
		int nReadBufferWidth = 0;
		int nReadBufferHeight = 0;
		int nMaxTextureSize = 2048;//0;

		nReadBufferWidth = tx;//=ccNextPOT(tx);
		nReadBufferHeight = ty;//=ccNextPOT(ty);

		CC_BREAK_IF(0 == nReadBufferWidth || 0 == nReadBufferHeight);
		CC_BREAK_IF(nReadBufferWidth > nMaxTextureSize || nReadBufferHeight > nMaxTextureSize);

		CC_BREAK_IF(! (pTempData = new CCubyte[nReadBufferWidth * nReadBufferHeight * 4]));

		this->begin();

		/************************************************************************/
		ID3D11Texture2D*                pStagingTexture = NULL;
		D3D11_MAPPED_SUBRESOURCE  Subresource;
		D3D11_TEXTURE2D_DESC RTDESC;
		ZeroMemory(&RTDESC, sizeof(D3D11_TEXTURE2D_DESC));

		ID3D11ShaderResourceView* tmpShaderResourceView=m_pTexture->getTextureResource();
		ID3D11Resource* tmpResource;
		tmpShaderResourceView->GetResource(&tmpResource);
		((ID3D11Texture2D*)tmpResource)->GetDesc(&RTDESC);

		RTDESC.Usage = D3D11_USAGE_STAGING;
		RTDESC.BindFlags = 0;
		RTDESC.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		CCEGLView* eglView = CCDirector::sharedDirector()->getOpenGLView();
		if(FAILED(eglView->GetDevice()->CreateTexture2D(&RTDESC,NULL,&pStagingTexture)))
		{
			// e_fail
		}

		eglView->GetDeviceContext()->CopyResource(pStagingTexture, tmpResource);

		eglView->GetDeviceContext()->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &Subresource);

		//void* pData = Subresource.pData;
		memcpy(pTempData,Subresource.pData,sizeof(byte)*nReadBufferWidth * nReadBufferHeight * 4);

		eglView->GetDeviceContext()->Unmap(pStagingTexture, 0);

		if ( pStagingTexture )
		{
			pStagingTexture->Release();
			pStagingTexture = 0;
		}
		/************************************************************************/
		this->end(false);

		// to get the actual texture data 
		// #640 the image read from rendertexture is upseted
		for (int i = 0; i < nSavedBufferHeight; ++i)
		{
			memcpy(&pBuffer[i * nSavedBufferWidth * 4], 
				&pTempData[(y + nSavedBufferHeight - i - 1) * nReadBufferWidth * 4 + x * 4], 
				nSavedBufferWidth * 4);
		}

		bRet = false;//pImage->initWithImageData(pBuffer, nSavedBufferWidth * nSavedBufferHeight * 4, CCImage::kFmtRawData, nSavedBufferWidth, nSavedBufferHeight, 8);
	} while (0);

	CC_SAFE_DELETE_ARRAY(pBuffer);
	CC_SAFE_DELETE_ARRAY(pTempData);

	return bRet;
}


CCData * CCRenderTexture::getUIImageAsDataFromBuffer(int format)
{
    CC_UNUSED_PARAM(format);
    CCData *  pData     = NULL;
//@ todo CCRenderTexture::getUIImageAsDataFromBuffer

// #include "Availability.h"
// #include "UIKit.h"

//     CCubyte * pBuffer   = NULL;
//     CCubyte * pPixels   = NULL;
//     do 
//     {
//         CC_BREAK_IF(! m_pTexture);
// 
//         CCAssert(m_ePixelFormat == kCCTexture2DPixelFormat_RGBA8888, "only RGBA8888 can be saved as image");
// 
//         const CCSize& s = m_pTexture->getContentSizeInPixels();
//         int tx = s.width;
//         int ty = s.height;
// 
//         int bitsPerComponent = 8;
//         int bitsPerPixel = 32;
// 
//         int bytesPerRow = (bitsPerPixel / 8) * tx;
//         int myDataLength = bytesPerRow * ty;
// 
//         CC_BREAK_IF(! (pBuffer = new CCubyte[tx * ty * 4]));
//         CC_BREAK_IF(! (pPixels = new CCubyte[tx * ty * 4]));
// 
//         this->begin();
//         glReadPixels(0,0,tx,ty,CC_RGBA,CC_UNSIGNED_BYTE, pBuffer);
//         this->end();
// 
//         int x,y;
// 
//         for(y = 0; y <ty; y++) {
//             for(x = 0; x <tx * 4; x++) {
//                 pPixels[((ty - 1 - y) * tx * 4 + x)] = pBuffer[(y * 4 * tx + x)];
//             }
//         }
// 
//         if (format == kCCImageFormatRawData)
//         {
//             pData = CCData::dataWithBytesNoCopy(pPixels, myDataLength);
//             break;
//         }

        //@ todo impliment save to jpg or png
        /*
        CGImageCreate(size_t width, size_t height,
        size_t bitsPerComponent, size_t bitsPerPixel, size_t bytesPerRow,
        CGColorSpaceRef space, CGBitmapInfo bitmapInfo, CGDataProviderRef provider,
        const CGFloat decode[], bool shouldInterpolate,
        CGColorRenderingIntent intent)
        */
        // make data provider with data.
//         CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault;
//         CGDataProviderRef provider		= CGDataProviderCreateWithData(NULL, pixels, myDataLength, NULL);
//         CGColorSpaceRef colorSpaceRef	= CGColorSpaceCreateDeviceRGB();
//         CGImageRef iref					= CGImageCreate(tx, ty,
//             bitsPerComponent, bitsPerPixel, bytesPerRow,
//             colorSpaceRef, bitmapInfo, provider,
//             NULL, false,
//             kCGRenderingIntentDefault);
// 
//         UIImage* image					= [[UIImage alloc] initWithCGImage:iref];
// 
//         CGImageRelease(iref);	
//         CGColorSpaceRelease(colorSpaceRef);
//         CGDataProviderRelease(provider);
// 
// 
// 
//         if (format == kCCImageFormatPNG)
//             data = UIImagePNGRepresentation(image);
//         else
//             data = UIImageJPEGRepresentation(image, 1.0f);
// 
//         [image release];
//     } while (0);
//     
//     CC_SAFE_DELETE_ARRAY(pBuffer);
//     CC_SAFE_DELETE_ARRAY(pPixels);
	return pData;
}



void CCRenderTexture::CopyResource()
{
	ID3D11Texture2D*                pStagingTexture = NULL;
	D3D11_MAPPED_SUBRESOURCE  Subresource;
	D3D11_TEXTURE2D_DESC RTDESC;
	ZeroMemory(&RTDESC, sizeof(D3D11_TEXTURE2D_DESC));

	ID3D11ShaderResourceView* tmpShaderResourceView=m_pTexture->getTextureResource();
	ID3D11Resource* tmpResource;
	tmpShaderResourceView->GetResource(&tmpResource);
	((ID3D11Texture2D*)tmpResource)->GetDesc(&RTDESC);

	RTDESC.Usage = D3D11_USAGE_STAGING;
	RTDESC.BindFlags = 0;
	RTDESC.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	CCEGLView* eglView = CCDirector::sharedDirector()->getOpenGLView();
	if(FAILED(eglView->GetDevice()->CreateTexture2D(&RTDESC,NULL,&pStagingTexture)))
	{
		// e_fail
	}

	eglView->GetDeviceContext()->CopyResource(pStagingTexture, tmpResource);

	eglView->GetDeviceContext()->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &Subresource);

	void* pData = Subresource.pData;

	eglView->GetDeviceContext()->Unmap(pStagingTexture, 0);

	if ( pStagingTexture )
	{
		pStagingTexture->Release();
		pStagingTexture = 0;
	}
	
}

bool CCRenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	ID3D11ShaderResourceView* tmpShaderResourceView=m_pTexture->getTextureResource();
	ID3D11Resource* tmpResource;
	tmpShaderResourceView->GetResource(&tmpResource);

	D3D11_TEXTURE2D_DESC pDesc;
	((ID3D11Texture2D*)tmpResource)->GetDesc(&pDesc);
	ZeroMemory(&renderTargetViewDesc,sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = pDesc.Format;//textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(tmpResource, &renderTargetViewDesc, &m_renderTargetView);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void CCRenderTexture::Shutdown()
{

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	return;
}


void CCRenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}


void CCRenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	CCD3DCLASS->clearRender(m_renderTargetView);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}
} // namespace cocos2d
