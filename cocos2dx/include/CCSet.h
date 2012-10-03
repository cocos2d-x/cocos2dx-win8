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

#ifndef __NS_SET_H__
#define __NS_SET_H__

#include <set>
#include "CCObject.h"

namespace   cocos2d {

typedef std::set<CCObject *>::iterator CCSetIterator;

class CC_DLL CCSet : public CCObject
{
public:
	CCSet(void);
	CCSet(const CCSet &rSetObject);
	virtual ~CCSet(void);

	/**
	*@brief Return a copy of the CCSet, it will copy all the elelments.
	*/
	CCSet* copy();
	/**
	*@brief It is the same as copy().
	*/
	CCSet* mutableCopy();
	/**
	*@brief Return the number of elements the CCSet contains.
	*/
	int count();
	/**
	*@brief Add a element into CCSet, it will retain the element.
	*/
	void addObject(CCObject *pObject);
	/**
	*@brief Remove the given element, nothing todo if no element equals pObject.
	*/
	void removeObject(CCObject *pObject);
	/**
	*@brief Check if CCSet contains a element equals pObject.
	*/
	bool containsObject(CCObject *pObject);
	/**
	*@brief Return the iterator that points to the first element.
	*/
	CCSetIterator begin();
	/**
	*@brief Return the iterator that points to the poisition after the last element.
	*/
	CCSetIterator end();
	/**
	*@brief Return the first element if it contains elements, or null if it doesn't contain any element.
	*/
    CCObject* anyObject();

private:
    std::set<CCObject *> *m_pSet;
};

typedef CCSet NSMutableSet;
}//namespace   cocos2d 

#endif // __NS_SET_H__

