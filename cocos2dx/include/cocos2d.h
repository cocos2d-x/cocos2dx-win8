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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   01 00 01
#define COCOS2D_VERSION 0x00010001

//
// all cocos2d include files
//

#include "ccConfig.h"	// should be included first
#include "CCAction.h"
#include "CCAnimation.h"
#include "CCAnimationCache.h"
#include "CCActionManager.h"
#include "CCCamera.h"
#include "CCActionCamera.h"
#include "CCProtocols.h"
#include "CCNode.h"
#include "CCDirector.h"
#include "CCUserDefault.h"
#include "CCActionInstant.h"
#include "CCActionInterval.h"
#include "CCActionEase.h"
#include "CCLabelTTF.h"
#include "CCLayer.h"
#include "CCMenu.h"
#include "CCMenuItem.h"
#include "CCParticleSystem.h"
#include "CCParticleSystemPoint.h"
#include "CCParticleSystemQuad.h"
#include "CCParticleExamples.h"
#include "CCScene.h"
#include "CCSprite.h"
#include "CCSpriteFrameCache.h"
#include "CCTextureCache.h"
#include "CCTransition.h"
#include "CCTextureAtlas.h"
#include "CCLabelAtlas.h"
#include "CCAtlasNode.h"
#include "CCActionTiledGrid.h"
#include "CCActionGrid3D.h"
#include "CCActionGrid.h"
#include "CCLabelBMFont.h"
#include "CCParallaxNode.h"
#include "CCTileMapAtlas.h"
#include "CCTMXTiledMap.h"
#include "CCTMXLayer.h"
#include "CCTMXObjectGroup.h"
#include "CCTMXXMLParser.h"
#include "CCRenderTexture.h"
#include "CCMotionStreak.h"
#include "CCActionPageTurn3D.h"
#include "CCTransitionPageTurn.h"
#include "CCTexture2D.h"
#include "CCTexturePVR.h"
#include "CCTransitionRadial.h"
#include "CCActionProgressTimer.h"
#include "CCTouchHandler.h"
#include "CCTouchDispatcher.h"
#include "CCDrawingPrimitives.h"
#include "CCScheduler.h"

// havn't implement on wophone and marmalade
#if (CC_TARGET_PLATFORM != CC_PLATFORM_MARMALADE)
#include "CCTextFieldTTF.h"
#endif

//
// cocoa includes
//
#include "CCSet.h"
#include "CCMutableArray.h"
#include "CCMutableDictionary.h"
#include "CCObject.h"
#include "CCZone.h"
#include "CCGeometry.h"
#include "CCAffineTransform.h"
#include "selector_protocol.h"
#include "CCTouch.h"
#include "CCPointExtension.h"

//
// platform specific
//
#include "CCApplication.h"
#include "CCEGLView.h"
#include "CCImage.h"
#include "CCFileUtils.h"
#include "CCAccelerometer.h"
 
//
// cocos2d macros
//
#include "ccTypes.h"
#include "ccMacros.h"

namespace   cocos2d {

const char* cocos2dVersion();

}//namespace   cocos2d 

#endif // __COCOS2D_H__
