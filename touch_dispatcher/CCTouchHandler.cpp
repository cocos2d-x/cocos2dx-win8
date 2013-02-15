/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009      Valentin Milea
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

#include "CCTouchHandler.h"
#include "ccMacros.h"

NS_CC_BEGIN

CCTouchDelegate* CCTouchHandler::getDelegate(void)
{
	return m_pDelegate;
}

void CCTouchHandler::setDelegate(CCTouchDelegate *pDelegate)
{
	/*
	 * RTTI may return null on android. More info please refer issue #926(cocos2d-x)
	 */
	if (pDelegate)
	{
		dynamic_cast<CCObject*>(pDelegate)->retain();
    }

	dynamic_cast<CCObject*>(pDelegate)->retain();

    if (m_pDelegate)
    {
		dynamic_cast<CCObject*>(m_pDelegate)->release();
    }

	m_pDelegate = pDelegate;
}

int CCTouchHandler::getPriority(void)
{
	return m_nPriority;
}

void CCTouchHandler::setPriority(int nPriority)
{
	m_nPriority = nPriority;
}

int CCTouchHandler::getEnabledSelectors(void)
{
	return m_nEnabledSelectors;
}

void CCTouchHandler::setEnalbedSelectors(int nValue)
{
	m_nEnabledSelectors = nValue;
}

CCTouchHandler* CCTouchHandler::handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority)
{
	CCTouchHandler *pHandler = new CCTouchHandler();

	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}
	
	return pHandler;
}

bool CCTouchHandler::initWithDelegate(CCTouchDelegate *pDelegate, int nPriority)
{
	CCAssert(pDelegate != NULL, "touch delegate should not be null");

	m_pDelegate = pDelegate; 

	dynamic_cast<CCObject*>(pDelegate)->retain();

	m_nPriority = nPriority;
	m_nEnabledSelectors = 0;

	return true;
}

CCTouchHandler::~CCTouchHandler(void)
{
	if (m_pDelegate)
	{
		dynamic_cast<CCObject*>(m_pDelegate)->release();
	}   
}

// implementation of CCStandardTouchHandler
bool CCStandardTouchHandler::initWithDelegate(CCTouchDelegate *pDelegate, int nPriority)
{
	if (CCTouchHandler::initWithDelegate(pDelegate, nPriority))
	{
		return true;
	}

	return false;
}

CCStandardTouchHandler* CCStandardTouchHandler::handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority)
{
	CCStandardTouchHandler* pHandler = new CCStandardTouchHandler();

	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}

	return pHandler;
}

// implementation of CCTargetedTouchHandler

bool CCTargetedTouchHandler::isSwallowsTouches(void)
{
	return m_bSwallowsTouches;
}

void CCTargetedTouchHandler::setSwallowsTouches(bool bSwallowsTouches)
{
	m_bSwallowsTouches = bSwallowsTouches;
}

CCSet* CCTargetedTouchHandler::getClaimedTouches(void)
{
	return m_pClaimedTouches;
}

CCTargetedTouchHandler* CCTargetedTouchHandler::handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow)
{
	CCTargetedTouchHandler *pHandler = new CCTargetedTouchHandler();
	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority, bSwallow))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}

	return pHandler;
}

bool CCTargetedTouchHandler::initWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow)
{
	if (CCTouchHandler::initWithDelegate(pDelegate, nPriority))
	{
		m_pClaimedTouches = new CCSet();
		m_bSwallowsTouches = bSwallow;

		return true;
	}

	return false;
}

CCTargetedTouchHandler::~CCTargetedTouchHandler(void)
{
	CC_SAFE_RELEASE(m_pClaimedTouches);
}
NS_CC_END
