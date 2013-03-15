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

#include "CCGeometry.h"
// implementation of CCPoint
NS_CC_BEGIN

CCPoint::CCPoint(void)
{
	x = 0;
	y = 0;
}

CCPoint::CCPoint(float x, float y)
{
	this->x = x;
	this->y = y;
}

bool CCPoint::CCPointEqualToPoint(const CCPoint& point1, const CCPoint& point2)
{
	return ((point1.x == point2.x) && (point1.y == point2.y));
}

// implementation of CCSize

CCSize::CCSize(void)
{
	width = 0;
	height = 0;
}

CCSize::CCSize(float width, float height)
{
	this->width = width;
	this->height = height;
}

bool CCSize::CCSizeEqualToSize(const CCSize& size1, const CCSize& size2)
{
	return ((size1.width == size2.width) && (size1.height == size2.height));
}

// implementation of CCRect

CCRect::CCRect(void)
{
	origin.x = 0;
	origin.y = 0;

	size.width = 0;
	size.height = 0;
}

CCRect::CCRect(float x, float y, float width, float height)
{
	// Only support that, the width and height > 0
	CC_ASSERT(width >= 0 && height >= 0);

	origin.x = x;
	origin.y = y;

	size.width = width;
	size.height = height;
}
bool CCPoint::equals(const CCPoint& target) const
{
    return ((x == target.x) && (y == target.y));
}
bool CCSize::equals(const CCSize& target) const
{
    return ((width == target.width) && (height == target.height));
}

bool CCRect::equals(const CCRect& rect) const
{
    return (origin.equals(rect.origin) && 
            size.equals(rect.size));
}
bool CCRect::CCRectEqualToRect(const CCRect& rect1, const CCRect& rect2)
{
	return (CCPoint::CCPointEqualToPoint(rect1.origin, rect2.origin)
		&& CCSize::CCSizeEqualToSize(rect1.size, rect2.size));
}

CGFloat CCRect::CCRectGetMaxX(const CCRect& rect)
{
	return rect.origin.x + rect.size.width;
}

CGFloat CCRect::CCRectGetMidX(const CCRect& rect)
{
	return (float)(rect.origin.x + rect.size.width / 2.0);
}

CGFloat CCRect::CCRectGetMinX(const CCRect& rect)
{
	return rect.origin.x;
}

CGFloat CCRect::CCRectGetMaxY(const CCRect& rect)
{
	return rect.origin.y + rect.size.height;
}

CGFloat CCRect::CCRectGetMidY(const CCRect& rect)
{
	return (float)(rect.origin.y + rect.size.height / 2.0);
}

CGFloat CCRect::CCRectGetMinY(const CCRect& rect)
{
	return rect.origin.y;
}

bool CCRect::CCRectContainsPoint(const CCRect& rect, const CCPoint& point)
{
    bool bRet = false;

	if (point.x >= CCRectGetMinX(rect) && point.x <= CCRectGetMaxX(rect)
		&& point.y >= CCRectGetMinY(rect) && point.y <= CCRectGetMaxY(rect))
	{
		bRet = true;
	}

	return bRet;
}

bool CCRect::CCRectIntersectsRect(const CCRect& rectA, const CCRect& rectB)
{
	return !(CCRectGetMaxX(rectA) < CCRectGetMinX(rectB)||
			CCRectGetMaxX(rectB) < CCRectGetMinX(rectA)||
			CCRectGetMaxY(rectA) < CCRectGetMinY(rectB)||
			CCRectGetMaxY(rectB) < CCRectGetMinY(rectA));
}

NS_CC_END
