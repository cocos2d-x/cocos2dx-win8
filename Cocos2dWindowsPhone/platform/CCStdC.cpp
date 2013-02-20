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

#include "pch.h"

#include "CCStdC.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)

int CC_DLL gettimeofday(struct timeval * val, struct timezone *)
{
    if (val)
    {
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);

        struct tm tTm;
        tTm.tm_year     = wtm.wYear - 1900;
        tTm.tm_mon      = wtm.wMonth - 1;
        tTm.tm_mday     = wtm.wDay;
        tTm.tm_hour     = wtm.wHour;
        tTm.tm_min      = wtm.wMinute;
        tTm.tm_sec      = wtm.wSecond;
        tTm.tm_isdst    = -1;

        val->tv_sec     = (long)mktime(&tTm);       // time_t is 64-bit on win32
        val->tv_usec    = wtm.wMilliseconds * 1000;
    }
    return 0;
}
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)

using namespace Osp::System;

int CC_DLL gettimeofday(struct timeval * val, struct timezone *)
{
    if (val)
    {
    	long long curTick = 0;
    	SystemTime::GetTicks(curTick);
    	unsigned int ms = curTick;
    	val->tv_sec = ms / 1000;
    	val->tv_usec = (ms % 1000) * 1000;
    }
    return 0;
}


#endif  // CC_PLATFORM_WIN32
