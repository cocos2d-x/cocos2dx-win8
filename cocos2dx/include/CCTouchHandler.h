/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2009      Valentin Milea
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

#ifndef __TOUCH_DISPATCHER_CCTOUCH_HANDLER_H__
#define __TOUCH_DISPATCHER_CCTOUCH_HANDLER_H__

#include "CCTouchDelegateProtocol.h"
#include "CCTouchDispatcher.h"
#include "CCObject.h"
#include "CCSet.h"
namespace   cocos2d {

/**
 CCTouchHandler
 Object than contains the delegate and priority of the event handler.
*/
class CC_DLL  CCTouchHandler : public CCObject
{
public:
	virtual ~CCTouchHandler(void);

	/** delegate */
	CCTouchDelegate* getDelegate();
	void setDelegate(CCTouchDelegate *pDelegate);

	/** priority */
	int getPriority(void);
	void setPriority(int nPriority);

	/** enabled selectors */
	int getEnabledSelectors(void);
	void setEnalbedSelectors(int nValue);

	/** initializes a TouchHandler with a delegate and a priority */
	virtual bool initWithDelegate(CCTouchDelegate *pDelegate, int nPriority);

public:
	/** allocates a TouchHandler with a delegate and a priority */
	static CCTouchHandler* handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority);

protected:
	CCTouchDelegate *m_pDelegate;
	int m_nPriority;
	int m_nEnabledSelectors;
};

/** CCStandardTouchHandler
 It forwardes each event to the delegate.
 */
class CC_DLL  CCStandardTouchHandler : public CCTouchHandler
{
public:
	/** initializes a TouchHandler with a delegate and a priority */
	virtual bool initWithDelegate(CCTouchDelegate *pDelegate, int nPriority);

public:
	/** allocates a TouchHandler with a delegate and a priority */
	static CCStandardTouchHandler* handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority);
};

/**
 CCTargetedTouchHandler
 Object than contains the claimed touches and if it swallos touches.
 Used internally by TouchDispatcher
 */
class CC_DLL  CCTargetedTouchHandler : public CCTouchHandler
{
public:
    ~CCTargetedTouchHandler(void);

    /** whether or not the touches are swallowed */
	bool isSwallowsTouches(void);
	void setSwallowsTouches(bool bSwallowsTouches);

	/** MutableSet that contains the claimed touches */
	NSMutableSet* getClaimedTouches(void);

	/** initializes a TargetedTouchHandler with a delegate, a priority and whether or not it swallows touches or not */
	bool initWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow);

public:
	/** allocates a TargetedTouchHandler with a delegate, a priority and whether or not it swallows touches or not */
	static CCTargetedTouchHandler* handlerWithDelegate(CCTouchDelegate *pDelegate, int nPriority, bool bSwallow);

protected:
	bool m_bSwallowsTouches;
	NSMutableSet *m_pClaimedTouches;
};
}//namespace   cocos2d 

#endif // __TOUCH_DISPATCHER_CCTOUCH_HANDLER_H__
