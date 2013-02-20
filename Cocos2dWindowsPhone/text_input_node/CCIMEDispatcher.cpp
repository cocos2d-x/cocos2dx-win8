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

#include "CCIMEDispatcher.h"

#include <list>

NS_CC_BEGIN;

//////////////////////////////////////////////////////////////////////////
// add/remove delegate in CCIMEDelegate Cons/Destructor
//////////////////////////////////////////////////////////////////////////

CCIMEDelegate::CCIMEDelegate()
{
    CCIMEDispatcher::sharedDispatcher()->addDelegate(this);
}

CCIMEDelegate::~CCIMEDelegate()
{
    CCIMEDispatcher::sharedDispatcher()->removeDelegate(this);
}

bool CCIMEDelegate::attachWithIME()
{
    return CCIMEDispatcher::sharedDispatcher()->attachDelegateWithIME(this);
}

bool CCIMEDelegate::detachWithIME()
{
    return CCIMEDispatcher::sharedDispatcher()->detachDelegateWithIME(this);
}

//////////////////////////////////////////////////////////////////////////

typedef std::list< CCIMEDelegate * > DelegateList;
typedef std::list< CCIMEDelegate * >::iterator  DelegateIter;

//////////////////////////////////////////////////////////////////////////
// Delegate List manage class
//////////////////////////////////////////////////////////////////////////

class CCIMEDispatcher::Impl
{
public:
    Impl()
    {
    }

    ~Impl()
    {

    }

    void init()
    {
        m_DelegateWithIme = 0;
    }

    DelegateIter findDelegate(CCIMEDelegate* pDelegate)
    {
        DelegateIter end = m_DelegateList.end();
        for (DelegateIter iter = m_DelegateList.begin(); iter != end; ++iter)
        {
            if (pDelegate == *iter)
            {
                return iter;
            }
        }
        return end;
    }

    DelegateList    m_DelegateList;
    CCIMEDelegate*  m_DelegateWithIme;
};

//////////////////////////////////////////////////////////////////////////
// Cons/Destructor
//////////////////////////////////////////////////////////////////////////

CCIMEDispatcher::CCIMEDispatcher()
: m_pImpl(new CCIMEDispatcher::Impl)
{
    m_pImpl->init();
}

CCIMEDispatcher::~CCIMEDispatcher()
{
    CC_SAFE_DELETE(m_pImpl);
}

//////////////////////////////////////////////////////////////////////////
// Add/Attach/Remove CCIMEDelegate
//////////////////////////////////////////////////////////////////////////

void CCIMEDispatcher::addDelegate(CCIMEDelegate* pDelegate)
{
    if (! pDelegate || ! m_pImpl)
    {
        return;
    }
    if (m_pImpl->m_DelegateList.end() != m_pImpl->findDelegate(pDelegate))
    {
        // pDelegate already in list
        return;
    }
    m_pImpl->m_DelegateList.push_front(pDelegate);
}

bool CCIMEDispatcher::attachDelegateWithIME(CCIMEDelegate * pDelegate)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! m_pImpl || ! pDelegate);

        DelegateIter end  = m_pImpl->m_DelegateList.end();
        DelegateIter iter = m_pImpl->findDelegate(pDelegate);

        // if pDelegate is not in delegate list, return
        CC_BREAK_IF(end == iter);

        if (m_pImpl->m_DelegateWithIme)
        {
            // if old delegate canDetachWithIME return false 
            // or pDelegate canAttachWithIME return false,
            // do nothing.
            CC_BREAK_IF(! m_pImpl->m_DelegateWithIme->canDetachWithIME()
                || ! pDelegate->canAttachWithIME());

            // detach first
            CCIMEDelegate * pOldDelegate = m_pImpl->m_DelegateWithIme;
            m_pImpl->m_DelegateWithIme = 0;
            pOldDelegate->didDetachWithIME();

            m_pImpl->m_DelegateWithIme = *iter;
            pDelegate->didAttachWithIME();
            bRet = true;
            break;
        }

        // havn't delegate attached with IME yet
        CC_BREAK_IF(! pDelegate->canAttachWithIME());

        m_pImpl->m_DelegateWithIme = *iter;
        pDelegate->didAttachWithIME();
        bRet = true;
    } while (0);
    return bRet;
}

