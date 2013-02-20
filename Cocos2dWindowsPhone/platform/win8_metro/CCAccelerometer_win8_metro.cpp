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
#include "CCAccelerometer_win8_metro.h"
#include "CCAccelerometerDelegate.h"
using namespace Windows::Foundation;
using namespace Windows::Devices::Sensors;

namespace cocos2d
{
	Windows::Devices::Sensors::Accelerometer^ m_accelerometer;
	CCAccelerometer::CCAccelerometer() : m_pAccelDelegate(NULL)
	{

	}

	CCAccelerometer::~CCAccelerometer() 
	{

	}

	bool CCAccelerometer::init(void)
	{
		return true;
	}

	static CCAccelerometer *pSharedAccelerometer = NULL;

	CCAccelerometer* CCAccelerometer::sharedAccelerometer()
	{
		if(pSharedAccelerometer == NULL)
		{
			pSharedAccelerometer = new CCAccelerometer();
			pSharedAccelerometer->init();
		}
		return pSharedAccelerometer;
	}

	void CCAccelerometer::setDelegate(CCAccelerometerDelegate* pDelegate) 
	{
		//m_pAccelDelegate = pDelegate;
		//

		//if (pDelegate)
		//{
		//	m_accelerometer = Accelerometer::GetDefault();

		//	if(m_accelerometer != nullptr){

		//		m_accelerometer->ReadingChanged += ref new TypedEventHandler
		//			<Accelerometer^,AccelerometerReadingChangedEventArgs^>
		//			([=](Accelerometer^, AccelerometerReadingChangedEventArgs^)
		//		{

		//			AccelerometerReading^ reading = m_accelerometer->GetCurrentReading();
		//			m_obAccelerationValue.x = reading->AccelerationX;
		//			m_obAccelerationValue.y = reading->AccelerationY;
		//			m_obAccelerationValue.z = reading->AccelerationZ;
		//			m_pAccelDelegate->didAccelerate(&m_obAccelerationValue);
		//		});
		//	}
		//}
		//else
		//{
		//	m_accelerometer = nullptr;
		//}
	}
	
}