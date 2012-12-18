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

#pragma once

#include "CCCommon.h"  

NS_CC_BEGIN;

class CCRect;

class CC_DLL CCApplication
{
public:
    CCApplication();
    virtual ~CCApplication();

    /**
    @brief	Implement for initialize OpenGL instance, set source path, etc...
    */
    virtual bool initInstance() = 0;

    /**
    @brief	Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching() = 0;

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground() = 0;

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground() = 0;

    virtual void applicationViewStateChanged(int newState, int oldState) = 0;

    /**
    @brief	Callback by CCDirector for limit FPS.
    @interval       The time, which expressed in second in second, between current frame and next. 
    */
    void setAnimationInterval(double interval);

    typedef enum
    {
        /// Device oriented vertically, home button on the bottom
        kOrientationPortrait = 0,
        /// Device oriented vertically, home button on the top
        kOrientationPortraitUpsideDown = 1,
        /// Device oriented horizontally, home button on the right
        kOrientationLandscapeLeft = 2,
        /// Device oriented horizontally, home button on the left
        kOrientationLandscapeRight = 3,
    } Orientation;

    /**
    @brief	Callback by CCDirector for change device orientation.
    @orientation    The defination of orientation which CCDirector want change to.
    @return         The actual orientation of the application.
    */
    Orientation setOrientation(Orientation orientation);

    /**
    @brief	Get status bar rectangle in EGLView window.
    */
    void    statusBarFrame(CCRect * rect);

    /**
    @brief	Get current applicaiton instance.
    @return Current application instance pointer.
    */
    static CCApplication& sharedApplication();

    /**
    @brief Get current language config
    @return Current language config
    */
    static ccLanguageType getCurrentLanguage();
};

CC_DLL Windows::ApplicationModel::Core::IFrameworkView^ getSharedCCApplicationFrameworkView();

NS_CC_END;
