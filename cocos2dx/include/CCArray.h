/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010 ForzeField Studios S.L. http://forzefield.com
* Copyright (c) 2010 cocos2d-x.org
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

#ifndef __CCARRAY_H__
#define __CCARRAY_H__

#include "support/data_support/ccCArray.h"

/** @def CCARRAY_FOREACH
A convience macro to iterate over a CCArray using. It is faster than the "fast enumeration" interface.
@since v0.99.4
*/

/*
In cocos2d-iphone 1.0.0, This macro have been update to like this:

#define CCARRAY_FOREACH(__array__, __object__)												\
if (__array__ && __array__->data->num > 0)													\
for(id *__arr__ = __array__->data->arr, *end = __array__->data->arr + __array__->data->num-1;	\
__arr__ <= end && ((__object__ = *__arr__) != nil || true);										\
__arr__++)

I found that it's not work in C++. So it keep what it's look like in version 1.0.0-rc3. ---By Bin
*/
#define CCARRAY_FOREACH(__array__, __object__)												\
    if (__array__ && __array__->data->num > 0)													\
    for(CCObject** arr = __array__->data->arr, **end = __array__->data->arr + __array__->data->num-1;	\
    arr <= end && ((__object__ = *arr) != NULL/* || true*/);										\
    arr++)

namespace cocos2d
{

class CC_DLL CCArray : public CCObject
{
public:
    ~CCArray();
    static CCArray* array();
    static CCArray* arrayWithCapacity(unsigned int capacity);
    static CCArray* arrayWithArray(CCArray* otherArray);

    bool init();
    bool initWithCapacity(unsigned int capacity);
    bool initWithArray(CCArray* otherArray);

    // Querying an Array
    unsigned int count();
    unsigned int capacity();
    unsigned int indexOfObject(CCObject* object);
    CCObject* objectAtIndex(unsigned int index);
    CCObject* lastObject();
    CCObject* randomObject();
    bool containsObject(CCObject* object);

    // Adding Objects
    void addObject(CCObject* object);
    void addObjectsFromArray(CCArray* otherArray);
    void insertObject(CCObject* object, unsigned int index);

    // Removing Objects
    void removeLastObject();
    void removeObject(CCObject* object);
    void removeObjectAtIndex(unsigned int index);
    void removeObjectsInArray(CCArray* otherArray);
    void removeAllObjects();
    void fastRemoveObject(CCObject* object);
    void fastRemoveObjectAtIndex(unsigned int index);

    // Rearranging Content
    void exchangeObject(CCObject* object1, CCObject* object2);
    void exchangeObjectAtIndex(unsigned int index1, unsigned int index2);
    void reverseObjects();
    void reduceMemoryFootprint();

public:
    ccArray* data;

private:
	CCArray() : data(NULL) {};
};

}

#endif // __CCARRAY_H__
