/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2011 Zynga Inc.
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

#include "CCActionInterval.h"
#include "CCSprite.h"
#include "CCNode.h"
#include "CCPointExtension.h"
#include "CCStdC.h"

#include <stdarg.h>

NS_CC_BEGIN

// Extra action for making a CCSequence or CCSpawn when only adding one action to it.
class ExtraAction : public CCFiniteTimeAction
{
public:
    static ExtraAction* create();
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual ExtraAction* reverse(void);
    virtual void update(float time);
    virtual void step(float dt);
};

ExtraAction* ExtraAction::create()
{
    ExtraAction* pRet = new ExtraAction();
    if (pRet)
    {
        pRet->autorelease();
    }
    return pRet;
}

CCObject* ExtraAction::copyWithZone(CCZone* pZone)
{
    CC_UNUSED_PARAM(pZone);
    ExtraAction* pRet = new ExtraAction();
    return pRet;
}

ExtraAction* ExtraAction::reverse(void)
{
    return ExtraAction::create();
}

void ExtraAction::update(float time)
{
    CC_UNUSED_PARAM(time);
}

void ExtraAction::step(float dt)
{
    CC_UNUSED_PARAM(dt);
}


//
// IntervalAction
//
CCActionInterval* CCActionInterval::create(float d)
{
	CCActionInterval *pAction = new CCActionInterval();
	pAction->initWithDuration(d);
	pAction->autorelease();

	return pAction;
}

bool CCActionInterval::initWithDuration(float d)
{
	m_fDuration = d;

	// prevent division by 0
	// This comparison could be in step:, but it might decrease the performance
	// by 3% in heavy based action games.
	if (m_fDuration == 0)
	{
		m_fDuration = FLT_EPSILON;
	}

	m_elapsed = 0;
	m_bFirstTick = true;

	return true;
}

CCObject* CCActionInterval::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCActionInterval* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCActionInterval*)(pZone->m_pCopyObject);
	}
	else
	{
		// action's base class , must be called using __super::copyWithZone(), after overriding from derived class
		CCAssert(0, "");  

		pCopy = new CCActionInterval();
		pZone = pNewZone = new CCZone(pCopy);
	}

	
	CCFiniteTimeAction::copyWithZone(pZone);

	CC_SAFE_DELETE(pNewZone);

	pCopy->initWithDuration(m_fDuration);

	return pCopy;
}

bool CCActionInterval::isDone(void)
{
	return m_elapsed >= m_fDuration;
}

void CCActionInterval::step(float dt)
{
	if (m_bFirstTick)
	{
		m_bFirstTick = false;
		m_elapsed = 0;
	}
	else
	{
		m_elapsed += dt;
	}

//	update(min(1, m_elapsed/m_fDuration));
	update(1 > m_elapsed/m_fDuration ? m_elapsed/m_fDuration : 1);
}

void CCActionInterval::setAmplitudeRate(CGFloat amp)
{
    CC_UNUSED_PARAM(amp);
	// Abstract class needs implementation
	CCAssert(0, "");
}

CGFloat CCActionInterval::getAmplitudeRate(void)
{
	// Abstract class needs implementation
	CCAssert(0, "");

	return 0;
}

void CCActionInterval::startWithTarget(CCNode *pTarget)
{
	CCFiniteTimeAction::startWithTarget(pTarget);
	m_elapsed = 0.0f;
	m_bFirstTick = true;
}

CCActionInterval* CCActionInterval::reverse(void)
{
	/*
	 NSException* myException = [NSException
								exceptionWithName:@"ReverseActionNotImplemented"
								reason:@"Reverse Action not implemented"
								userInfo:nil];
	@throw myException;	
	*/
	return NULL;
}

//
// Sequence
//
CCSequence* CCSequence::createWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo)
{
	CCSequence *pSequence = new CCSequence();
	pSequence->initWithTwoActions(pActionOne, pActionTwo);
	pSequence->autorelease();

	return pSequence;
}

//CCFiniteTimeAction* CCSequence::actions(CCFiniteTimeAction *pAction1, ...)
//{
//	va_list params;
//	va_start(params, pAction1);
//
//	CCFiniteTimeAction *pNow;
//	CCFiniteTimeAction *pPrev = pAction1;
//
//	while (pAction1)
//	{
//		pNow = va_arg(params, CCFiniteTimeAction*);
//		if (pNow)
//		{
//			pPrev = createWithTwoActions(pPrev, pNow);
//		}
//		else
//		{
//			break;
//		}
//	}
//
//	va_end(params);
//	return pPrev;
//}
//
//CCFiniteTimeAction* CCSequence::actionsWithArray(CCArray *actions)
//{
//	CCFiniteTimeAction* prev = (CCFiniteTimeAction*)actions->objectAtIndex(0);
//
//	for (unsigned int i = 1; i < actions->count(); ++i)
//	{
//		prev = createWithTwoActions(prev, (CCFiniteTimeAction*)actions->objectAtIndex(i));
//	}
//
//	return prev;
//}

CCSequence* CCSequence::create(CCFiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    CCSequence *pRet = CCSequence::createWithVariableList(pAction1, params);

    va_end(params);
    
    return pRet;
}

CCSequence* CCSequence::createWithVariableList(CCFiniteTimeAction *pAction1, va_list args)
{
    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;
    bool bOneAction = true;

    while (pAction1)
    {
        pNow = va_arg(args, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to CCSequence, make up a CCSequence by adding a simplest finite time action.
            if (bOneAction)
            {
                pPrev = createWithTwoActions(pPrev, ExtraAction::create());
            }
            break;
        }
    }
    
    return ((CCSequence*)pPrev);
}