bool CCIMEDispatcher::detachDelegateWithIME(CCIMEDelegate * pDelegate)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! m_pImpl || ! pDelegate);

        // if pDelegate is not the current delegate attached with ime, return
        CC_BREAK_IF(m_pImpl->m_DelegateWithIme != pDelegate);

        CC_BREAK_IF(! pDelegate->canDetachWithIME());

        m_pImpl->m_DelegateWithIme = 0;
        pDelegate->didDetachWithIME();
        bRet = true;
    } while (0);
    return bRet;
}

void CCIMEDispatcher::removeDelegate(CCIMEDelegate* pDelegate)
{
    do 
    {
        CC_BREAK_IF(! pDelegate || ! m_pImpl);

        DelegateIter iter = m_pImpl->findDelegate(pDelegate);
        DelegateIter end  = m_pImpl->m_DelegateList.end();
        CC_BREAK_IF(end == iter);

        if (m_pImpl->m_DelegateWithIme)

        if (*iter == m_pImpl->m_DelegateWithIme)
        {
            m_pImpl->m_DelegateWithIme = 0;
        }
        m_pImpl->m_DelegateList.erase(iter);
    } while (0);
}

//////////////////////////////////////////////////////////////////////////
// dispatch text message
//////////////////////////////////////////////////////////////////////////

void CCIMEDispatcher::dispatchInsertText(const char * pText, int nLen)
{
    do 
    {
        CC_BREAK_IF(! m_pImpl || ! pText || nLen <= 0);

        // there is no delegate attach with ime
        CC_BREAK_IF(! m_pImpl->m_DelegateWithIme);

        m_pImpl->m_DelegateWithIme->insertText(pText, nLen);
    } while (0);
}

void CCIMEDispatcher::dispatchDeleteBackward()
{
    do 
    {
        CC_BREAK_IF(! m_pImpl);

        // there is no delegate attach with ime
        CC_BREAK_IF(! m_pImpl->m_DelegateWithIme);

        m_pImpl->m_DelegateWithIme->deleteBackward();
    } while (0);
}

const char * CCIMEDispatcher::getContentText()
{
    const char * pszContentText = 0;
    if (m_pImpl && m_pImpl->m_DelegateWithIme)
    {
        pszContentText = m_pImpl->m_DelegateWithIme->getContentText();
    }
    return (pszContentText) ? pszContentText : "";
}

//////////////////////////////////////////////////////////////////////////
// dispatch keyboard message
//////////////////////////////////////////////////////////////////////////

void CCIMEDispatcher::dispatchKeyboardWillShow(CCIMEKeyboardNotificationInfo& info)
{
    if (m_pImpl)
    {
        CCIMEDelegate * pDelegate = 0;
        DelegateIter last = m_pImpl->m_DelegateList.end();
        for (DelegateIter first = m_pImpl->m_DelegateList.begin(); first != last; ++first)
        {
            pDelegate = *(first);
            if (pDelegate)
            {
                pDelegate->keyboardWillShow(info);
            }
        }
    }
}

void CCIMEDispatcher::dispatchKeyboardDidShow(CCIMEKeyboardNotificationInfo& info)
{
    if (m_pImpl)
    {
        CCIMEDelegate * pDelegate = 0;
        DelegateIter last = m_pImpl->m_DelegateList.end();
        for (DelegateIter first = m_pImpl->m_DelegateList.begin(); first != last; ++first)
        {
            pDelegate = *(first);
            if (pDelegate)
            {
                pDelegate->keyboardDidShow(info);
            }
        }
    }
}

void CCIMEDispatcher::dispatchKeyboardWillHide(CCIMEKeyboardNotificationInfo& info)
{
    if (m_pImpl)
    {
        CCIMEDelegate * pDelegate = 0;
        DelegateIter last = m_pImpl->m_DelegateList.end();
        for (DelegateIter first = m_pImpl->m_DelegateList.begin(); first != last; ++first)
        {
            pDelegate = *(first);
            if (pDelegate)
            {
                pDelegate->keyboardWillHide(info);
            }
        }
    }
}

void CCIMEDispatcher::dispatchKeyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
    if (m_pImpl)
    {
        CCIMEDelegate * pDelegate = 0;
        DelegateIter last = m_pImpl->m_DelegateList.end();
        for (DelegateIter first = m_pImpl->m_DelegateList.begin(); first != last; ++first)
        {
            pDelegate = *(first);
            if (pDelegate)
            {
                pDelegate->keyboardDidHide(info);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// protected member function
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////

CCIMEDispatcher* CCIMEDispatcher::sharedDispatcher()
{
    static CCIMEDispatcher s_instance;
    return &s_instance;
}

NS_CC_END;
