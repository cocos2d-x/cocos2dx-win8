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

#ifndef __CC_APPLICATION_PLATFORM_H__
#define __CC_APPLICATION_PLATFORM_H__

#include "CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    #include "win32/CCApplication_win32.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "android/CCApplication_android.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    # include "ios/CCApplication_ios.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)
    #include "wophone/CCApplication_wophone.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
	#include "marmalade/CCApplication_marmalade.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
	#include "Linux/CCApplication_linux.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)
	#include "bada/CCApplication_bada.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
    #include "qnx/CCApplication_qnx.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)
    #include "win8_metro\CCApplication_win8_metro.h"
#else
    #error
#endif

#endif	// __CC_APPLICATION_PLATFORM_H__
