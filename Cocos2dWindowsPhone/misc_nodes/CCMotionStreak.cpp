/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2009 Jason Booth
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
#include "CCMotionStreak.h"
#include "CCPointExtension.h"
#include "CCRibbon.h"
namespace cocos2d {

/*
 * Motion Streak manages a Ribbon based on it's motion in absolute space.
 * You construct it with a fadeTime, minimum segment size, texture path, texture
 * length and color. The fadeTime controls how long it takes each vertex in
 * the streak to fade out, the minimum segment size it how many pixels the
 * streak will move before adding a new ribbon segement, and the texture
 * length is the how many pixels the texture is stretched across. The texture
 * is vertically aligned along the streak segemnts. 
 */
//implementation CCMotionStreak
CCMotionStreak* CCMotionStreak::create(float fade, float minSeg, float stroke, ccColor3B color, const char* path)
{
    CCMotionStreak *pRet = new CCMotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, path))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCMotionStreak* CCMotionStreak::create(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture)
{
    CCMotionStreak *pRet = new CCMotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, texture))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool CCMotionStreak::initWithFade(float fade, float minSeg, float stroke, ccColor3B color, const char* path)
{
    CCAssert(path != NULL, "Invalid filename");

    CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(path);
    return initWithFade(fade, minSeg, stroke, color, texture);
}

bool CCMotionStreak::initWithFade(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture)
{
    CCNode::setPosition(CCPointZero);
    setAnchorPoint(CCPointZero);
    ignoreAnchorPointForPosition(true);
    m_bStartingPositionInitialized = false;

    m_tPositionR = CCPointZero;
    m_bFastMode = true;
    m_fMinSeg = (minSeg == -1.0f) ? stroke/5.0f : minSeg;
    m_fMinSeg *= m_fMinSeg;

    m_fStroke = stroke;
    m_fFadeDelta = 1.0f/fade;

    m_uMaxPoints = (int)(fade*60.0f)+2;
    m_uNuPoints = 0;
    m_pPointState = (float *)malloc(sizeof(float) * m_uMaxPoints);
    m_pPointVertexes = (CCPoint*)malloc(sizeof(CCPoint) * m_uMaxPoints);

    m_pVertices = (ccVertex2F*)malloc(sizeof(ccVertex2F) * m_uMaxPoints * 2);
    m_pTexCoords = (ccTex2F*)malloc(sizeof(ccTex2F) * m_uMaxPoints * 2);
    m_pColorPointer =  (CCubyte*)malloc(sizeof(CCubyte) * m_uMaxPoints * 2 * 4);

    // Set blend mode
    m_tBlendFunc.src = CC_SRC_ALPHA;
    m_tBlendFunc.dst = CC_ONE_MINUS_SRC_ALPHA;

    // shader program
    //setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));

    setTexture(texture);
    setColor(color);
    scheduleUpdate();

    return true;
}


void CCMotionStreak::update(ccTime delta)
{
	CCPoint location = this->convertToWorldSpace(CCPointZero);
	m_pRibbon->setPosition(ccp(-1*location.x, -1*location.y));
    float len = ccpLength(ccpSub(m_tLastLocation, location));
	if (len > m_fSegThreshold)
	{
		m_pRibbon->addPointAt(location, m_fWidth);
		m_tLastLocation = location;
	}
	m_pRibbon->update(delta);
}

//MotionStreak - CocosNodeTexture protocol

void CCMotionStreak::setTexture(CCTexture2D* texture)
{
	m_pRibbon->setTexture(texture);
}

CCTexture2D * CCMotionStreak::getTexture()
{
	return m_pRibbon->getTexture();
}

ccBlendFunc CCMotionStreak::getBlendFunc()
{
	return m_pRibbon->getBlendFunc();
}

void CCMotionStreak::setBlendFunc(ccBlendFunc blendFunc)
{
	m_pRibbon->setBlendFunc(blendFunc);
}

CCRibbon * CCMotionStreak::getRibbon()
{
	return m_pRibbon;
}
void CCMotionStreak::setColor(const ccColor3B& color)
{
    m_tColor = color;
}

const ccColor3B& CCMotionStreak::getColor(void)
{
    return m_tColor;
}
}// namespace cocos2d
