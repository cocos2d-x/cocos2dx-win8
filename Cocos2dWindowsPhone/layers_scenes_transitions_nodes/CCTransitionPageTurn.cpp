/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009      Sindesso Pty Ltd http://www.sindesso.com/
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

#include "CCTransitionPageTurn.h"
#include "CCDirector.h"
#include "CCActionInterval.h"
#include "CCActionInstant.h"
#include "CCActionGrid.h"
#include "CCActionPageTurn3D.h"

NS_CC_BEGIN

CCTransitionPageTurn::CCTransitionPageTurn()
{
}
CCTransitionPageTurn::~CCTransitionPageTurn()
{
}

/** creates a base transition with duration and incoming scene */
CCTransitionPageTurn * CCTransitionPageTurn::transitionWithDuration(ccTime t, CCScene *scene, bool backwards)
{
	CCTransitionPageTurn * pTransition = new CCTransitionPageTurn();
	pTransition->initWithDuration(t,scene,backwards);
	pTransition->autorelease();
	return pTransition;
}

/** initializes a transition with duration and incoming scene */
bool CCTransitionPageTurn::initWithDuration(ccTime t, CCScene *scene, bool backwards)
{
	// XXX: needed before [super init]
	m_bBack = backwards;

	if( CCTransitionScene::initWithDuration(t, scene) )
	{
		// do something
	}
	return true;
}

void CCTransitionPageTurn::sceneOrder()
{
	m_bIsInSceneOnTop = m_bBack;
}

void CCTransitionPageTurn::onEnter()
{
	CCTransitionScene::onEnter();
	CCSize s = CCDirector::sharedDirector()->getWinSize();
	int x,y;
	if( s.width > s.height)
	{
		x=16;
        y=12;
	}
	else
	{
		x=12;
        y=16;
	}

	CCActionInterval *action  = this->actionWithSize(CCSizeMake(x,y));

	if(! m_bBack )
	{
		m_pOutScene->runAction
		(
			CCSequence::create
			(
				action,
				CCCallFunc::create(this, callfunc_selector(CCTransitionScene::finish)),
				CCStopGrid::action(),
				NULL
			)
		);
	}
	else
	{
		// to prevent initial flicker
		m_pInScene->setVisible(false);
		m_pInScene->runAction
		(
			CCSequence::create
			(
			    CCShow::create(),
				action,
				CCCallFunc::create(this, callfunc_selector(CCTransitionScene::finish)),
				CCStopGrid::action(),
				NULL
			)
		);
	}
}


CCActionInterval* CCTransitionPageTurn:: actionWithSize(const CCSize& vector)
{
	if( m_bBack )
	{
		// Get hold of the PageTurn3DAction
		return CCReverseTime::create
		(
			CCPageTurn3D::actionWithSize(vector, m_fDuration)
		);
	}
	else
	{
		// Get hold of the PageTurn3DAction
		return CCPageTurn3D::actionWithSize(vector, m_fDuration);
	}
}

NS_CC_END
