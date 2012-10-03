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

#include "CCObject.h"
#include "CCAutoreleasePool.h"
#include "ccMacros.h"
namespace   cocos2d {

CCObject* CCCopying::copyWithZone(CCZone *pZone)
{
    CC_UNUSED_PARAM(pZone);
	CCAssert(0, "not implement");
    return 0;
}


CCObject::CCObject(void)
{
	static unsigned int uObjectCount = 0;

	m_uID = ++uObjectCount;

	// when the object is created, the refrence count of it is 1
	m_uReference = 1;
	m_bManaged = false;
}

CCObject::~CCObject(void)
{
	// if the object is managed, we should remove it
	// from pool manager
	if (m_bManaged)
	{
		CCPoolManager::getInstance()->removeObject(this);
	}
}

CCObject* CCObject::copy()
{
        return copyWithZone(0);
}

void CCObject::release(void)
{
	CCAssert(m_uReference > 0, "reference count should greater than 0");
	--m_uReference;

	if (m_uReference == 0)
	{
		delete this;
	}
}

void CCObject::retain(void)
{
	CCAssert(m_uReference > 0, "reference count should greater than 0");

	++m_uReference;
}

CCObject* CCObject::autorelease(void)
{
	CCPoolManager::getInstance()->addObject(this);

	m_bManaged = true;
	return this;
}

bool CCObject::isSingleRefrence(void)
{
	return m_uReference == 1;
}

unsigned int CCObject::retainCount(void)
{
	return m_uReference;
}

bool CCObject::isEqual(const CCObject *pObject)
{
	return this == pObject;
}
}//namespace   cocos2d 
