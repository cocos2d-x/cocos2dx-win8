/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2011 Ricardo Quesada
* Copyright (c) 2011      Zynga Inc.
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

#include "CCSpriteFrame.h"
#include "CCDirector.h"

NS_CC_BEGIN
// implementation of CCSpriteFrame

CCSpriteFrame* CCSpriteFrame::createWithTexture(CCTexture2D *pobTexture, const CCRect& rect)
{
	CCSpriteFrame *pSpriteFrame = new CCSpriteFrame();;
	pSpriteFrame->initWithTexture(pobTexture, rect);
	pSpriteFrame->autorelease();

	return pSpriteFrame;
}

CCSpriteFrame* CCSpriteFrame::createWithTexture(CCTexture2D* pobTexture, const CCRect& rect, bool rotated, const CCPoint& offset, const CCSize& originalSize)
{
    CCSpriteFrame *pSpriteFrame = new CCSpriteFrame();;
	pSpriteFrame->initWithTexture(pobTexture, rect, rotated, offset, originalSize);
	pSpriteFrame->autorelease();

	return pSpriteFrame;
}

bool CCSpriteFrame::initWithTexture(CCTexture2D* pobTexture, const CCRect& rect)
{
	CCRect rectInPixels = CC_RECT_POINTS_TO_PIXELS(rect);
	return initWithTexture(pobTexture, rectInPixels, false, CCPointZero, rectInPixels.size);
}

bool CCSpriteFrame::initWithTexture(CCTexture2D* pobTexture, const CCRect& rect, bool rotated, const CCPoint& offset, const CCSize& originalSize)
{
	m_pobTexture = pobTexture;

    if (pobTexture)
    {
        pobTexture->retain();
    }

	m_obRectInPixels = rect;
	m_obRect = CC_RECT_PIXELS_TO_POINTS(rect);
	m_bRotated = rotated;
	m_obOffsetInPixels = offset;

	m_obOriginalSizeInPixels = originalSize;

	return true;
}

CCSpriteFrame::~CCSpriteFrame(void)
{
	CCLOGINFO("cocos2d: deallocing %p", this);
	CC_SAFE_RELEASE(m_pobTexture);
}

CCObject* CCSpriteFrame::copyWithZone(CCZone *pZone)
{
    CC_UNUSED_PARAM(pZone);
	CCSpriteFrame *pCopy = new CCSpriteFrame();
	
	pCopy->initWithTexture(m_pobTexture, m_obRectInPixels, m_bRotated, m_obOffsetInPixels, m_obOriginalSizeInPixels);
	
	return pCopy;
}

void CCSpriteFrame::setRect(const CCRect& rect)
{
    m_obRect = rect;
    m_obRectInPixels = CC_RECT_POINTS_TO_PIXELS(m_obRect);
}

void CCSpriteFrame::setRectInPixels(const CCRect& rectInPixels)
{
    m_obRectInPixels = rectInPixels;
    m_obRect = CC_RECT_PIXELS_TO_POINTS(rectInPixels);
}

NS_CC_END
