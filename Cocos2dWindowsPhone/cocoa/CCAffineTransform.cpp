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
#include "CCAffineTransform.h"
#include <algorithm>
#include <math.h>

using namespace std;


NS_CC_BEGIN

CCAffineTransform __CCAffineTransformMake(CGFloat a, CGFloat b, CGFloat c, CGFloat d, CGFloat tx, CGFloat ty)
{
  CCAffineTransform t;
  t.a = a; t.b = b; t.c = c; t.d = d; t.tx = tx; t.ty = ty;
  return t;
}

CCPoint __CCPointApplyAffineTransform(const CCPoint& point, const CCAffineTransform& t)
{
  CCPoint p;
  p.x = (CGFloat)((double)t.a * point.x + (double)t.c * point.y + t.tx);
  p.y = (CGFloat)((double)t.b * point.x + (double)t.d * point.y + t.ty);
  return p;
}

CCSize __CCSizeApplyAffineTransform(const CCSize& size, const CCAffineTransform& t)
{
  CCSize s;
  s.width = (CGFloat)((double)t.a * size.width + (double)t.c * size.height);
  s.height = (CGFloat)((double)t.b * size.width + (double)t.d * size.height);
  return s;
}


CCAffineTransform CCAffineTransformMakeIdentity()
{
	return __CCAffineTransformMake(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}

extern const CCAffineTransform CCAffineTransformIdentity = CCAffineTransformMakeIdentity();


CCRect CCRectApplyAffineTransform(const CCRect& rect, const CCAffineTransform& anAffineTransform)
{
    float top    = rect.getMinY();
    float left   = rect.getMinX();
    float right  = rect.getMaxX();
    float bottom = rect.getMaxY();
	
	CCPoint topLeft = CCPointApplyAffineTransform(CCPointMake(left, top), anAffineTransform);
    CCPoint topRight = CCPointApplyAffineTransform(CCPointMake(right, top), anAffineTransform);
    CCPoint bottomLeft = CCPointApplyAffineTransform(CCPointMake(left, bottom), anAffineTransform);
    CCPoint bottomRight = CCPointApplyAffineTransform(CCPointMake(right, bottom), anAffineTransform);

    CGFloat minX = min(min(topLeft.x, topRight.x), min(bottomLeft.x, bottomRight.x));
    CGFloat maxX = max(max(topLeft.x, topRight.x), max(bottomLeft.x, bottomRight.x));
    CGFloat minY = min(min(topLeft.y, topRight.y), min(bottomLeft.y, bottomRight.y));
    CGFloat maxY = max(max(topLeft.y, topRight.y), max(bottomLeft.y, bottomRight.y));
        
    return CCRectMake(minX, minY, (maxX - minX), (maxY - minY));
}

CCAffineTransform CCAffineTransformTranslate(const CCAffineTransform& t, float tx, float ty)
{
	return __CCAffineTransformMake(t.a, t.b, t.c, t.d, t.tx + t.a * tx + t.c * ty, t.ty + t.b * tx + t.d * ty);
}

CCAffineTransform CCAffineTransformScale(const CCAffineTransform& t, CGFloat sx, CGFloat sy)
{
	return __CCAffineTransformMake(t.a * sx, t.b * sx, t.c * sy, t.d * sy, t.tx, t.ty);
}

CCAffineTransform CCAffineTransformRotate(const CCAffineTransform& t, CGFloat anAngle)
{
    float fSin = sin(anAngle);
    float fCos = cos(anAngle);

	return __CCAffineTransformMake(	t.a * fCos + t.c * fSin,
									t.b * fCos + t.d * fSin,
									t.c * fCos - t.a * fSin,
									t.d * fCos - t.b * fSin,
									t.tx,
									t.ty);
}

/* Concatenate `t2' to `t1' and return the result:
     t' = t1 * t2 */
CCAffineTransform CCAffineTransformConcat(const CCAffineTransform& t1, const CCAffineTransform& t2)
{
	return __CCAffineTransformMake(	t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d, //a,b
									t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d, //c,d
									t1.tx * t2.a + t1.ty * t2.c + t2.tx,				  //tx
									t1.tx * t2.b + t1.ty * t2.d + t2.ty);				  //ty
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
bool CCAffineTransformEqualToTransform(const CCAffineTransform& t1, const CCAffineTransform& t2)
{
	return (t1.a == t2.a && t1.b == t2.b && t1.c == t2.c && t1.d == t2.d && t1.tx == t2.tx && t1.ty == t2.ty);
}

CCAffineTransform CCAffineTransformInvert(const CCAffineTransform& t)
{
    float determinant = 1 / (t.a * t.d - t.b * t.c);

    return __CCAffineTransformMake(determinant * t.d, -determinant * t.b, -determinant * t.c, determinant * t.a,
							determinant * (t.c * t.ty - t.d * t.tx), determinant * (t.b * t.tx - t.a * t.ty) );
}
NS_CC_END
