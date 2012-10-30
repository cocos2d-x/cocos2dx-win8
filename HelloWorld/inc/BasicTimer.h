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

#pragma once

#include <wrl\client.h>

ref class BasicTimer sealed
{
private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_currentTime;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTime;
    float m_total;
    float m_delta;

public:
    BasicTimer()
    {
        if (!QueryPerformanceFrequency(&m_frequency))
        {
            throw ref new Platform::FailureException();
        }
        Reset();
    }



    void Reset()
    {
        Update();
        m_startTime = m_currentTime;
        m_total = 0.0f;
        m_delta = 1.0f / 60.0f;
    }

    void Update()
    {
        if (!QueryPerformanceCounter(&m_currentTime))
        {
            throw ref new Platform::FailureException();
        }

        m_total = static_cast<float>(
            static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) /
            static_cast<double>(m_frequency.QuadPart)
            );

        if (m_lastTime.QuadPart==m_startTime.QuadPart)
        {
            // If the timer was just reset, report a time delta equivalent to 60Hz frame time.
            m_delta = 1.0f / 60.0f;
        }
        else
        {
            m_delta = static_cast<float>(
                static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) /
                static_cast<double>(m_frequency.QuadPart)
                );
        }

        m_lastTime = m_currentTime;
    }

    property float Total
    {
        float get() { return m_total; }
    }

    property float Delta
    {
        float get() { return m_delta; }
    }
};