CCSequence* CCSequence::create(CCArray* arrayOfActions)
{
    CCSequence* pRet = NULL;
    do 
    {
        unsigned  int count = arrayOfActions->count();
        CC_BREAK_IF(count == 0);

        CCFiniteTimeAction* prev = (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(0);

        if (count > 1)
        {
            for (unsigned int i = 1; i < count; ++i)
            {
                prev = createWithTwoActions(prev, (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(i));
            }
        }
        else
        {
            // If only one action is added to CCSequence, make up a CCSequence by adding a simplest finite time action.
            prev = createWithTwoActions(prev, ExtraAction::create());
        }
        pRet = (CCSequence*)prev;
    }while (0);
    return pRet;
}

CCSequence* CCSequence::actionsWithArrayLua(CCArray *actions)
{
    if (actions->count() >= 2)
    {
        CCFiniteTimeAction* prev = (CCFiniteTimeAction*)actions->objectAtIndex(0);
        for (unsigned int i = 1; i < actions->count(); ++i)
        {
            prev = createWithTwoActions(prev, (CCFiniteTimeAction*)actions->objectAtIndex(i));
        }
        return (CCSequence*)prev;
    }
    return NULL;
}

bool CCSequence::initWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo)
{
	CCAssert(pActionOne != NULL, "");
	CCAssert(pActionTwo != NULL, "");

	float d = pActionOne->getDuration() + pActionTwo->getDuration();
	CCActionInterval::initWithDuration(d);

	m_pActions[0] = pActionOne;
	pActionOne->retain();

	m_pActions[1] = pActionTwo;
	pActionTwo->retain();

	return true;
}

CCObject* CCSequence::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCSequence* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCSequence*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCSequence();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithTwoActions((CCFiniteTimeAction*)(m_pActions[0]->copy()->autorelease()), 
				(CCFiniteTimeAction*)(m_pActions[1]->copy()->autorelease()));

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

CCSequence::~CCSequence(void)
{
	CC_SAFE_RELEASE(m_pActions[0]);
	CC_SAFE_RELEASE(m_pActions[1]);
}

void CCSequence::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_split = m_pActions[0]->getDuration() / m_fDuration;
	m_last = -1;
}

void CCSequence::stop(void)
{
	m_pActions[0]->stop();
	m_pActions[1]->stop();
	CCActionInterval::stop();
}

void CCSequence::update(float time)
{
	int found = 0;
	float new_t = 0.0f;

	if (time >= m_split)
	{
		found = 1;
		if (m_split == 1)
		{
			new_t = 1;
		}
		else
		{
			new_t = (time - m_split) / (1 - m_split);
		}
	}
	else
	{
		found = 0;
		if (m_split != 0)
		{
			new_t = time / m_split;
		}
		else
		{
			new_t = 1;
		}
	}

	if (m_last == -1 && found == 1)
	{
		m_pActions[0]->startWithTarget(m_pTarget);
		m_pActions[0]->update(1.0f);
		m_pActions[0]->stop();
	}

	if (m_last != found)
	{
		if (m_last != -1)
		{
			m_pActions[m_last]->update(1.0f);
			m_pActions[m_last]->stop();
		}

		m_pActions[found]->startWithTarget(m_pTarget);
	}

	m_pActions[found]->update(new_t);
	m_last = found;
}

CCActionInterval* CCSequence::reverse(void)
{
	return CCSequence::createWithTwoActions(m_pActions[1]->reverse(), m_pActions[0]->reverse());
}

//
// Repeat
//
CCRepeat* CCRepeat::create(CCFiniteTimeAction *pAction, unsigned int times)
{
	CCRepeat* pRepeat = new CCRepeat();
	pRepeat->initWithAction(pAction, times);
	pRepeat->autorelease();

	return pRepeat;
}

bool CCRepeat::initWithAction(CCFiniteTimeAction *pAction, unsigned int times)
{
	float d = pAction->getDuration() * times;

	if (CCActionInterval::initWithDuration(d))
	{
        m_uTimes = times;
		m_pInnerAction = pAction;
		pAction->retain();

		m_uTotal = 0;

		return true;
	}

	return false;
}

