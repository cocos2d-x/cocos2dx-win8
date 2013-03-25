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

#include "CCScene.h"
#include "CCPointExtension.h"
#include "CCDirector.h"

NS_CC_BEGIN

CCScene::CCScene()
{
	m_bIsRelativeAnchorPoint = false;
	setAnchorPoint(ccp(0.5f, 0.5f));
}

CCScene::~CCScene()
{
}

bool CCScene::init()
{
	bool bRet = false;
 	do 
 	{
 		CCDirector * pDirector;
 		CC_BREAK_IF( ! (pDirector = CCDirector::sharedDirector()) );
 		this->setContentSize(pDirector->getWinSize());
 		// success
 		bRet = true;
 	} while (0);
 	return bRet;
}

CCScene *CCScene::create()
{
	CCScene *pRet = new CCScene();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
    else
    {
	    CC_SAFE_DELETE(pRet)
	    return NULL;
    }
}
NS_CC_END
