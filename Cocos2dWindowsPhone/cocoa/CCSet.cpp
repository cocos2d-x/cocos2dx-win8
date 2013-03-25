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

#include "CCSet.h"

using namespace std;

NS_CC_BEGIN

CCSet::CCSet(void)
{
    m_pSet = new set<CCObject *>;
}

CCSet::CCSet(const CCSet &rSetObject)
{
    m_pSet = new set<CCObject *>(*rSetObject.m_pSet);

	// call retain of members
	CCSetIterator iter;
	for (iter = m_pSet->begin(); iter != m_pSet->end(); ++iter)
	{
		if (! (*iter))
		{
			break;
		}

		(*iter)->retain();
	}
}

CCSet::~CCSet(void)
{
	// call release() of elements
	CCSetIterator iter;
	for (iter = m_pSet->begin(); iter != m_pSet->end(); ++iter)
	{
		if (! (*iter))
		{
			break;
		}

		(*iter)->release();
	}

	CC_SAFE_DELETE(m_pSet);
}

CCSet* CCSet::copy(void)
{
	CCSet *pSet = new CCSet(*this);

	return pSet;
}

CCSet* CCSet::mutableCopy(void)
{
	return copy();
}

int CCSet::count(void)
{
	return (int)m_pSet->size();
}

void CCSet::addObject(CCObject *pObject)
{
	CC_SAFE_RETAIN(pObject);
	m_pSet->insert(pObject);
}

void CCSet::removeObject(CCObject *pObject)
{
	m_pSet->erase(pObject);
	CC_SAFE_RELEASE(pObject);
}

bool CCSet::containsObject(CCObject *pObject)
{
	return m_pSet->find(pObject) != m_pSet->end();
}

CCSetIterator CCSet::begin(void)
{
	return m_pSet->begin();
}

CCSetIterator CCSet::end(void)
{
	return m_pSet->end();
}

CCObject* CCSet::anyObject()
{
    if (!m_pSet || m_pSet->empty())
    {
        return 0;
    }
    
    CCSetIterator it;

    for( it = m_pSet->begin(); it != m_pSet->end(); ++it)
    {
        if (*it)
        {
            return (*it);
        }
    }

    return 0;
}

NS_CC_END