CCObject* CCRepeat::copyWithZone(CCZone *pZone)
{
	
	CCZone* pNewZone = NULL;
	CCRepeat* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCRepeat*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCRepeat();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithAction((CCFiniteTimeAction*)(m_pInnerAction->copy()->autorelease()), m_uTimes);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

CCRepeat::~CCRepeat(void)
{
	CC_SAFE_RELEASE(m_pInnerAction);
}

void CCRepeat::startWithTarget(CCNode *pTarget)
{
	m_uTotal = 0;
	CCActionInterval::startWithTarget(pTarget);
	m_pInnerAction->startWithTarget(pTarget);
}

void CCRepeat::stop(void)
{
	m_pInnerAction->stop();
	CCActionInterval::stop();
}

// issue #80. Instead of hooking step:, hook update: since it can be called by any 
// container action like Repeat, Sequence, AccelDeccel, etc..
void CCRepeat::update(float time)
{
	float t = time * m_uTimes;
	if (t > m_uTotal + 1)
	{
		m_pInnerAction->update(1.0f);
		m_uTotal++;
		m_pInnerAction->stop();
		m_pInnerAction->startWithTarget(m_pTarget);

		// repeat is over?
		if (m_uTotal == m_uTimes)
		{
			// so, set it in the original position
			m_pInnerAction->update(0);
		}
		else
		{
			// no ? start next repeat with the right update
			// to prevent jerk (issue #390)
			m_pInnerAction->update(t - m_uTotal);
		}
	}
	else
	{
		float r = fmodf(t, 1.0f);

		// fix last repeat position
		// else it could be 0.
		if (time == 1.0f)
		{
			r = 1.0f;
			m_uTotal++; // this is the added line
		}

//		m_pOther->update(min(r, 1));
		m_pInnerAction->update(r > 1 ? 1 : r);
	}
}

bool CCRepeat::isDone(void)
{
	return m_uTotal == m_uTimes;
}

CCActionInterval* CCRepeat::reverse(void)
{
	return CCRepeat::create(m_pInnerAction->reverse(), m_uTimes);
}

//
// RepeatForever
//
CCRepeatForever::~CCRepeatForever()
{
	CC_SAFE_RELEASE(m_pInnerAction);
}
CCRepeatForever *CCRepeatForever::create(CCActionInterval *pAction)
{
	CCRepeatForever *pRet = new CCRepeatForever();
	if (pRet && pRet->initWithAction(pAction))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool CCRepeatForever::initWithAction(CCActionInterval *pAction)
{
	CCAssert(pAction != NULL, "");
	pAction->retain();
	m_pInnerAction = pAction;
	return true;
}
CCObject* CCRepeatForever::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCRepeatForever* pRet = NULL;
	if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
	{
		pRet = (CCRepeatForever*)(pZone->m_pCopyObject);
	}
	else
	{
		pRet = new CCRepeatForever();
		pZone = pNewZone = new CCZone(pRet);
	}
	CCActionInterval::copyWithZone(pZone);
	// win32 : use the m_pOther's copy object.
	pRet->initWithAction((CCActionInterval*)(m_pInnerAction->copy()->autorelease())); 
	CC_SAFE_DELETE(pNewZone);
	return pRet;
}

void CCRepeatForever::startWithTarget(CCNode* pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_pInnerAction->startWithTarget(pTarget);
}

void CCRepeatForever::step(float dt)
{
	m_pInnerAction->step(dt);
	if (m_pInnerAction->isDone())
	{
		float diff = m_pInnerAction->getElapsed() - m_pInnerAction->getDuration();
		m_pInnerAction->startWithTarget(m_pTarget);
		// to prevent jerk. issue #390, 1247
		m_pInnerAction->step(0.0f);
		m_pInnerAction->step(diff);
	}
}

bool CCRepeatForever::isDone()
{
	return false;
}

CCActionInterval *CCRepeatForever::reverse()
{
	return (CCActionInterval*)(CCRepeatForever::create(m_pInnerAction->reverse()));
}

//
// Spawn
//
CCFiniteTimeAction* CCSpawn::create(CCFiniteTimeAction *pAction1, ...)
{
	va_list params;
	va_start(params, pAction1);

	CCFiniteTimeAction *pNow;
	CCFiniteTimeAction *pPrev = pAction1;

	while (pAction1)
	{
		pNow = va_arg(params, CCFiniteTimeAction*);
		if (pNow)
		{
			pPrev = createWithTwoActions(pPrev, pNow);
		}
		else
		{
			break;
		}
	}

	va_end(params);
	return pPrev;
}

CCFiniteTimeAction* CCSpawn::create(CCArray *actions)
{
	CCFiniteTimeAction* prev = (CCFiniteTimeAction*)actions->objectAtIndex(0);

	for (unsigned int i = 1; i < actions->count(); ++i)
	{
		prev = createWithTwoActions(prev, (CCFiniteTimeAction*)actions->objectAtIndex(i));
	}

	return prev;
}

CCSpawn* CCSpawn::createWithVariableList(CCFiniteTimeAction *pAction1, va_list args)
{
    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;
    bool bOneAction = true;

    while (pAction1)
    {
        pNow = va_arg(args, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to CCSpawn, make up a CCSpawn by adding a simplest finite time action.
            if (bOneAction)
            {
                pPrev = createWithTwoActions(pPrev, ExtraAction::create());
            }
            break;
        }
    }

    return ((CCSpawn*)pPrev);
}

CCSpawn* CCSpawn::actionsWithArrayLua(CCArray *actions)
{
    if (actions->count() >= 2)
    {
        CCFiniteTimeAction* prev = (CCFiniteTimeAction*)actions->objectAtIndex(0);
        
        for (unsigned int i = 1; i < actions->count(); ++i)
        {
            prev = createWithTwoActions(prev, (CCFiniteTimeAction*)actions->objectAtIndex(i));
        }
        
        return (CCSpawn*)prev;
    }
    return NULL;
}

CCSpawn* CCSpawn::createWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2)
{
	CCSpawn *pSpawn = new CCSpawn();
	pSpawn->initWithTwoActions(pAction1, pAction2);
	pSpawn->autorelease();

	return pSpawn;
}

bool CCSpawn:: initWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2)
{
	CCAssert(pAction1 != NULL, "");
	CCAssert(pAction2 != NULL, "");

	bool bRet = false;

	float d1 = pAction1->getDuration();
	float d2 = pAction2->getDuration();

	if (CCActionInterval::initWithDuration(MAX(d1, d2)))
	{
		m_pOne = pAction1;
		m_pTwo = pAction2;

		if (d1 > d2)
		{
			m_pTwo = CCSequence::createWithTwoActions(pAction2, CCDelayTime::create(d1 - d2));
		} else
		if (d1 < d2)
		{
			m_pOne = CCSequence::createWithTwoActions(pAction1, CCDelayTime::create(d2 - d1));
		}

		m_pOne->retain();
		m_pTwo->retain();

		bRet = true;
	}

    
	return bRet;
}

CCObject* CCSpawn::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCSpawn* pCopy = NULL;

	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCSpawn*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCSpawn();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithTwoActions((CCFiniteTimeAction*)(m_pOne->copy()->autorelease()), 
					(CCFiniteTimeAction*)(m_pTwo->copy()->autorelease()));

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

CCSpawn::~CCSpawn(void)
{
	CC_SAFE_RELEASE(m_pOne);
	CC_SAFE_RELEASE(m_pTwo);
}

void CCSpawn::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_pOne->startWithTarget(pTarget);
	m_pTwo->startWithTarget(pTarget);
}

void CCSpawn::stop(void)
{
	m_pOne->stop();
	m_pTwo->stop();
	CCActionInterval::stop();
}

void CCSpawn::update(float time)
{
	if (m_pOne)
	{
		m_pOne->update(time);
	}
	if (m_pTwo)
	{
		m_pTwo->update(time);
	}
}

CCActionInterval* CCSpawn::reverse(void)
{
	return CCSpawn::createWithTwoActions(m_pOne->reverse(), m_pTwo->reverse());
}

//
// RotateTo
//
CCRotateTo* CCRotateTo::create(float duration, float fDeltaAngle)
{
	CCRotateTo* pRotateTo = new CCRotateTo();
	pRotateTo->initWithDuration(duration, fDeltaAngle);
	pRotateTo->autorelease();

	return pRotateTo;
}

bool CCRotateTo::initWithDuration(float duration, float fDeltaAngle)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_fDstAngle = fDeltaAngle;
		return true;
	}

	return false;
}
CCRotateTo* CCRotateTo::create(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    CCRotateTo* pRotateTo = new CCRotateTo();
    pRotateTo->initWithDuration(fDuration, fDeltaAngleX, fDeltaAngleY);
    pRotateTo->autorelease();
    
    return pRotateTo;
}

