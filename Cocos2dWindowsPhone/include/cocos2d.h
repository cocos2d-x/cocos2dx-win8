/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/


#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   02 01 00
#define COCOS2D_VERSION 0x00020100

//
// all cocos2d include files
//
#include "ccConfig.h"	// should be included first
// kazmath
#include "kazmath/kazmath.h"
#include "kazmath/GL/matrix.h"

// actions
// Features of 1.0
#include "CCAction.h"
#include "CCActionInterval.h"
#include "CCActionCamera.h"
#include "CCActionManager.h"
#include "CCActionEase.h"
#include "CCActionPageTurn3D.h"
#include "CCActionGrid.h"
#include "CCActionProgressTimer.h"
#include "CCActionGrid3D.h"
#include "CCActionTiledGrid.h"
#include "CCActionInstant.h"
#include "CCActionTween.h"		
#include "CCActionCatmullRom.h"   

// base_nodes
#include "CCNode.h"
#include "CCAtlasNode.h"

// cocoa includes
#include "CCAffineTransform.h"
// Replace CCDictionary with CCMutableDictionary
//#include "CCDictionary.h"
#include "CCDictionary.h"
#include "CCObject.h"
// Replace CCArray with CCMutableArray
//#include "CCArray.h"
#include "CCArray.h"
#include "CCGeometry.h"
#include "CCSet.h"
#include "CCAutoreleasePool.h"
#include "CCInteger.h"
#include "CCFloat.h"
#include "CCDouble.h"
#include "CCBool.h"
#include "CCString.h"
#include "CCNS.h"
#include "CCZone.h"

// draw nodes
#include "CCDrawingPrimitives.h"
//#include "CCDrawNode.h" // Faster than the "drawing primitives" since they it draws everything in one single batch

// effects
#include "CCGrabber.h"
#include "CCGrid.h"

// include
#include "CCEventType.h"
#include "CCProtocols.h"
#include "ccMacros.h"
#include "ccTypes.h"

// keypad_dispatcher
#include "CCKeypadDelegate.h"
#include "CCKeypadDispatcher.h"


// label_nodes
#include "CCLabelAtlas.h"
#include "CCLabelTTF.h"
#include "CCLabelBMFont.h"

// layers_scenes_transitions_nodes
#include "CCLayer.h"
#include "CCScene.h"
#include "CCTransition.h"
#include "CCTransitionPageTurn.h"
#include "CCTransitionProgress.h" 

// menu_nodes
#include "CCMenu.h"
#include "CCMenuItem.h"

// misc_nodes
//#include "CCClippingNode.h"
#include "CCMotionStreak.h"
//#include "CCProgressTimer.h"
#include "CCRenderTexture.h"

// particle_nodes
#include "CCParticleBatchNode.h"
#include "CCParticleSystem.h"
#include "CCParticleSystemPoint.h"
#include "CCParticleSystemQuad.h"
#include "CCParticleExamples.h"

// platform
#include "CCCommon.h"
#include "CCFileUtils.h"
#include "CCImage.h"
//#include "CCSAXParser.h"
//#include "CCThread.h"
//#include "platform.h"
//#include "CCPlatformConfig.h"
#include "CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #include "platform/ios/CCAccelerometer.h"
    #include "platform/ios/CCApplication.h"
    #include "platform/ios/CCEGLView.h"
    #include "platform/ios/CCGL.h"
    #include "platform/ios/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/CCAccelerometer.h"
    #include "platform/android/CCApplication.h"
    #include "platform/android/CCEGLView.h"
    #include "platform/android/CCGL.h"
    #include "platform/android/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
    #include "platform/blackberry/CCAccelerometer.h"
    #include "platform/blackberry/CCApplication.h"
    #include "platform/blackberry/CCEGLView.h"
    #include "platform/blackberry/CCGL.h"
    #include "platform/blackberry/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#include "CCAccelerometer.h"
	#include "CCApplication.h"
	#include "CCEGLView.h"
	#include "CCGL.h"
	#include "CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)
	#include "CCAccelerometer.h"
	#include "CCApplication.h"
    #include "CCApplication_win8_metro.h"
	#include "CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WINPHONE

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	#include "platform/mac/CCAccelerometer.h"
	#include "platform/mac/CCApplication.h"
	#include "platform/mac/CCEGLView.h"
	#include "platform/mac/CCGL.h"
	#include "platform/mac/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	#include "platform/linux/CCAccelerometer.h"
	#include "platform/linux/CCApplication.h"
	#include "platform/linux/CCEGLView.h"
	#include "platform/linux/CCGL.h"
	#include "platform/linux/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

// script_support
#include "CCScriptSupport.h"

// shaders
//#include "shaders/CCGLProgram.h"
//#include "shaders/ccGLStateCache.h"
//#include "shaders/CCShaderCache.h"
//#include "shaders/ccShaders.h"

// sprite_nodes
#include "CCAnimation.h"
#include "CCAnimationCache.h"
#include "CCSprite.h"
//#include "CCSpriteBatchNode.h"
//#include "CCSpriteFrame.h"
#include "CCSpriteFrameCache.h"

// support
#include "CCNotificationCenter.h"
#include "CCPointExtension.h"
//#include "CCProfiling.h"
//#include "CCUserDefault.h"
//#include "CCVertex.h"


// text_input_node
//#include "CCIMEDelegate.h"
//#include "CCIMEDispatcher.h"
//#include "CCTextFieldTTF.h"


// textures
#include "CCTexture2D.h"
#include "CCTextureAtlas.h"
#include "CCTextureCache.h"
#include "CCTexturePVR.h"


// tilemap_parallax_nodes
#include "CCParallaxNode.h"
#include "CCTMXLayer.h"
#include "CCTMXObjectGroup.h"
#include "CCTMXTiledMap.h"
#include "CCTMXXMLParser.h"
#include "CCTileMapAtlas.h"


// touch_dispatcher
#include "CCTouch.h"
//#include "CCTouchDelegateProtocol.h"
#include "CCTouchDispatcher.h"
#include "CCTouchHandler.h"


// root
#include "CCCamera.h"
//#include "CCConfiguration.h"
#include "CCDirector.h"
#include "CCScheduler.h"


#include "CCUserDefault.h"



// havn't implement on wophone
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WOPHONE)
#include "CCTextFieldTTF.h"
#endif


//
// cocos2d macros
//



NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END

#endif // __COCOS2D_H__
