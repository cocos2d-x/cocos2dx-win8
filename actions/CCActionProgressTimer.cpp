/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (C) 2010      Lam Pham
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
#include "CCActionProgressTimer.h"
#include "CCProgressTimer.h"

namespace cocos2d
{
	#define kProgressTimerCast CCProgressTimer*

    // implementation of CCProgressTo

	CCProgressTo* CCProgressTo::actionWithDuration(ccTime duration, float fPercent)
	{
		CCProgressTo *pProgressTo = new CCProgressTo();
		pProgressTo->initWithDuration(duration, fPercent);
		pProgressTo->autorelease();

		return pProgressTo;
	}
 
	bool CCProgressTo::initWithDuration(ccTime duration, float fPercent)
	{
		if (CCActionInterval::initWithDuration(duration))
		{
			m_fTo = fPercent;

			return true;
		}

		return false;
	}

	CCObject* CCProgressTo::copyWithZone(CCZone *pZone)
	{
		CCZone* pNewZone = NULL;
		CCProgressTo* pCopy = NULL;
		if(pZone && pZone->m_pCopyObject) 
		{
			//in case of being called at sub class
			pCopy = (CCProgressTo*)(pZone->m_pCopyObject);
		}
		else
		{
			pCopy = new CCProgressTo();
			pZone = pNewZone = new CCZone(pCopy);
		}

		CCActionInterval::copyWithZone(pZone);

		pCopy->initWithDuration(m_fDuration, m_fTo);

		CC_SAFE_DELETE(pNewZone);
		return pCopy;
	}

	void CCProgressTo::startWithTarget(CCNode *pTarget)
	{
		CCActionInterval::startWithTarget(pTarget);
		m_fFrom = ((kProgressTimerCast)(pTarget))->getPercentage();

		// XXX: Is this correct ?
		// Adding it to support CCRepeat
		if (m_fFrom == 100)
		{
			m_fFrom = 0;
		}
	}

	void CCProgressTo::update(ccTime time)
	{
		((kProgressTimerCast)(m_pTarget))->setPercentage(m_fFrom + (m_fTo - m_fFrom) * time);
	}

	// implementation of CCProgressFromTo
 
	CCProgressFromTo* CCProgressFromTo::actionWithDuration(ccTime duration, float fFromPercentage, float fToPercentage)
	{
		CCProgressFromTo *pProgressFromTo = new CCProgressFromTo();
		pProgressFromTo->initWithDuration(duration, fFromPercentage, fToPercentage);
		pProgressFromTo->autorelease();

		return pProgressFromTo;
	}

	bool CCProgressFromTo::initWithDuration(ccTime duration, float fFromPercentage, float fToPercentage)
	{
		if (CCActionInterval::initWithDuration(duration))
		{
			m_fTo = fToPercentage;
			m_fFrom = fFromPercentage;

			return true;
		}

		return false;
	}

	CCObject* CCProgressFromTo::copyWithZone(CCZone *pZone)
	{
		CCZone* pNewZone = NULL;
		CCProgressFromTo* pCopy = NULL;
		if(pZone && pZone->m_pCopyObject) 
		{
			//in case of being called at sub class
			pCopy = (CCProgressFromTo*)(pZone->m_pCopyObject);
		}
		else
		{
			pCopy = new CCProgressFromTo();
			pZone = pNewZone = new CCZone(pCopy);
		}

		CCActionInterval::copyWithZone(pZone);

		pCopy->initWithDuration(m_fDuration, m_fFrom, m_fTo);

		CC_SAFE_DELETE(pNewZone);
		return pCopy;
	}

	CCActionInterval* CCProgressFromTo::reverse(void)
	{
		return CCProgressFromTo::actionWithDuration(m_fDuration, m_fTo, m_fFrom);
	}

	void CCProgressFromTo::startWithTarget(CCNode *pTarget)
	{
		CCActionInterval::startWithTarget(pTarget);
	}

	void CCProgressFromTo::update(ccTime time)
	{
		((kProgressTimerCast)(m_pTarget))->setPercentage(m_fFrom + (m_fTo - m_fFrom) * time);
	}

}// end of namespace cocos2d