bool CCRotateTo::initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (CCActionInterval::initWithDuration(fDuration))
    {
        m_fDstAngleX = fDeltaAngleX;
        m_fDstAngleY = fDeltaAngleY;
        
        return true;
    }
    
    return false;
}
CCObject* CCRotateTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCRotateTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject)
	{
		//in case of being called at sub class
		pCopy = (CCRotateTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCRotateTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_fDstAngle);

	//Action *copy = [[[self class] allocWithZone: zone] initWithDuration:[self duration] angle: angle];
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCRotateTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);

	m_fStartAngle = pTarget->getRotation();

	if (m_fStartAngle > 0)
	{
		m_fStartAngle = fmodf(m_fStartAngle, 360.0f);
	}
	else
	{
		m_fStartAngle = fmodf(m_fStartAngle, -360.0f);
	}

	m_fDiffAngle = m_fDstAngle - m_fStartAngle;
	if (m_fDiffAngle > 180)
	{
		m_fDiffAngle -= 360;
	}

	if (m_fDiffAngle < -180)
	{
		m_fDiffAngle += 360;
	}
}

void CCRotateTo::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setRotation(m_fStartAngle + m_fDiffAngle * time);
	}
}

//
// RotateBy
//
CCRotateBy* CCRotateBy::create(float duration, float fDeltaAngle)
{
	CCRotateBy *pRotateBy = new CCRotateBy();
	pRotateBy->initWithDuration(duration, fDeltaAngle);
	pRotateBy->autorelease();

	return pRotateBy;
}

bool CCRotateBy::initWithDuration(float duration, float fDeltaAngle)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_fAngle = fDeltaAngle;
		return true;
	}

	return false;
}
CCRotateBy* CCRotateBy::create(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    CCRotateBy *pRotateBy = new CCRotateBy();
    pRotateBy->initWithDuration(fDuration, fDeltaAngleX, fDeltaAngleY);
    pRotateBy->autorelease();
    
    return pRotateBy;
}
bool CCRotateBy::initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (CCActionInterval::initWithDuration(fDuration))
    {
        m_fAngleX = fDeltaAngleX;
        m_fAngleY = fDeltaAngleY;
        return true;
    }
    
    return false;
}
CCObject* CCRotateBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCRotateBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCRotateBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCRotateBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_fAngle);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCRotateBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_fStartAngle = pTarget->getRotation();
}

void CCRotateBy::update(float time)
{
	// XXX: shall I add % 360
	if (m_pTarget)
	{
		m_pTarget->setRotation(m_fStartAngle + m_fAngle * time);
	}
}

CCActionInterval* CCRotateBy::reverse(void)
{
	return CCRotateBy::create(m_fDuration, -m_fAngle);
}

//
// MoveTo
//
CCMoveTo* CCMoveTo::create(float duration, const CCPoint& position)
{
	CCMoveTo *pMoveTo = new CCMoveTo();
	pMoveTo->initWithDuration(duration, position);
	pMoveTo->autorelease();

	return pMoveTo;
}

bool CCMoveTo::initWithDuration(float duration, const CCPoint& position)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_endPosition = position;
		return true;
	}

	return false;
}

CCObject* CCMoveTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCMoveTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCMoveTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCMoveTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_endPosition);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCMoveTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_startPosition = pTarget->getPosition();
	m_delta = ccpSub(m_endPosition, m_startPosition);
}

void CCMoveTo::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
			m_startPosition.y + m_delta.y * time));
	}
}

//
// MoveBy
//
CCMoveBy* CCMoveBy::create(float duration, const CCPoint& position)
{
	CCMoveBy *pMoveBy = new CCMoveBy();
	pMoveBy->initWithDuration(duration, position);
	pMoveBy->autorelease();

	return pMoveBy;
}

bool CCMoveBy::initWithDuration(float duration, const CCPoint& position)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_delta = position;
		return true;
	}

	return false;
}

CCObject* CCMoveBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCMoveBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCMoveBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCMoveBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCMoveTo::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_delta);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCMoveBy::startWithTarget(CCNode *pTarget)
{
	CCPoint dTmp = m_delta;
	CCMoveTo::startWithTarget(pTarget);
	m_delta = dTmp;
}

CCActionInterval* CCMoveBy::reverse(void)
{
	return CCMoveBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y));
}

//
// CCSkewTo
//
CCSkewTo* CCSkewTo::create(float t, float sx, float sy)
{
	CCSkewTo *pSkewTo = new CCSkewTo();
	if (pSkewTo)
	{
		if (pSkewTo->initWithDuration(t, sx, sy))
		{
			pSkewTo->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(pSkewTo);
		}
	}

	return pSkewTo;
}

bool CCSkewTo::initWithDuration(float t, float sx, float sy)
{
	bool bRet = false;

	if (CCActionInterval::initWithDuration(t))
	{
		m_fEndSkewX = sx;
		m_fEndSkewY = sy;

		bRet = true;
	}

	return bRet;
}

CCObject* CCSkewTo::copyWithZone(CCZone* pZone)
{
	CCZone* pNewZone = NULL;
	CCSkewTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCSkewTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCSkewTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_fEndSkewX, m_fEndSkewY);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCSkewTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);

	m_fStartSkewX = pTarget->getSkewX();

	if (m_fStartSkewX > 0)
	{
		m_fStartSkewX = fmodf(m_fStartSkewX, 180.f);
	}
	else
	{
		m_fStartSkewX = fmodf(m_fStartSkewX, -180.f);
	}

	m_fDeltaX = m_fEndSkewX - m_fStartSkewX;

	if (m_fDeltaX > 180)
	{
		m_fDeltaX -= 360;
	}
	if (m_fDeltaX < -180)
	{
		m_fDeltaX += 360;
	}

	m_fStartSkewY = pTarget->getSkewY();

	if (m_fStartSkewY > 0)
	{
		m_fStartSkewY = fmodf(m_fStartSkewY, 360.f);
	}
	else
	{
		m_fStartSkewY = fmodf(m_fStartSkewY, -360.f);
	}

	m_fDeltaY = m_fEndSkewY - m_fStartSkewY;

	if (m_fDeltaY > 180)
	{
		m_fDeltaY -= 360;
	}
	if (m_fDeltaY < -180)
	{
		m_fDeltaY += 360;
	}
}

void CCSkewTo::update(float t)
{
	m_pTarget->setSkewX(m_fStartSkewX + m_fDeltaX * t);
	m_pTarget->setSkewY(m_fStartSkewY + m_fDeltaY * t);
}

