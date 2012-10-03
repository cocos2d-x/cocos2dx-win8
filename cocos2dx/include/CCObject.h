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

#ifndef __COCOA_NSOBJECT_H__
#define __COCOA_NSOBJECT_H__

#include "CCCommon.h"


namespace   cocos2d {
class CCZone;
class CCObject;
class CCString;

class CC_DLL CCCopying
{
public:
	virtual CCObject* copyWithZone(CCZone* pZone);
};

class CC_DLL CCObject : public CCCopying
{
protected:
	// object id
    unsigned int		m_uID;
	// count of refrence
	unsigned int		m_uReference;
	// is the object autoreleased
	bool		m_bManaged;		
public:
	CCObject(void);
	virtual ~CCObject(void);
    
    void release(void);
	void retain(void);
	CCObject* autorelease(void);
	CCObject* copy(void);
	bool isSingleRefrence(void);
	unsigned int retainCount(void);
	virtual bool isEqual(const CCObject* pObject);
	friend class CCAutoreleasePool;
};
}//namespace   cocos2d 

#endif // __COCOA_NSOBJECT_H__
