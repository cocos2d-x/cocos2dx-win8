/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2010      Ricardo Quesada
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
#include "CCAnimationCache.h"
#include "ccMacros.h"
#include "CCAnimation.h"
#include "CCSpriteFrame.h"

namespace cocos2d
{
	CCAnimationCache* CCAnimationCache::s_pSharedAnimationCache = NULL;

	CCAnimationCache* CCAnimationCache::sharedAnimationCache(void)
	{
        if (! s_pSharedAnimationCache)
		{
			s_pSharedAnimationCache = new CCAnimationCache();
			s_pSharedAnimationCache->init();
		}

		return s_pSharedAnimationCache;
	}

	void CCAnimationCache::purgeSharedAnimationCache(void)
	{
        CC_SAFE_RELEASE_NULL(s_pSharedAnimationCache);
	}

	bool CCAnimationCache::init()
	{
		m_pAnimations = new CCMutableDictionary<std::string, CCAnimation*>();
		return true;
	}

	CCAnimationCache::CCAnimationCache()
    : m_pAnimations(NULL)
	{
	}

	CCAnimationCache::~CCAnimationCache()
	{
		CCLOGINFO("cocos2d: deallocing %p", this);
		CC_SAFE_RELEASE(m_pAnimations);
	}

	void CCAnimationCache::addAnimation(CCAnimation *animation, const char * name)
	{
		m_pAnimations->setObject(animation, std::string(name));
	}

	void CCAnimationCache::removeAnimationByName(const char* name)
	{
        if (! name)
		{
			return;
		}

		m_pAnimations->removeObjectForKey(std::string(name));
	}

	CCAnimation* CCAnimationCache::animationByName(const char* name)
	{
		return m_pAnimations->objectForKey(std::string(name));
	}
}