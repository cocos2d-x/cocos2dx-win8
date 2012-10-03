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

#ifndef __SCRIPT_SUPPORT_H__
#define __SCRIPT_SUPPORT_H__

#include <string>
#include <vector>
#include <map>

#include "CCCommon.h"
#include "CCTouch.h"
#include "CCSet.h"
#include "CCNode.h"

/*
 * Only one script is support at a time.
 * Should we use more than one script at a time?
 */

NS_CC_BEGIN;

class CC_DLL CCScriptEngineProtocol
{
public:
	CCScriptEngineProtocol();

	// functions for excute touch event
	virtual bool executeTouchEvent(const char *pszFuncName, CCTouch *pTouch) = 0;
	virtual bool executeTouchesEvent(const char *pszFuncName, CCSet *pTouches) = 0;

	// functions for CCCallFuncX
	virtual bool executeCallFunc(const char *pszFuncName) = 0;
	virtual bool executeCallFuncN(const char *pszFuncName, CCNode *pNode) = 0;
	virtual bool executeCallFuncND(const char *pszFuncName, CCNode *pNode, void *pData) = 0;
	virtual bool executeCallFunc0(const char *pszFuncName, CCObject *pObject) = 0;

	// excute a script function without params
	virtual int executeFuction(const char *pszFuncName) = 0;
	// excute a script file
	virtual bool executeScriptFile(const char* pszFileName) = 0;
	// excute script from string
	virtual bool executeString(const char* pszCodes) = 0;

	// execute a schedule function
	virtual bool executeSchedule(const char* pszFuncName, ccTime t) = 0;
    // add a search path  
    virtual bool addSearchPath(const char* pszPath) = 0;
};

/**
 CCScriptEngineManager is a singleton which holds an object instance of CCScriptEngineProtocl
 It helps cocos2d-x and the user code to find back LuaEngine object
 @since v0.99.5-x-0.8.5
 */
class CC_DLL CCScriptEngineManager
{
public:
    static CCScriptEngineManager* sharedScriptEngineManager();
    
	void setScriptEngine(CCScriptEngineProtocol *pScriptEngine);
	CCScriptEngineProtocol* getScriptEngine();
    void removeScriptEngine();
    
private:
    CCScriptEngineManager();
	virtual ~CCScriptEngineManager();
    
	CCScriptEngineProtocol *m_pScriptEngine;
};

NS_CC_END;

#endif // __SCRIPT_SUPPORT_H__
