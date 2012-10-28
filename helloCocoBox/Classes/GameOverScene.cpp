/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2010      Ray Wenderlich
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

#include "GameOverScene.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

bool GameOverScene::init()
{
	if( CCScene::init() )
	{
		this->_layer = GameOverLayer::node();
		this->_layer->retain();
		this->addChild(_layer);
		
		return true;
	}
	else
	{
		return false;
	}
}

GameOverScene::~GameOverScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}


bool GameOverLayer::init()
{

	if ( CCLayerColor::initWithColor( ccc4f(255,0,255,255) ) )
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		this->_label = CCLabelTTF::labelWithString("","Arial", 32);
		_label->retain();
		_label->setColor( ccc3(255, 255, 255) );
		_label->setPosition( ccp(winSize.width/2, winSize.height/2) );
		//_label->setPosition(ccp(0,0));
		this->addChild(_label);
		
		this->runAction( CCSequence::actions(
		CCDelayTime::actionWithDuration(1),
		CCCallFunc::actionWithTarget(this, 
						callfunc_selector(GameOverLayer::gameOverDone)),
						NULL));
		
		return true;
	}
	else
	{
		return false;
	}
}

void GameOverLayer::gameOverDone()
{
	CCDirector::sharedDirector()->replaceScene( HelloWorld::scene() );
}

GameOverLayer::~GameOverLayer()
{
	if (_label)
	{
		_label->release();
		_label = NULL;
	}
}
