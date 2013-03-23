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

#include "CCAtlasNode.h"
#include "CCTextureAtlas.h"
#include "CCDirector.h"

NS_CC_BEGIN

// implementation CCAtlasNode

// CCAtlasNode - Creation & Init

CCAtlasNode::CCAtlasNode()
: m_uItemsPerRow(0)
, m_uItemsPerColumn(0)
, m_uItemWidth(0)
, m_uItemHeight(0)
, m_pTextureAtlas(NULL)
, m_bIsOpacityModifyRGB(false)
, m_cOpacity(0)
, m_uQuadsToDraw(0)
{
}

CCAtlasNode::~CCAtlasNode()
{
	CC_SAFE_RELEASE(m_pTextureAtlas);
}

CCAtlasNode * CCAtlasNode::create(const char *tile, unsigned int tileWidth, unsigned int tileHeight, 
											 unsigned int itemsToRender)
{
	CCAtlasNode * pRet = new CCAtlasNode();
	if (pRet->initWithTileFile(tile, tileWidth, tileHeight, itemsToRender))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool CCAtlasNode::initWithTileFile(const char *tile, unsigned int tileWidth, unsigned int tileHeight, 
								   unsigned int itemsToRender)
{
	CCAssert(tile != NULL, "title should not be null");
	m_uItemWidth  = (int) (tileWidth * CC_CONTENT_SCALE_FACTOR());
	m_uItemHeight = (int) (tileHeight * CC_CONTENT_SCALE_FACTOR());

	m_cOpacity = 255;
	m_tColor = m_tColorUnmodified = ccWHITE;
	m_bIsOpacityModifyRGB = true;

	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;

	// double retain to avoid the autorelease pool
	// also, using: self.textureAtlas supports re-initialization without leaking
	this->m_pTextureAtlas = new CCTextureAtlas();
	m_pTextureAtlas->initWithFile(tile, itemsToRender);
    
	if (! m_pTextureAtlas)
	{
		CCLOG("cocos2d: Could not initialize CCAtlasNode. Invalid Texture.");
		delete this;
		return false;
	}

	this->updateBlendFunc();
	this->updateOpacityModifyRGB();

	this->calculateMaxItems();

	m_uQuadsToDraw = itemsToRender;

	return true;
}


// CCAtlasNode - Atlas generation

void CCAtlasNode::calculateMaxItems()
{
	const CCSize& s = m_pTextureAtlas->getTexture()->getContentSizeInPixels();
	m_uItemsPerColumn = (int)(s.height / m_uItemHeight);
	m_uItemsPerRow = (int)(s.width / m_uItemWidth);
}

void CCAtlasNode::updateAtlasValues()
{
	CCAssert(false, "CCAtlasNode:Abstract updateAtlasValue not overriden");
	//[NSException raise:@"CCAtlasNode:Abstract" format:@"updateAtlasValue not overriden"];
}

// CCAtlasNode - draw
void CCAtlasNode::draw()
{
	CCNode::draw();

	m_pTextureAtlas->SetColor(m_tColor.r,m_tColor.g,m_tColor.b,m_cOpacity);
	bool newBlend = m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST;
	if(newBlend) 
	{
		CCD3DCLASS->D3DBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
	}

	m_pTextureAtlas->drawNumberOfQuads(m_uQuadsToDraw, 0);

	if( newBlend )
	{
		CCD3DCLASS->D3DBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	}

}

// CCAtlasNode - RGBA protocol

const ccColor3B& CCAtlasNode:: getColor()
{
	if(m_bIsOpacityModifyRGB)
	{
		return m_tColorUnmodified;
	}
	return m_tColor;
}

void CCAtlasNode::setColor(const ccColor3B& color3)
{
	m_tColor = m_tColorUnmodified = color3;

	if( m_bIsOpacityModifyRGB )
	{
		m_tColor.r = color3.r * m_cOpacity/255;
		m_tColor.g = color3.g * m_cOpacity/255;
		m_tColor.b = color3.b * m_cOpacity/255;
	}	
}

CCubyte CCAtlasNode::getOpacity()
{
	return m_cOpacity;
}

void CCAtlasNode::setOpacity(CCubyte opacity)
{
	m_cOpacity = opacity;

	// special opacity for premultiplied textures
	if( m_bIsOpacityModifyRGB )
		this->setColor(m_tColorUnmodified);
}


bool CCAtlasNode::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}

void CCAtlasNode::setOpacityModifyRGB(bool bValue)
{
    ccColor3B oldColor = this->getColor();
    m_bIsOpacityModifyRGB = bValue;
    this->setColor(oldColor);
}


void CCAtlasNode::updateOpacityModifyRGB()
{
	m_bIsOpacityModifyRGB = m_pTextureAtlas->getTexture()->getHasPremultipliedAlpha();
}

// CCAtlasNode - CocosNodeTexture protocol

ccBlendFunc CCAtlasNode::getBlendFunc()
{
	return m_tBlendFunc;
}

void CCAtlasNode::setBlendFunc(ccBlendFunc blendFunc)
{
	m_tBlendFunc = blendFunc;
}

void CCAtlasNode::updateBlendFunc()
{
	if( ! m_pTextureAtlas->getTexture()->getHasPremultipliedAlpha() ) {
		m_tBlendFunc.src = CC_SRC_ALPHA;
		m_tBlendFunc.dst = CC_ONE_MINUS_SRC_ALPHA;
	}
}

void CCAtlasNode::setTexture(CCTexture2D *texture)
{
	m_pTextureAtlas->setTexture(texture);
	this->updateBlendFunc();
	this->updateOpacityModifyRGB();
}

CCTexture2D * CCAtlasNode::getTexture()
{
	return m_pTextureAtlas->getTexture();
}

void CCAtlasNode::setTextureAtlas(CCTextureAtlas* var)
{
	CC_SAFE_RETAIN(var);
	CC_SAFE_RELEASE(m_pTextureAtlas);
	m_pTextureAtlas = var;
}
CCTextureAtlas * CCAtlasNode::getTextureAtlas()
{
	return m_pTextureAtlas;
}

unsigned int CCAtlasNode::getQuadsToDraw()
{
	return m_uQuadsToDraw;
}

void CCAtlasNode::setQuadsToDraw(unsigned int uQuadsToDraw)
{
	m_uQuadsToDraw = uQuadsToDraw;
}

NS_CC_END