CCSkewTo::CCSkewTo()
: m_fSkewX(0.0)
, m_fSkewY(0.0)
, m_fStartSkewX(0.0)
, m_fStartSkewY(0.0)
, m_fEndSkewX(0.0)
, m_fEndSkewY(0.0)
, m_fDeltaX(0.0)
, m_fDeltaY(0.0)
{
}

//
// CCSkewBy
//
CCSkewBy* CCSkewBy::create(float t, float sx, float sy)
{
	CCSkewBy *pSkewBy = new CCSkewBy();
	if (pSkewBy)
	{
		if (pSkewBy->initWithDuration(t, sx, sy))
		{
			pSkewBy->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(pSkewBy);
		}
	}

	return pSkewBy;
}

bool CCSkewBy::initWithDuration(float t, float deltaSkewX, float deltaSkewY)
{
	bool bRet = false;

	if (CCSkewTo::initWithDuration(t, deltaSkewX, deltaSkewY))
	{
		m_fSkewX = deltaSkewX;
		m_fSkewY = deltaSkewY;

		bRet = true;
	}

	return bRet;
}

void CCSkewBy::startWithTarget(CCNode *pTarget)
{
	CCSkewTo::startWithTarget(pTarget);
	m_fDeltaX = m_fSkewX;
	m_fDeltaY = m_fSkewY;
	m_fEndSkewX = m_fStartSkewX + m_fDeltaX;
	m_fEndSkewY = m_fStartSkewY + m_fDeltaY;
}

CCActionInterval* CCSkewBy::reverse()
{
	return create(m_fDuration, -m_fSkewX, -m_fSkewY);
}

//
// JumpBy
//
CCJumpBy* CCJumpBy::create(float duration, const CCPoint& position, float height, unsigned int jumps)
{
	CCJumpBy *pJumpBy = new CCJumpBy();
	pJumpBy->initWithDuration(duration, position, height, jumps);
	pJumpBy->autorelease();

	return pJumpBy;
}

bool CCJumpBy::initWithDuration(float duration, const CCPoint& position, float height, unsigned int jumps)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_delta = position;
		m_height = height;
		m_nJumps = jumps;

		return true;
	}

	return false;
}

CCObject* CCJumpBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCJumpBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCJumpBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCJumpBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCJumpBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_startPosition = pTarget->getPosition();
}

void CCJumpBy::update(float time)
{
	// parabolic jump (since v0.8.2)
	if (m_pTarget)
	{
		float frac = fmodf(time * m_nJumps, 1.0f);
		float y = m_height * 4 * frac * (1 - frac);
		y += m_delta.y * time;
		float x = m_delta.x * time;
		m_pTarget->setPosition(ccp(m_startPosition.x + x, m_startPosition.y + y));
	}
}

CCActionInterval* CCJumpBy::reverse(void)
{
	return CCJumpBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y),
		m_height, m_nJumps);
}

//
// JumpTo
//
CCJumpTo* CCJumpTo::create(float duration, const CCPoint& position, float height, int jumps)
{
	CCJumpTo *pJumpTo = new CCJumpTo();
	pJumpTo->initWithDuration(duration, position, height, jumps);
	pJumpTo->autorelease();

	return pJumpTo;
}

CCObject* CCJumpTo::copyWithZone(CCZone* pZone)
{
	CCZone* pNewZone = NULL;
	CCJumpTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject)
	{
		//in case of being called at sub class
		pCopy = (CCJumpTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCJumpTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCJumpBy::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCJumpTo::startWithTarget(CCNode *pTarget)
{
	CCJumpBy::startWithTarget(pTarget);
	m_delta = ccp(m_delta.x - m_startPosition.x, m_delta.y - m_startPosition.y);
}

// Bezier cubic formula:
//	((1 - t) + t)3 = 1 
// Expands to¡­ 
//   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1 
static inline float bezierat( float a, float b, float c, float d, float t )
{
	return (powf(1-t,3) * a + 
			3*t*(powf(1-t,2))*b + 
			3*powf(t,2)*(1-t)*c +
			powf(t,3)*d );
}

//
// BezierBy
//
CCBezierBy* CCBezierBy::create(float t, const ccBezierConfig& c)
{
	CCBezierBy *pBezierBy = new CCBezierBy();
	pBezierBy->initWithDuration(t, c);
	pBezierBy->autorelease();

	return pBezierBy;
}

bool CCBezierBy::initWithDuration(float t, const ccBezierConfig& c)
{
	if (CCActionInterval::initWithDuration(t))
	{
        m_sConfig = c;
		return true;
	}

	return false;
}

void CCBezierBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_startPosition = pTarget->getPosition();
}

CCObject* CCBezierBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCBezierBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCBezierBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCBezierBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_sConfig);
    
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCBezierBy::update(float time)
{
	if (m_pTarget)
	{
		float xa = 0;
		float xb = m_sConfig.controlPoint_1.x;
		float xc = m_sConfig.controlPoint_2.x;
		float xd = m_sConfig.endPosition.x;

		float ya = 0;
		float yb = m_sConfig.controlPoint_1.y;
		float yc = m_sConfig.controlPoint_2.y;
		float yd = m_sConfig.endPosition.y;

		float x = bezierat(xa, xb, xc, xd, time);
		float y = bezierat(ya, yb, yc, yd, time);
		m_pTarget->setPosition(ccpAdd(m_startPosition, ccp(x, y)));
	}
}

CCActionInterval* CCBezierBy::reverse(void)
{
	ccBezierConfig r;

	r.endPosition = ccpNeg(m_sConfig.endPosition);
	r.controlPoint_1 = ccpAdd(m_sConfig.controlPoint_2, ccpNeg(m_sConfig.endPosition));
	r.controlPoint_2 = ccpAdd(m_sConfig.controlPoint_1, ccpNeg(m_sConfig.endPosition));

	CCBezierBy *pAction = CCBezierBy::create(m_fDuration, r);
	return pAction;
}

//
// BezierTo
//
CCBezierTo* CCBezierTo::create(float t, const ccBezierConfig& c)
{
	CCBezierTo *pBezierTo = new CCBezierTo();
	pBezierTo->initWithDuration(t, c);
	pBezierTo->autorelease();

	return pBezierTo;
}

