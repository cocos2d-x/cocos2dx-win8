/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2011      Zynga Inc.
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
#include "CCActionCamera.h"
#include "CCNode.h"
#include "CCCamera.h"
#include "CCStdC.h"

NS_CC_BEGIN
	//
	// CameraAction
	//
	void CCActionCamera::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);

	CCCamera *camera = pTarget->getCamera();
	camera->getCenterXYZ(&m_fCenterXOrig, &m_fCenterYOrig, &m_fCenterZOrig);
	camera->getEyeXYZ(&m_fEyeXOrig, &m_fEyeYOrig, &m_fEyeZOrig);
	camera->getUpXYZ(&m_fUpXOrig, &m_fUpYOrig, &m_fUpZOrig);
}

CCActionInterval * CCActionCamera::reverse()
{
	return CCReverseTime::create(this);
}
//
// CCOrbitCamera
//
CCOrbitCamera * CCOrbitCamera::create(float t, float radius, float deltaRadius, float angleZ, float deltaAngleZ, float angleX, float deltaAngleX)
{
	CCOrbitCamera * pRet = new CCOrbitCamera();
	if(pRet->initWithDuration(t, radius, deltaRadius, angleZ, deltaAngleZ, angleX, deltaAngleX))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

CCObject * CCOrbitCamera::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	CCOrbitCamera* pRet = NULL;
	if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
		pRet = (CCOrbitCamera*)(pZone->m_pCopyObject);
	else
	{
		pRet = new CCOrbitCamera();
		pZone = pNewZone = new CCZone(pRet);
	}

	CCActionInterval::copyWithZone(pZone);

	pRet->initWithDuration(m_fDuration, m_fRadius, m_fDeltaRadius, m_fAngleZ, m_fDeltaAngleZ, m_fAngleX, m_fDeltaAngleX);

	CC_SAFE_DELETE(pNewZone);
	return pRet;
}

bool CCOrbitCamera::initWithDuration(float t, float radius, float deltaRadius, float angleZ, float deltaAngleZ, float angleX, float deltaAngleX)
{
	if ( CCActionInterval::initWithDuration(t) )
	{
		m_fRadius = radius;
		m_fDeltaRadius = deltaRadius;
		m_fAngleZ = angleZ;
		m_fDeltaAngleZ = deltaAngleZ;
		m_fAngleX = angleX;
		m_fDeltaAngleX = deltaAngleX;

		m_fRadDeltaZ = (CGFloat)CC_DEGREES_TO_RADIANS(deltaAngleZ);
		m_fRadDeltaX = (CGFloat)CC_DEGREES_TO_RADIANS(deltaAngleX);
		return true;
	}
	return false;
}

void CCOrbitCamera::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	float r, zenith, azimuth;
	this->sphericalRadius(&r, &zenith, &azimuth);
	if( isnan(m_fRadius) )
		m_fRadius = r;
	if( isnan(m_fAngleZ) )
		m_fAngleZ = (CGFloat)CC_RADIANS_TO_DEGREES(zenith);
	if( isnan(m_fAngleX) )
		m_fAngleX = (CGFloat)CC_RADIANS_TO_DEGREES(azimuth);

	m_fRadZ = (CGFloat)CC_DEGREES_TO_RADIANS(m_fAngleZ);
	m_fRadX = (CGFloat)CC_DEGREES_TO_RADIANS(m_fAngleX);
}

void CCOrbitCamera::update(ccTime dt)
{
	float r = (m_fRadius + m_fDeltaRadius * dt) * CCCamera::getZEye();
	float za = m_fRadZ + m_fRadDeltaZ * dt;
	float xa = m_fRadX + m_fRadDeltaX * dt;

	float i = sinf(za) * cosf(xa) * r + m_fCenterXOrig;
	float j = sinf(za) * sinf(xa) * r + m_fCenterYOrig;
	float k = cosf(za) * r + m_fCenterZOrig;

	m_pTarget->getCamera()->setEyeXYZ(i,j,k);
}

void CCOrbitCamera::sphericalRadius(float *newRadius, float *zenith, float *azimuth)
{
	float ex, ey, ez, cx, cy, cz, x, y, z;
	float r; // radius
	float s;

	CCCamera* pCamera = m_pTarget->getCamera();
	pCamera->getEyeXYZ(&ex, &ey, &ez);
	pCamera->getCenterXYZ(&cx, &cy, &cz);

	x = ex-cx;
	y = ey-cy;
	z = ez-cz;

	r = sqrtf( powf(x,2) + powf(y,2) + powf(z,2));
	s = sqrtf( powf(x,2) + powf(y,2));
	if( s == 0.0f )
		s = FLT_EPSILON;
	if(r==0.0f)
		r = FLT_EPSILON;

	*zenith = acosf( z/r);
	if( x < 0 )
		*azimuth= (CGFloat)M_PI - asinf(y/s);
	else
		*azimuth = asinf(y/s);

	*newRadius = r / CCCamera::getZEye();				
}
NS_CC_END