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

NS_CC_BEGIN

CCAnimationFrame::CCAnimationFrame()
: m_pSpriteFrame(NULL)
, m_fDelayUnits(0.0f)
//, m_pUserInfo(NULL)
{

}

//bool CCAnimationFrame::initWithSpriteFrame(CCSpriteFrame* spriteFrame, float delayUnits, CCDictionary* userInfo)
//{
//    setSpriteFrame(spriteFrame);
//    setDelayUnits(delayUnits);
//    setUserInfo(userInfo);
//
//    return true;
//}

CCAnimationFrame::~CCAnimationFrame()
{    
    CCLOGINFO( "cocos2d: deallocing %s", this);

    CC_SAFE_RELEASE(m_pSpriteFrame);
//    CC_SAFE_RELEASE(m_pUserInfo);
}

float CCAnimation::getDuration(void)
{
    return m_fTotalDelayUnits * m_fDelayPerUnit;
}

CCObject* CCAnimationFrame::copyWithZone(CCZone* pZone)
{
	CCAssert(false, "Not implemented!");
    CCZone* pNewZone = NULL;
    CCAnimationFrame* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCAnimationFrame*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCAnimationFrame();
        pNewZone = new CCZone(pCopy);
    }

    //pCopy->initWithSpriteFrame((CCSpriteFrame*)m_pSpriteFrame->copy()->autorelease(),
    //    m_fDelayUnits, m_pUserInfo != NULL ? (CCDictionary*)m_pUserInfo->copy()->autorelease() : NULL);

    //CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

// implementation of CCAnimation
	CCAnimation* CCAnimation::create()
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

CCAnimation* CCAnimation::create(CCArray *frames)
{
	CCAnimation *pAnimation = new CCAnimation();
	pAnimation->initWithFrames(frames);
	pAnimation->autorelease();

	return pAnimation;
}

bool CCAnimation::initWithSpriteFrames(CCArray *pFrames, float delay/* = 0.0f*/)
{
    CCARRAY_VERIFY_TYPE(pFrames, CCSpriteFrame*);

	CCAssert(false, "Not implemented!");
    //m_uLoops = 1;
    //m_fDelayPerUnit = delay;
    //CCArray* pTmpFrames = CCArray::create();
    //setFrames(pTmpFrames);

    //if (pFrames != NULL)
    //{
    //    CCObject* pObj = NULL;
    //    CCARRAY_FOREACH(pFrames, pObj)
    //    {
    //        CCSpriteFrame* frame = (CCSpriteFrame*)pObj;
    //        CCAnimationFrame *animFrame = new CCAnimationFrame();
    //        animFrame->initWithSpriteFrame(frame, 1, NULL);
    //        m_pFrames->addObject(animFrame);
    //        animFrame->release();

    //        m_fTotalDelayUnits++;
    //    }
    //}

    return true;
}


CCAnimation* CCAnimation::createWithSpriteFrames(CCArray *frames, float delay/* = 0.0f*/)
{
    CCAnimation *pAnimation = new CCAnimation();
    pAnimation->initWithSpriteFrames(frames, delay);
    pAnimation->autorelease();

    return pAnimation;
}

CCAnimation* CCAnimation::create(CCArray *frames, float delay)
{
	CCAnimation *pAnimation = new CCAnimation();
	pAnimation->initWithFrames(frames, delay);
	pAnimation->autorelease();

	return pAnimation;
}

bool CCAnimation::initWithFrames(CCArray *pFrames, float delay)
{
	m_fDelay = delay;
	m_pobFrames = CCArray::arrayWithArray(pFrames);
	m_pobFrames->retain();

	return true;
}

bool CCAnimation::initWithFrames(CCArray *pFrames)
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
	CCSpriteFrame *pFrame = CCSpriteFrame::createWithTexture(pTexture, rect);
	m_pobFrames->addObject(pFrame);
}

void CCAnimation::addSpriteFrameWithFileName(const char *pszFileName)
{
    CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(pszFileName);
    CCRect rect = CCRectZero;
    rect.size = pTexture->getContentSize();
    CCSpriteFrame *pFrame = CCSpriteFrame::createWithTexture(pTexture, rect);
    addSpriteFrame(pFrame);
}
void CCAnimation::addFrameWithTexture(CCTexture2D *pobTexture, const CCRect& rect)
{
	CCSpriteFrame *pFrame = CCSpriteFrame::createWithTexture(pobTexture, rect);
	m_pobFrames->addObject(pFrame);
}
NS_CC_END