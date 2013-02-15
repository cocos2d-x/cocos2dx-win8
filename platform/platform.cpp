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

#include "platform.h"

#include "CCStdC.h"

NS_CC_BEGIN;

int CCTime::gettimeofdayCocos2d(struct cc_timeval *tp, void *tzp)
{
    CC_UNUSED_PARAM(tzp);
    if (tp)
    {
        gettimeofday((struct timeval *)tp,  0);
    }
    return 0;
}

void CCTime::timersubCocos2d(struct cc_timeval *out, struct cc_timeval *start, struct cc_timeval *end)
{
    if (! out || ! start || ! end)
    {
        return;
    }
    out->tv_sec = end->tv_sec - start->tv_sec;
    out->tv_usec = end->tv_usec - start->tv_usec;
}

NS_CC_END;