CCObject* CCBezierTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCBezierBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCBezierTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCBezierTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCBezierBy::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_sConfig);
    
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCBezierTo::startWithTarget(CCNode *pTarget)
{
	CCBezierBy::startWithTarget(pTarget);
	m_sConfig.controlPoint_1 = ccpSub(m_sConfig.controlPoint_1, m_startPosition);
	m_sConfig.controlPoint_2 = ccpSub(m_sConfig.controlPoint_2, m_startPosition);
	m_sConfig.endPosition = ccpSub(m_sConfig.endPosition, m_startPosition);
}

//
// ScaleTo
//
CCScaleTo* CCScaleTo::create(float duration, float s)
{
	CCScaleTo *pScaleTo = new CCScaleTo();
	pScaleTo->initWithDuration(duration, s);
	pScaleTo->autorelease();

	return pScaleTo;
}

bool CCScaleTo::initWithDuration(float duration, float s)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_fEndScaleX = s;
		m_fEndScaleY = s;

		return true;
	}

	return false;
}

CCScaleTo* CCScaleTo::create(float duration, float sx, float sy)
{
	CCScaleTo *pScaleTo = new CCScaleTo();
	pScaleTo->initWithDuration(duration, sx, sy);
	pScaleTo->autorelease();

	return pScaleTo;
}

bool CCScaleTo::initWithDuration(float duration, float sx, float sy)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		m_fEndScaleX = sx;
		m_fEndScaleY = sy;

		return true;
	}

	return false;
}

CCObject* CCScaleTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCScaleTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCScaleTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCScaleTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);


	pCopy->initWithDuration(m_fDuration, m_fEndScaleX, m_fEndScaleY);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCScaleTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_fStartScaleX = pTarget->getScaleX();
	m_fStartScaleY = pTarget->getScaleY();
    m_fDeltaX = m_fEndScaleX - m_fStartScaleX;
	m_fDeltaY = m_fEndScaleY - m_fStartScaleY;
}

void CCScaleTo::update(float time)
{
	if (m_pTarget)
	{
		m_pTarget->setScaleX(m_fStartScaleX + m_fDeltaX * time);
		m_pTarget->setScaleY(m_fStartScaleY + m_fDeltaY * time);
	}
}

//
// ScaleBy
//
CCScaleBy* CCScaleBy::create(float duration, float s)
{
	CCScaleBy *pScaleBy = new CCScaleBy();
	pScaleBy->initWithDuration(duration, s);
	pScaleBy->autorelease();

	return pScaleBy;
}

CCScaleBy* CCScaleBy::create(float duration, float sx, float sy)
{
	CCScaleBy *pScaleBy = new CCScaleBy();
	pScaleBy->initWithDuration(duration, sx, sy);
	pScaleBy->autorelease();

	return pScaleBy;
}

CCObject* CCScaleBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCScaleTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject)
	{
		//in case of being called at sub class
		pCopy = (CCScaleBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCScaleBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCScaleTo::copyWithZone(pZone);


	pCopy->initWithDuration(m_fDuration, m_fEndScaleX, m_fEndScaleY);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCScaleBy::startWithTarget(CCNode *pTarget)
{
	CCScaleTo::startWithTarget(pTarget);
	m_fDeltaX = m_fStartScaleX * m_fEndScaleX - m_fStartScaleX;
	m_fDeltaY = m_fStartScaleY * m_fEndScaleY - m_fStartScaleY;
}

CCActionInterval* CCScaleBy::reverse(void)
{
	return CCScaleBy::create(m_fDuration, 1 / m_fEndScaleX, 1 / m_fEndScaleY);
}

//
// Blink
//
CCBlink* CCBlink::create(float duration, unsigned int uBlinks)
{
	CCBlink *pBlink = new CCBlink();
	pBlink->initWithDuration(duration, uBlinks);
	pBlink->autorelease();

	return pBlink;
}

bool CCBlink::initWithDuration(float duration, unsigned int uBlinks)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_nTimes = uBlinks;
		return true;
	}

	return false;
}

CCObject* CCBlink::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCBlink* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCBlink*)(pZone->m_pCopyObject);

	}
	else
	{
		pCopy = new CCBlink();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, (unsigned int)m_nTimes);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCBlink::update(float time)
{
	if (m_pTarget && ! isDone())
	{
		float slice = 1.0f / m_nTimes;
		float m = fmodf(time, slice);
		m_pTarget->setVisible(m > slice / 2 ? true : false);
	}
}

CCActionInterval* CCBlink::reverse(void)
{
	// return 'self'
	return CCBlink::create(m_fDuration, m_nTimes);
}

//
// FadeIn
//
CCFadeIn* CCFadeIn::create(float d)
{
	CCFadeIn* pAction = new CCFadeIn();

	pAction->initWithDuration(d);
 	pAction->autorelease();

	return pAction;
}

CCObject* CCFadeIn::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCFadeIn* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject)
	{
		//in case of being called at sub class
		pCopy = (CCFadeIn*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCFadeIn();
		pZone = pNewZone = new CCZone(pCopy);
	}
	
	CCActionInterval::copyWithZone(pZone);

	CC_SAFE_DELETE(pNewZone);

	return pCopy;
}

void CCFadeIn::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
        pRGBAProtocol->setOpacity((CCubyte)(255 * time));
	}
	/*m_pTarget->setOpacity((CCubyte)(255 * time));*/
}

CCActionInterval* CCFadeIn::reverse(void)
{
	return CCFadeOut::create(m_fDuration);
}

//
// FadeOut
//
CCFadeOut* CCFadeOut::create(float d)
{
	CCFadeOut* pAction = new CCFadeOut();

	pAction->initWithDuration(d);
 	pAction->autorelease();

	return pAction;
}

CCObject* CCFadeOut::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCFadeOut* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCFadeOut*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCFadeOut();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	CC_SAFE_DELETE(pNewZone);

	return pCopy;
}

void CCFadeOut::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		pRGBAProtocol->setOpacity(CCubyte(255 * (1 - time)));
	}
	/*m_pTarget->setOpacity(CCubyte(255 * (1 - time)));*/	
}

CCActionInterval* CCFadeOut::reverse(void)
{
	return CCFadeIn::create(m_fDuration);
}

//
// FadeTo
//
CCFadeTo* CCFadeTo::create(float duration, CCubyte opacity)
{
	CCFadeTo *pFadeTo = new CCFadeTo();
	pFadeTo->initWithDuration(duration, opacity);
	pFadeTo->autorelease();

	 return pFadeTo;
}

