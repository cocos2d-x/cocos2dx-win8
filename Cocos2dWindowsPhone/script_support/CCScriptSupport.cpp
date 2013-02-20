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
#include "CCScriptSupport.h"
#include "CCScheduler.h"

NS_CC_BEGIN

CCSchedulerScriptHandlerEntry* CCSchedulerScriptHandlerEntry::entryWithHandler(int nHandler, ccTime fInterval, bool bPaused)
{
    CCSchedulerScriptHandlerEntry* pEntry = new CCSchedulerScriptHandlerEntry();
    pEntry->initWithHandler(nHandler, fInterval, bPaused);
    pEntry->autorelease();
    return pEntry;
}

bool CCSchedulerScriptHandlerEntry::initWithHandler(int nHandler, ccTime fInterval, bool bPaused)
{
    m_pTimer = new CCTimer();
    m_pTimer->initWithScriptHandler(nHandler, fInterval);
    m_pTimer->autorelease();
    m_pTimer->retain();
    m_nHandler = nHandler;
    m_bPaused = bPaused;
    LUALOG("[LUA] ADD script schedule: %d, entryID: %d", m_nHandler, m_nEntryID);
    return true;
}

CCSchedulerScriptHandlerEntry::CCSchedulerScriptHandlerEntry(void)
: m_pTimer(NULL)
, m_nHandler(0)
, m_bPaused(true)
, m_bMarkedForDeletion(false)
{
    static int nEntryCount = 0;
    m_nEntryID = ++nEntryCount;
}

CCSchedulerScriptHandlerEntry::~CCSchedulerScriptHandlerEntry(void)
{
    m_pTimer->release();
    CCScriptEngineManager::sharedManager()->getScriptEngine()->removeLuaHandler(m_nHandler);
    LUALOG("[LUA] DEL script schedule %d, entryID: %d", m_nHandler, m_nEntryID);
}

// ----------------------------


CCTouchScriptHandlerEntry* CCTouchScriptHandlerEntry::entryWithHandler(int nHandler, bool bIsMultiTouches, int nPriority, bool bSwallowsTouches)
{
    CCTouchScriptHandlerEntry* pEntry = new CCTouchScriptHandlerEntry();
    pEntry->initWithHandler(nHandler, bIsMultiTouches, nPriority, bSwallowsTouches);
    pEntry->autorelease();
    return pEntry;
}

CCTouchScriptHandlerEntry::CCTouchScriptHandlerEntry(void)
: m_nHandler(0)
, m_bIsMultiTouches(false)
, m_nPriority(0)
, m_bSwallowsTouches(false)
{
}

CCTouchScriptHandlerEntry::~CCTouchScriptHandlerEntry(void)
{
    CCScriptEngineManager::sharedManager()->getScriptEngine()->removeLuaHandler(m_nHandler);
    LUALOG("[LUA] Remove touch event handler: %d", m_nHandler);
}

bool CCTouchScriptHandlerEntry::initWithHandler(int nHandler, bool bIsMultiTouches, int nPriority, bool bSwallowsTouches)
{
    m_nHandler = nHandler;
    m_bIsMultiTouches = bIsMultiTouches;
    m_nPriority = nPriority;
    m_bSwallowsTouches = bSwallowsTouches;
    
    return true;
}

// ----------------------------


static CCScriptEngineManager* s_pSharedScriptEngineManager = NULL;


CCScriptEngineManager::~CCScriptEngineManager(void)
{
    removeScriptEngine();
}

void CCScriptEngineManager::setScriptEngine(CCScriptEngineProtocol *pScriptEngine)
{
    removeScriptEngine();
	m_pScriptEngine = pScriptEngine;
    m_pScriptEngine->retain();
}

void CCScriptEngineManager::removeScriptEngine(void)
{
    if (m_pScriptEngine)
    {
        m_pScriptEngine->release();
        m_pScriptEngine = NULL;
    }
}

CCScriptEngineManager* CCScriptEngineManager::sharedManager(void)
{
    if (!s_pSharedScriptEngineManager)
    {
        s_pSharedScriptEngineManager = new CCScriptEngineManager();
    }
    return s_pSharedScriptEngineManager;
}

void CCScriptEngineManager::purgeSharedManager(void)
{
    if (s_pSharedScriptEngineManager)
    {
        delete s_pSharedScriptEngineManager;
        s_pSharedScriptEngineManager = NULL;
    }
}

NS_CC_END;
