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
#include "CCAutoreleasePool.h"
#include "ccMacros.h"

namespace cocos2d 
{

CCPoolManager	g_PoolManager;

CCAutoreleasePool::CCAutoreleasePool(void)
{
	m_pManagedObjectArray = new CCMutableArray<CCObject*>();
}

CCAutoreleasePool::~CCAutoreleasePool(void)
{
	CC_SAFE_DELETE(m_pManagedObjectArray);
}

void CCAutoreleasePool::addObject(CCObject* pObject)
{
	m_pManagedObjectArray->addObject(pObject);

	CCAssert(pObject->m_uReference > 1, "reference count should greager than 1");

	pObject->release(); // no ref count, in this case autorelease pool added.
}

void CCAutoreleasePool::removeObject(CCObject* pObject)
{
	m_pManagedObjectArray->removeObject(pObject, false);
}

void CCAutoreleasePool::clear()
{
	if(m_pManagedObjectArray->count() > 0)
	{
		//CCAutoreleasePool* pReleasePool;
#ifdef _DEBUG
		int nIndex = m_pManagedObjectArray->count() - 1;
#endif
		CCMutableArray<CCObject*>::CCMutableArrayRevIterator it;
		for(it = m_pManagedObjectArray->rbegin(); it != m_pManagedObjectArray->rend(); ++it)
		{
			if(!*it)
				break;

			(*it)->m_bManaged = false;
			//(*it)->release();
			//delete (*it);
#ifdef _DEBUG
			nIndex--;
#endif
		}

		m_pManagedObjectArray->removeAllObjects();
	}
}


//--------------------------------------------------------------------
//
// CCPoolManager
//
//--------------------------------------------------------------------

CCPoolManager* CCPoolManager::getInstance()
{
	return &g_PoolManager;
}

CCPoolManager::CCPoolManager()
{
	m_pReleasePoolStack = new CCMutableArray<CCAutoreleasePool*>();	
        m_pCurReleasePool = 0;
}

CCPoolManager::~CCPoolManager()
{
	
	finalize();

	// we only release the last autorelease pool here 
        m_pCurReleasePool = 0;
	m_pReleasePoolStack->removeObjectAtIndex(0);

	CC_SAFE_DELETE(m_pReleasePoolStack);
}

void CCPoolManager::finalize()
{
	if(m_pReleasePoolStack->count() > 0)
	{
		//CCAutoreleasePool* pReleasePool;
		CCMutableArray<CCAutoreleasePool*>::CCMutableArrayIterator it;
		for(it = m_pReleasePoolStack->begin(); it != m_pReleasePoolStack->end(); ++it)
		{
			if(!*it)
				break;

			(*it)->clear();
		}
	}
}

void CCPoolManager::push()
{
	CCAutoreleasePool* pPool = new CCAutoreleasePool();	   //ref = 1
	m_pCurReleasePool = pPool;

	m_pReleasePoolStack->addObject(pPool);				   //ref = 2

	pPool->release();									   //ref = 1
}

void CCPoolManager::pop()
{
    if (! m_pCurReleasePool)
    {
        return;
    }

 	int nCount = m_pReleasePoolStack->count();

	m_pCurReleasePool->clear();
 
  	if(nCount > 1)
  	{
		m_pReleasePoolStack->removeObjectAtIndex(nCount-1);

// 		if(nCount > 1)
// 		{
// 			m_pCurReleasePool = m_pReleasePoolStack->getObjectAtIndex(nCount - 2);
// 			return;
// 		}
		m_pCurReleasePool = m_pReleasePoolStack->getObjectAtIndex(nCount - 2);
	}

	/*m_pCurReleasePool = NULL;*/
}

void CCPoolManager::removeObject(CCObject* pObject)
{
	CCAssert(m_pCurReleasePool, "current auto release pool should not be null");

	m_pCurReleasePool->removeObject(pObject);
}

void CCPoolManager::addObject(CCObject* pObject)
{
	getCurReleasePool()->addObject(pObject);
}


CCAutoreleasePool* CCPoolManager::getCurReleasePool()
{
	if(!m_pCurReleasePool)
	{
		push();
	}

	CCAssert(m_pCurReleasePool, "current auto release pool should not be null");

	return m_pCurReleasePool;
}

}