bool CCFadeTo::initWithDuration(float duration, CCubyte opacity)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_toOpacity = opacity;
		return true;
	}

	return false;
}

CCObject* CCFadeTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCFadeTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCFadeTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCFadeTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_toOpacity);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCFadeTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);

	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(pTarget);
	if (pRGBAProtocol)
	{
		m_fromOpacity = pRGBAProtocol->getOpacity();
	}
	/*m_fromOpacity = pTarget->getOpacity();*/
}

void CCFadeTo::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		pRGBAProtocol->setOpacity((CCubyte)(m_fromOpacity + (m_toOpacity - m_fromOpacity) * time));
	}
	/*m_pTarget->setOpacity((CCubyte)(m_fromOpacity + (m_toOpacity - m_fromOpacity) * time));*/
}

//
// TintTo
//
CCTintTo* CCTintTo::create(float duration, CCubyte red, CCubyte green, CCubyte blue)
{
	CCTintTo *pTintTo = new CCTintTo();
	pTintTo->initWithDuration(duration, red, green, blue);
	pTintTo->autorelease();

	return pTintTo;
}

bool CCTintTo::initWithDuration(float duration, CCubyte red, CCubyte green, CCubyte blue)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_to = ccc3(red, green, blue);
		return true;
	}

	return false;
}

CCObject* CCTintTo::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCTintTo* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCTintTo*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCTintTo();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_to.r, m_to.g, m_to.b);
	
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCTintTo::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
    CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		m_from = pRGBAProtocol->getColor();
	}
	/*m_from = pTarget->getColor();*/
}

void CCTintTo::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		pRGBAProtocol->setColor(ccc3(CCubyte(m_from.r + (m_to.r - m_from.r) * time), 
			(CCbyte)(m_from.g + (m_to.g - m_from.g) * time),
			(CCbyte)(m_from.b + (m_to.b - m_from.b) * time)));
	}	
}

//
// TintBy
//
CCTintBy* CCTintBy::create(float duration, CCshort deltaRed, CCshort deltaGreen, CCshort deltaBlue)
{
	CCTintBy *pTintBy = new CCTintBy();
	pTintBy->initWithDuration(duration, deltaRed, deltaGreen, deltaBlue);
	pTintBy->autorelease();

	return pTintBy;
}

bool CCTintBy::initWithDuration(float duration, CCshort deltaRed, CCshort deltaGreen, CCshort deltaBlue)
{
	if (CCActionInterval::initWithDuration(duration))
	{
        m_deltaR = deltaRed;
		m_deltaG = deltaGreen;
		m_deltaB = deltaBlue;

		return true;
	}

	return false;
}

CCObject* CCTintBy::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCTintBy* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCTintBy*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCTintBy();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, (CCubyte)m_deltaR, (CCubyte)m_deltaG, (CCubyte)m_deltaB);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void CCTintBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);

	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(pTarget);
	if (pRGBAProtocol)
	{
		ccColor3B color = pRGBAProtocol->getColor();
		m_fromR = color.r;
		m_fromG = color.g;
		m_fromB = color.b;
	}	
}

void CCTintBy::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		pRGBAProtocol->setColor(ccc3((CCubyte)(m_fromR + m_deltaR * time),
			(CCubyte)(m_fromG + m_deltaG * time),
			(CCubyte)(m_fromB + m_deltaB * time)));
	}	
}

CCActionInterval* CCTintBy::reverse(void)
{
	return CCTintBy::create(m_fDuration, -m_deltaR, -m_deltaG, -m_deltaB);
}

//
// DelayTime
//
CCDelayTime* CCDelayTime::create(float d)
{
	CCDelayTime* pAction = new CCDelayTime();

	pAction->initWithDuration(d);
	pAction->autorelease();

	return pAction;
}

CCObject* CCDelayTime::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCDelayTime* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCDelayTime*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCDelayTime();
		pZone = pNewZone = new CCZone(pCopy);
	}

	
	CCActionInterval::copyWithZone(pZone);

	CC_SAFE_DELETE(pNewZone);

	return pCopy;
}

void CCDelayTime::update(float time)
{
    CC_UNUSED_PARAM(time);
	return;
}

CCActionInterval* CCDelayTime::reverse(void)
{
	return CCDelayTime::create(m_fDuration);
}

//
// ReverseTime
//
CCReverseTime* CCReverseTime::create(CCFiniteTimeAction *pAction)
{
	// casting to prevent warnings
	CCReverseTime *pReverseTime = new CCReverseTime();
	pReverseTime->initWithAction(pAction);
	pReverseTime->autorelease();

	return pReverseTime;
}

bool CCReverseTime::initWithAction(CCFiniteTimeAction *pAction)
{
	CCAssert(pAction != NULL, "");
	CCAssert(pAction != m_pOther, "");

	if (CCActionInterval::initWithDuration(pAction->getDuration()))
	{
		// Don't leak if action is reused
		CC_SAFE_RELEASE(m_pOther);

		m_pOther = pAction;
		pAction->retain();

		return true;
	}

	return false;
}

CCObject* CCReverseTime::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCReverseTime* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCReverseTime*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCReverseTime();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithAction((CCFiniteTimeAction*)(m_pOther->copy()->autorelease()));

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

CCReverseTime::CCReverseTime() : m_pOther(NULL) 
{

}

CCReverseTime::~CCReverseTime(void)
{
	CC_SAFE_RELEASE(m_pOther);
}

void CCReverseTime::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	m_pOther->startWithTarget(pTarget);
}

void CCReverseTime::stop(void)
{
	m_pOther->stop();
	CCActionInterval::stop();
}

void CCReverseTime::update(float time)
{
	if (m_pOther)
	{
		m_pOther->update(1 - time);
	}
}

CCActionInterval* CCReverseTime::reverse(void)
{
	return (CCActionInterval*)(m_pOther->copy()->autorelease());
}

//
// Animate
//
CCAnimate* CCAnimate::create(CCAnimation *pAnimation)
{
	CCAnimate *pAnimate = new CCAnimate();
	pAnimate->initWithAnimation(pAnimation, true);
	pAnimate->autorelease();

	return pAnimate;
}

bool CCAnimate::initWithAnimation(CCAnimation *pAnimation)
{
	CCAssert(pAnimation != NULL, "");

	return initWithAnimation(pAnimation, true);
}

