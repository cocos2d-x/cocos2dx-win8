/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2010      Stuart Carnie
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
#include "CCProfiling.h"



using namespace std;

NS_CC_BEGIN

//#pragma mark - Profiling Categories
/* set to NO the categories that you don't want to profile */
bool kCCProfilerCategorySprite = false;
bool kCCProfilerCategoryBatchSprite = false;
bool kCCProfilerCategoryParticles = false;


static CCProfiler* g_sSharedProfiler = NULL;

CCProfiler* CCProfiler::sharedProfiler(void)
{
    if (! g_sSharedProfiler)
    {
        g_sSharedProfiler = new CCProfiler();
        g_sSharedProfiler->init();
    }

    return g_sSharedProfiler;
}

CCProfilingTimer* CCProfiler::createAndAddTimerWithName(const char* timerName)
{
    CCProfilingTimer *t = new CCProfilingTimer();
    t->initWithName(timerName);
    m_pActiveTimers->setObject(t, timerName);
    t->release();

    return t;
}

void CCProfiler::releaseTimer(const char* timerName)
{
    m_pActiveTimers->removeObjectForKey(timerName);
}

void CCProfiler::releaseAllTimers()
{
    m_pActiveTimers->removeAllObjects();
}

bool CCProfiler::init()
{
    m_pActiveTimers = new CCDictionary();
    return true;
}

CCProfiler::~CCProfiler(void)
{
    CC_SAFE_RELEASE(m_pActiveTimers);
}

void CCProfiler::displayTimers()
{
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(m_pActiveTimers, pElement)
    {
        CCProfilingTimer* timer = (CCProfilingTimer*)pElement->getObject();
        CCLog(timer->description());
    }
}

// implementation of CCProfilingTimer

bool CCProfilingTimer::initWithName(const char* timerName)
{
    m_NameStr = timerName;
    numberOfCalls = 0;
    m_dAverageTime = 0.0;
    totalTime = 0.0;
    minTime = 10000.0;
    maxTime = 0.0;
    gettimeofday((struct timeval *)&m_sStartTime, NULL);

    return true;
}

CCProfilingTimer::~CCProfilingTimer(void)
{
    
}

const char* CCProfilingTimer::description()
{
    static char s_szDesciption[256] = {0};
    sprintf(s_szDesciption, "%s: avg time, %fms", m_NameStr.c_str(), m_dAverageTime);
    return s_szDesciption;
}

void CCProfilingTimer::reset()
{
    numberOfCalls = 0;
    m_dAverageTime = 0;
    totalTime = 0;
    minTime = 10000;
    maxTime = 0;
    gettimeofday((struct timeval *)&m_sStartTime, NULL);
}

void CCProfilingBeginTimingBlock(const char *timerName)
{
    CCProfiler* p = CCProfiler::sharedProfiler();
    CCProfilingTimer* timer = (CCProfilingTimer*)p->m_pActiveTimers->objectForKey(timerName);
    if( ! timer )
    {
        timer = p->createAndAddTimerWithName(timerName);
    }

    gettimeofday((struct timeval *)&timer->m_sStartTime, NULL);

    timer->numberOfCalls++;
}

void CCProfilingEndTimingBlock(const char *timerName)
{
    CCProfiler* p = CCProfiler::sharedProfiler();
    CCProfilingTimer* timer = (CCProfilingTimer*)p->m_pActiveTimers->objectForKey(timerName);

    CCAssert(timer, "CCProfilingTimer  not found");

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    double duration = CCTime::timersubCocos2d((struct cc_timeval *)&timer->m_sStartTime, (struct cc_timeval *)&currentTime);

    // milliseconds
    timer->m_dAverageTime = (timer->m_dAverageTime + duration) / 2.0f;
    timer->totalTime += duration;
    timer->maxTime = MAX( timer->maxTime, duration);
    timer->minTime = MIN( timer->minTime, duration);

}

void CCProfilingResetTimingBlock(const char *timerName)
{
    CCProfiler* p = CCProfiler::sharedProfiler();
    CCProfilingTimer *timer = (CCProfilingTimer*)p->m_pActiveTimers->objectForKey(timerName);

    CCAssert(timer, "CCProfilingTimer not found");

    timer->reset();
}

NS_CC_END
