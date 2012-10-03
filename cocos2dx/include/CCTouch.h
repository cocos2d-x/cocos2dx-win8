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

#ifndef __CC_TOUCH_H__
#define __CC_TOUCH_H__

#include "CCObject.h"
#include "CCGeometry.h"

namespace cocos2d {

class CCTouch : public CCObject
{
public:
    CCTouch() {}
    CCTouch(int nViewId, float x, float y) : m_nViewId(nViewId), m_point(x, y), m_prevPoint(x, y) {}

    CCPoint locationInView(int nViewId) {CC_UNUSED_PARAM(nViewId); return m_point; }
    CCPoint previousLocationInView(int nViewId) {CC_UNUSED_PARAM(nViewId); return m_prevPoint; }
    int view() { return m_nViewId; }
    int id(){ return m_iID; }

    void SetTouchInfo(int nViewId, float x, float y, int iID=0)
    {
        m_nViewId   = nViewId;
        m_prevPoint = m_point;
        m_point.x   = x;
        m_point.y   = y;
        m_iID       = iID;
    }

private:
    int     m_nViewId;
    CCPoint m_point;
    CCPoint	m_prevPoint;
    int     m_iID;
};

class CCEvent : public CCObject
{
};

}       // end of namespace cocos2d

#endif  // __PLATFORM_TOUCH_H__