CCAnimate* CCAnimate::create(CCAnimation *pAnimation, bool bRestoreOriginalFrame)
{
	CCAnimate *pAnimate = new CCAnimate();
	pAnimate->initWithAnimation(pAnimation, bRestoreOriginalFrame);
	pAnimate->autorelease();

	return pAnimate;
}

bool CCAnimate::initWithAnimation(CCAnimation *pAnimation, bool bRestoreOriginalFrame)
{
	CCAssert(pAnimation, "");

	if (CCActionInterval::initWithDuration(pAnimation->getFrames()->count() * pAnimation->getTotalDelayUnits()))
	{
		m_bRestoreOriginalFrame = bRestoreOriginalFrame;
       m_pAnimation = pAnimation;
		CC_SAFE_RETAIN(m_pAnimation);
		m_pOrigFrame = NULL;

		return true;
	}

	return false;
}

CCAnimate* CCAnimate::create(float duration, CCAnimation *pAnimation, bool bRestoreOriginalFrame)
{
	CCAnimate *pAnimate = new CCAnimate();
	pAnimate->initWithDuration(duration, pAnimation, bRestoreOriginalFrame);
	pAnimate->autorelease();

	return pAnimate;
}

bool CCAnimate::initWithDuration(float duration, CCAnimation *pAnimation, bool bRestoreOriginalFrame)
{
	CCAssert(pAnimation != NULL, "");

	if (CCActionInterval::initWithDuration(duration))
	{
		m_bRestoreOriginalFrame = bRestoreOriginalFrame;
		m_pAnimation = pAnimation;
		CC_SAFE_RETAIN(m_pAnimation);
		m_pOrigFrame = NULL;

		return true;
	}

	return false;
}

CCObject* CCAnimate::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCAnimate* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (CCAnimate*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new CCAnimate();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_pAnimation, m_bRestoreOriginalFrame);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

CCAnimate::~CCAnimate(void)
{
	CC_SAFE_RELEASE(m_pAnimation);
    CC_SAFE_RELEASE(m_pOrigFrame);
}

void CCAnimate::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	CCSprite *pSprite = (CCSprite*)(pTarget);

	CC_SAFE_RELEASE(m_pOrigFrame);

	if (m_bRestoreOriginalFrame)
	{
		m_pOrigFrame = pSprite->displayedFrame();
		m_pOrigFrame->retain();
	}
}

void CCAnimate::stop(void)
{
	if (m_bRestoreOriginalFrame && m_pTarget)
	{
		((CCSprite*)(m_pTarget))->setDisplayFrame(m_pOrigFrame);
	}

	CCActionInterval::stop();
}

void CCAnimate::update(float time)
{
	CCArray *pFrames = m_pAnimation->getFrames();
	unsigned int numberOfFrames = pFrames->count();

	unsigned int idx = (unsigned int)(time * numberOfFrames);

	if (idx >= numberOfFrames)
	{
		idx = numberOfFrames - 1;
	}

	CCSprite *pSprite = (CCSprite*)(m_pTarget);
	if (! pSprite->isFrameDisplayed((CCSpriteFrame*)pFrames->objectAtIndex(idx)))
	{
		pSprite->setDisplayFrame((CCSpriteFrame*)pFrames->objectAtIndex(idx));
	}
}

CCActionInterval* CCAnimate::reverse(void)
{
	CCArray *pOldArray = m_pAnimation->getFrames();
	CCArray *pNewArray = new CCArray(pOldArray->count());
   
	if (pOldArray->count() > 0)
	{
		CCSpriteFrame *pElement;
		//CCArray::CCMutableArrayRevIterator iter;
		//for (iter = pOldArray->rbegin(); iter != pOldArray->rend(); iter++)
		//{
		//	pElement = *iter;
		//	if (! pElement)
		//	{
		//		break;
		//	}

		//	pNewArray->addObject((CCSpriteFrame*)(pElement->copy()->autorelease()));
		//}
		CCObject* pObj = NULL;
		CCARRAY_FOREACH(pOldArray, pObj)
		{
			pElement = (CCSpriteFrame *)pObj;
			if (! pElement)
			{
				break;
			}

			pNewArray->addObject((CCSpriteFrame*)(pElement->copy()->autorelease()));
		}
	}

	CCAnimation *pNewAnim = CCAnimation::create(pNewArray, m_pAnimation->getTotalDelayUnits());

	pNewArray->release();

	return CCAnimate::create(m_fDuration, pNewAnim, m_bRestoreOriginalFrame);
}
// CCTargetedAction

CCTargetedAction::CCTargetedAction()
: m_pForcedTarget(NULL)
, m_pAction(NULL)
{

}

CCTargetedAction::~CCTargetedAction()
{
    CC_SAFE_RELEASE(m_pForcedTarget);
    CC_SAFE_RELEASE(m_pAction);
}

CCTargetedAction* CCTargetedAction::create(CCNode* pTarget, CCFiniteTimeAction* pAction)
{
    CCTargetedAction* p = new CCTargetedAction();
    p->initWithTarget(pTarget, pAction);
    p->autorelease();
    return p;
}


bool CCTargetedAction::initWithTarget(CCNode* pTarget, CCFiniteTimeAction* pAction)
{
    if(CCActionInterval::initWithDuration(pAction->getDuration()))
    {
        CC_SAFE_RETAIN(pTarget);
        m_pForcedTarget = pTarget;
        CC_SAFE_RETAIN(pAction);
        m_pAction = pAction;
        return true;
    }
    return false;
}

CCObject* CCTargetedAction::copyWithZone(CCZone* pZone)
{
    CCZone* pNewZone = NULL;
    CCTargetedAction* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (CCTargetedAction*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new CCTargetedAction();
        pZone = pNewZone = new CCZone(pRet);
    }
    CCActionInterval::copyWithZone(pZone);
    // win32 : use the m_pOther's copy object.
    pRet->initWithTarget(m_pForcedTarget, (CCFiniteTimeAction*)m_pAction->copy()->autorelease()); 
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCTargetedAction::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_pAction->startWithTarget(m_pForcedTarget);
}

void CCTargetedAction::stop(void)
{
    m_pAction->stop();
}

void CCTargetedAction::update(float time)
{
    m_pAction->update(time);
}
NS_CC_END