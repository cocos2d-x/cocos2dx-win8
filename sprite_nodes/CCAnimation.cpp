/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
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
#include "CCAnimation.h"
#include "CCTextureCache.h"
#include "CCTexture2D.h"
#include "ccMacros.h"
#include "CCSpriteFrame.h"

namespace cocos2d
{
	// implementation of CCAnimation

	CCAnimation* CCAnimation::animation()
	{
		CCAnimation *pAnimation = new CCAnimation();
		pAnimation->init();
		pAnimation->autorelease();

		return pAnimation;
	} 

	bool CCAnimation::init()
	{
		return initWithFrames(NULL, 0);
	}

	CCAnimation* CCAnimation::animationWithFrames(CCMutableArray<CCSpriteFrame*> *frames)
	{
		CCAnimation *pAnimation = new CCAnimation();
		pAnimation->initWithFrames(frames);
		pAnimation->autorelease();

		return pAnimation;
	}

	CCAnimation* CCAnimation::animationWithFrames(CCMutableArray<CCSpriteFrame*> *frames, float delay)
	{
		CCAnimation *pAnimation = new CCAnimation();
		pAnimation->initWithFrames(frames, delay);
		pAnimation->autorelease();

		return pAnimation;
	}

	bool CCAnimation::initWithFrames(CCMutableArray<CCSpriteFrame*> *pFrames, float delay)
	{
        m_fDelay = delay;
		m_pobFrames = CCMutableArray<CCSpriteFrame*>::arrayWithArray(pFrames);
        m_pobFrames->retain();
        
		return true;
	}

	bool CCAnimation::initWithFrames(CCMutableArray<CCSpriteFrame*> *pFrames)
	{
        return initWithFrames(pFrames, 0);
	}

	CCAnimation::~CCAnimation(void)
	{
		CCLOGINFO("cocos2d, deallocing %p", this);
		// [name_ release];
		m_nameStr.clear();
		CC_SAFE_RELEASE(m_pobFrames);
	}

	void CCAnimation::addFrame(CCSpriteFrame *pFrame)
	{
		m_pobFrames->addObject(pFrame);
	}

	void CCAnimation::addFrameWithFileName(const char *pszFileName)
	{
		CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(pszFileName);
		CCRect rect = CCRectZero;
		rect.size = pTexture->getContentSize();
		CCSpriteFrame *pFrame = CCSpriteFrame::frameWithTexture(pTexture, rect);
		m_pobFrames->addObject(pFrame);
	}

	void CCAnimation::addFrameWithTexture(CCTexture2D *pobTexture, const CCRect& rect)
	{
		CCSpriteFrame *pFrame = CCSpriteFrame::frameWithTexture(pobTexture, rect);
		m_pobFrames->addObject(pFrame);
	}
}