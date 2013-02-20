/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2010      Lam Pham
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
#include "CCProgressTimer.h"

#include "ccMacros.h"
#include "CCTextureCache.h"
#include "CCPointExtension.h"
#include "CCDirector.h"
#include <float.h>
#include "CCFileUtils.h"
#include "DirectXHelper.h"
#include "BasicLoader.h"

using namespace std;
using namespace DirectX;

namespace cocos2d {

#define kProgressTextureCoordsCount 4
const char kProgressTextureCoords = 0x1e;

CCDXProgressTimer CCProgressTimer::mDXProgressTimer;

CCProgressTimer* CCProgressTimer::progressWithFile(const char *pszFileName)
{
	CCProgressTimer *pProgressTimer = new CCProgressTimer();
	if (pProgressTimer->initWithFile(pszFileName))
	{
		pProgressTimer->autorelease();
	}
	else
	{
        delete pProgressTimer;
		pProgressTimer = NULL;
	}		

	return pProgressTimer;
}

bool CCProgressTimer::initWithFile(const char *pszFileName)
{
	return this->initWithTexture(CCTextureCache::sharedTextureCache()->addImage(pszFileName));
}

CCProgressTimer* CCProgressTimer::progressWithTexture(CCTexture2D *pTexture)
{
	CCProgressTimer *pProgressTimer = new CCProgressTimer();
	if (pProgressTimer->initWithTexture(pTexture))
	{
		pProgressTimer->autorelease();
	}
	else
	{
        delete pProgressTimer;
		pProgressTimer = NULL;
	}		

	return pProgressTimer;
}

bool CCProgressTimer::initWithTexture(CCTexture2D *pTexture)
{
	m_pSprite = CCSprite::spriteWithTexture(pTexture);
	CC_SAFE_RETAIN(m_pSprite);
	m_fPercentage = 0.f;
	m_pVertexData = NULL;
    m_nVertexDataCount = 0;
	setAnchorPoint(ccp(0.5f, 0.5f));
	setContentSize(m_pSprite->getContentSize());
    m_eType = kCCProgressTimerTypeRadialCCW;
	return true;
}

CCProgressTimer::~CCProgressTimer(void)
{
	CC_SAFE_DELETE_ARRAY(m_pVertexData);
	CC_SAFE_RELEASE(m_pSprite);
}

void CCProgressTimer::setPercentage(float fPercentage)
{
	if (m_fPercentage != fPercentage)
	{
		m_fPercentage = clampf(fPercentage, 0, 100);
		updateProgress();
	}
}

void CCProgressTimer::setSprite(CCSprite *pSprite)
{
	if (m_pSprite != pSprite)
	{
		CC_SAFE_RETAIN(pSprite);
		CC_SAFE_RELEASE(m_pSprite);
		m_pSprite = pSprite;
        setContentSize(m_pSprite->getContentSize());

		//	Everytime we set a new sprite, we free the current vertex data
		if (m_pVertexData)
		{
			delete[] m_pVertexData;
			m_pVertexData = NULL;
			m_nVertexDataCount = 0;
		}
	}		
}

void CCProgressTimer::setType(CCProgressTimerType type)
{
	if (type != m_eType)
	{
		//	release all previous information
		if (m_pVertexData)
		{
			delete[] m_pVertexData;
			m_pVertexData = NULL;
			m_nVertexDataCount = 0;
		}

		m_eType = type;
	}
}

// Interval

///
//	@returns the vertex position from the texture coordinate
///
ccVertex2F CCProgressTimer::vertexFromTexCoord(const CCPoint& texCoord)
{
    CCPoint tmp;
    ccVertex2F ret;

    CCTexture2D *pTexture = m_pSprite->getTexture();
    if (pTexture)
    {
        float fXMax = MAX(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
        float fXMin = MIN(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
        float fYMax = MAX(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
        float fYMin = MIN(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
        CCPoint tMax = ccp(fXMax, fYMax);
        CCPoint tMin = ccp(fXMin, fYMin);

        CCSize texSize = CCSizeMake(m_pSprite->getQuad().br.vertices.x - m_pSprite->getQuad().bl.vertices.x,
                                    m_pSprite->getQuad().tl.vertices.y - m_pSprite->getQuad().bl.vertices.y);
        tmp = ccp(texSize.width * (texCoord.x - tMin.x) / (tMax.x - tMin.x),
                    texSize.height * (1 - (texCoord.y - tMin.y) / (tMax.y - tMin.y)));
	}
	else
	{
	    tmp = CCPointZero;
	}

    ret.x = tmp.x;
    ret.y = tmp.y;
    return ret;
}

void CCProgressTimer::updateColor(void)
{
	CCubyte op = m_pSprite->getOpacity();
	ccColor3B c3b = m_pSprite->getColor();

	ccColor4B color = {c3b.r, c3b.g, c3b.b, op};
	if (m_pSprite->getTexture()->getHasPremultipliedAlpha())
	{
		color.r *= op / 255;
		color.g *= op / 255;
		color.b *= op / 255;
	}

	if (m_pVertexData)
	{
		for (int i = 0; i < m_nVertexDataCount; ++i)
		{
			m_pVertexData[i].colors = color;
		}			
	}
}

void CCProgressTimer::updateProgress(void)
{
	switch (m_eType)
	{
	case kCCProgressTimerTypeRadialCW:
	case kCCProgressTimerTypeRadialCCW:
        updateRadial();
		break;
	case kCCProgressTimerTypeHorizontalBarLR:
	case kCCProgressTimerTypeHorizontalBarRL:
	case kCCProgressTimerTypeVerticalBarBT:
	case kCCProgressTimerTypeVerticalBarTB:
		updateBar();
		break;
	default:
		break;
	}
}

///
//	Update does the work of mapping the texture onto the triangles
//	It now doesn't occur the cost of free/alloc data every update cycle.
//	It also only changes the percentage point but no other points if they have not
//	been modified.
//	
//	It now deals with flipped texture. If you run into this problem, just use the
//	sprite property and enable the methods flipX, flipY.
///
void CCProgressTimer::updateRadial(void)
{
	//	Texture Max is the actual max coordinates to deal with non-power of 2 textures
    float fXMax = MAX(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
    float fXMin = MIN(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
    float fYMax = MAX(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
    float fYMin = MIN(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
    CCPoint tMax = ccp(fXMax, fYMax);
    CCPoint tMin = ccp(fXMin, fYMin);

	//	Grab the midpoint
	CCPoint midpoint = ccpAdd(tMin, ccpCompMult(m_tAnchorPoint, ccpSub(tMax, tMin)));

	float alpha = m_fPercentage / 100.f;

	//	Otherwise we can get the angle from the alpha
	float angle = 2.f * ((float)M_PI) * (m_eType == kCCProgressTimerTypeRadialCW ? alpha : 1.f - alpha);

	//	We find the vector to do a hit detection based on the percentage
	//	We know the first vector is the one @ 12 o'clock (top,mid) so we rotate 
	//	from that by the progress angle around the midpoint pivot
	CCPoint topMid = ccp(midpoint.x, tMin.y);
	CCPoint percentagePt = ccpRotateByAngle(topMid, midpoint, angle);

	int index = 0;
	CCPoint hit = CCPointZero;

	if (alpha == 0.f)
	{
		//	More efficient since we don't always need to check intersection
		//	If the alpha is zero then the hit point is top mid and the index is 0.
		hit = topMid;
		index = 0;
	} else
	if (alpha == 1.f)
	{
		//	More efficient since we don't always need to check intersection
		//	If the alpha is one then the hit point is top mid and the index is 4.
		hit = topMid;
		index = 4;
	}
	else
	{
		//	We run a for loop checking the edges of the texture to find the
		//	intersection point
		//	We loop through five points since the top is split in half
		
		float min_t = FLT_MAX;

		for (int i = 0; i <= kProgressTextureCoordsCount; ++i)
		{
			int pIndex = (i + (kProgressTextureCoordsCount - 1)) % kProgressTextureCoordsCount;

			CCPoint edgePtA = ccpAdd(tMin, ccpCompMult(boundaryTexCoord(i % kProgressTextureCoordsCount), ccpSub(tMax, tMin)));
			CCPoint edgePtB = ccpAdd(tMin, ccpCompMult(boundaryTexCoord(pIndex), ccpSub(tMax, tMin)));

			//	Remember that the top edge is split in half for the 12 o'clock position
			//	Let's deal with that here by finding the correct endpoints
			if (i == 0)
			{
				edgePtB = ccpLerp(edgePtA, edgePtB, 0.5f);
			} else
			if (i == 4)
			{
				edgePtA = ccpLerp(edgePtA, edgePtB, 0.5f);
			}

			//	s and t are returned by ccpLineIntersect
			float s = 0;
			float t = 0;
			if (ccpLineIntersect(edgePtA, edgePtB, midpoint, percentagePt, &s, &t))
			{
				//	Since our hit test is on rays we have to deal with the top edge
				//	being in split in half so we have to test as a segment
				if (i == 0 || i == 4)
				{
					//	s represents the point between edgePtA--edgePtB
					if (!(0.f <= s && s <= 1.f))
					{
						continue;
					}
				}

				//	As long as our t isn't negative we are at least finding a 
				//	correct hitpoint from midpoint to percentagePt.
				if (t >= 0.f)
				{
					//	Because the percentage line and all the texture edges are
					//	rays we should only account for the shortest intersection
					if (t < min_t)
					{
						min_t = t;
						index = i;
					}
				}
			}
		
		}

        //	Now that we have the minimum magnitude we can use that to find our intersection
		hit = ccpAdd(midpoint, ccpMult(ccpSub(percentagePt, midpoint),min_t));
	}

    //	The size of the vertex data is the index from the hitpoint
	//	the 3 is for the midpoint, 12 o'clock point and hitpoint position.

	bool sameIndexCount = true;
	if (m_nVertexDataCount != index + 3)
	{
		sameIndexCount = false;
		if (m_pVertexData)
		{
			delete[] m_pVertexData;
			m_pVertexData = NULL;
			m_nVertexDataCount = 0;
		}
	}

	if (! m_pVertexData)
	{
		m_nVertexDataCount = index + 3;
		m_pVertexData = new ccV2F_C4B_T2F[m_nVertexDataCount];
		CCAssert(m_pVertexData, "");

		updateColor();
		//initVertexBuffer();
	}

	if (! sameIndexCount)
	{
		//	First we populate the array with the midpoint, then all 
		//	vertices/texcoords/colors of the 12 'o clock start and edges and the hitpoint
		m_pVertexData[0].texCoords = tex2(midpoint.x, midpoint.y);
		m_pVertexData[0].vertices = vertexFromTexCoord(midpoint);

		m_pVertexData[1].texCoords = tex2(midpoint.x, tMin.y);
		m_pVertexData[1].vertices = vertexFromTexCoord(ccp(midpoint.x, tMin.y));

		for (int i = 0; i < index; ++i)
		{
			CCPoint texCoords = ccpAdd(tMin, ccpCompMult(boundaryTexCoord(i), ccpSub(tMax, tMin)));

			m_pVertexData[i+2].texCoords = tex2(texCoords.x, texCoords.y);
			m_pVertexData[i+2].vertices = vertexFromTexCoord(texCoords);
		}

		//	Flip the texture coordinates if set
		if (m_pSprite->isFlipX() || m_pSprite->isFlipY())
		{
			for (int i = 0; i < m_nVertexDataCount - 1; ++i)
			{
				if (m_pSprite->isFlipX())
				{
					m_pVertexData[i].texCoords.u = tMin.x + tMax.x - m_pVertexData[i].texCoords.u;
				}

				if (m_pSprite->isFlipY())
				{
					m_pVertexData[i].texCoords.v = tMin.y + tMax.y - m_pVertexData[i].texCoords.v;
				}
			}
		}
	}

	//	hitpoint will go last
	m_pVertexData[m_nVertexDataCount - 1].texCoords = tex2(hit.x, hit.y);
	m_pVertexData[m_nVertexDataCount - 1].vertices = vertexFromTexCoord(hit);

	if (m_pSprite->isFlipX() || m_pSprite->isFlipY())
	{
		if (m_pSprite->isFlipX())
		{
			m_pVertexData[m_nVertexDataCount - 1].texCoords.u = tMin.x + tMax.x - m_pVertexData[m_nVertexDataCount - 1].texCoords.u;
		}

        if (m_pSprite->isFlipY())
		{
			m_pVertexData[m_nVertexDataCount - 1].texCoords.v = tMin.y + tMax.y - m_pVertexData[m_nVertexDataCount - 1].texCoords.v;
		}
	}
}

///
//	Update does the work of mapping the texture onto the triangles for the bar
//	It now doesn't occur the cost of free/alloc data every update cycle.
//	It also only changes the percentage point but no other points if they have not
//	been modified.
//	
//	It now deals with flipped texture. If you run into this problem, just use the
//	sprite property and enable the methods flipX, flipY.
///
void CCProgressTimer::updateBar(void)
{
	float alpha = m_fPercentage / 100.f;

    float fXMax = MAX(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
    float fXMin = MIN(m_pSprite->getQuad().br.texCoords.u, m_pSprite->getQuad().bl.texCoords.u);
    float fYMax = MAX(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
    float fYMin = MIN(m_pSprite->getQuad().tl.texCoords.v, m_pSprite->getQuad().bl.texCoords.v);
    CCPoint tMax = ccp(fXMax, fYMax);
    CCPoint tMin = ccp(fXMin, fYMin);

	unsigned char vIndexes[2] = {0, 0};
	unsigned char index = 0;

	//	We know vertex data is always equal to the 4 corners
	//	If we don't have vertex data then we create it here and populate
	//	the side of the bar vertices that won't ever change.
	if (! m_pVertexData)
	{
		m_nVertexDataCount = kProgressTextureCoordsCount;
		m_pVertexData = new ccV2F_C4B_T2F[m_nVertexDataCount];
		CCAssert(m_pVertexData, "");
		//initVertexBuffer();

		if (m_eType == kCCProgressTimerTypeHorizontalBarLR)
		{
			m_pVertexData[vIndexes[0] = 0].texCoords = tex2(tMin.x, tMin.y);
			m_pVertexData[vIndexes[1] = 1].texCoords = tex2(tMin.x, tMax.y);
		} else
		if (m_eType == kCCProgressTimerTypeHorizontalBarRL)
		{
			m_pVertexData[vIndexes[0] = 2].texCoords = tex2(tMax.x, tMax.y);
			m_pVertexData[vIndexes[1] = 3].texCoords = tex2(tMax.x, tMin.y);
		} else
		if (m_eType == kCCProgressTimerTypeVerticalBarBT)
		{
			m_pVertexData[vIndexes[0] = 1].texCoords = tex2(tMin.x, tMax.y);
			m_pVertexData[vIndexes[1] = 3].texCoords = tex2(tMax.x, tMax.y);
		} else
		if (m_eType == kCCProgressTimerTypeVerticalBarTB)
		{
			m_pVertexData[vIndexes[0] = 0].texCoords = tex2(tMin.x, tMin.y);
			m_pVertexData[vIndexes[1] = 2].texCoords = tex2(tMax.x, tMin.y);
		}

		index = vIndexes[0];
		m_pVertexData[index].vertices = vertexFromTexCoord(ccp(m_pVertexData[index].texCoords.u,
			                                                   m_pVertexData[index].texCoords.v));

		index = vIndexes[1];
		m_pVertexData[index].vertices = vertexFromTexCoord(ccp(m_pVertexData[index].texCoords.u,
			                                                   m_pVertexData[index].texCoords.v));

		if (m_pSprite->isFlipY() || m_pSprite->isFlipX())
		{
			if (m_pSprite->isFlipX())
			{
				index = vIndexes[0];
				m_pVertexData[index].texCoords.u = tMin.x + tMax.x - m_pVertexData[index].texCoords.u;
				index = vIndexes[1];
				m_pVertexData[index].texCoords.u = tMin.x + tMax.x - m_pVertexData[index].texCoords.u;
			}

			if (m_pSprite->isFlipY())
			{
				index = vIndexes[0];
				m_pVertexData[index].texCoords.v = tMin.y + tMax.y - m_pVertexData[index].texCoords.v;
				index = vIndexes[1];
				m_pVertexData[index].texCoords.v = tMin.y + tMax.y - m_pVertexData[index].texCoords.v;
			}
		}

		updateColor();
	}

	if(m_eType == kCCProgressTimerTypeHorizontalBarLR)
	{
		m_pVertexData[vIndexes[0] = 3].texCoords = tex2(tMin.x + (tMax.x - tMin.x) *alpha, tMax.y);
		m_pVertexData[vIndexes[1] = 2].texCoords = tex2(tMin.x + (tMax.x - tMin.x) *alpha, tMin.y);
	} else 
	if (m_eType == kCCProgressTimerTypeHorizontalBarRL) 
	{
		m_pVertexData[vIndexes[0] = 1].texCoords = tex2(tMin.x + (tMax.x - tMin.x) * (1.f - alpha), tMin.y);
		m_pVertexData[vIndexes[1] = 0].texCoords = tex2(tMin.x + (tMax.x - tMin.x) * (1.f - alpha), tMax.y);
	} else 
	if (m_eType == kCCProgressTimerTypeVerticalBarBT) 
	{
		m_pVertexData[vIndexes[0] = 0].texCoords = tex2(tMin.x, tMin.y + (tMax.y - tMin.y) * (1.f - alpha));
		m_pVertexData[vIndexes[1] = 2].texCoords = tex2(tMax.x, tMin.y + (tMax.y - tMin.y) * (1.f - alpha));
	} else 
	if (m_eType == kCCProgressTimerTypeVerticalBarTB) 
	{
		m_pVertexData[vIndexes[0] = 1].texCoords = tex2(tMin.x, tMin.y + (tMax.y - tMin.y) * alpha);
		m_pVertexData[vIndexes[1] = 3].texCoords = tex2(tMax.x, tMin.y + (tMax.y - tMin.y) * alpha);
	}

	index = vIndexes[0];
	m_pVertexData[index].vertices = vertexFromTexCoord(ccp(m_pVertexData[index].texCoords.u,
		                                                   m_pVertexData[index].texCoords.v));
	index = vIndexes[1];
	m_pVertexData[index].vertices = vertexFromTexCoord(ccp(m_pVertexData[index].texCoords.u,
		                                                   m_pVertexData[index].texCoords.v));

	if (m_pSprite->isFlipY() || m_pSprite->isFlipX()) 
	{
		if (m_pSprite->isFlipX()) 
		{
			index = vIndexes[0];
			m_pVertexData[index].texCoords.u = tMin.x + tMax.x - m_pVertexData[index].texCoords.u;
			index = vIndexes[1];
			m_pVertexData[index].texCoords.u = tMin.x + tMax.x - m_pVertexData[index].texCoords.u;
		}

		if (m_pSprite->isFlipY())
		{
			index = vIndexes[0];
			m_pVertexData[index].texCoords.v = tMin.y + tMax.y - m_pVertexData[index].texCoords.v;
			index = vIndexes[1];
			m_pVertexData[index].texCoords.v = tMin.y + tMax.y - m_pVertexData[index].texCoords.v;
		}
	}
}

CCPoint CCProgressTimer::boundaryTexCoord(char index)
{
	if (index < kProgressTextureCoordsCount) 
	{
		switch (m_eType) 
		{
		case kCCProgressTimerTypeRadialCW:
			return ccp( (float)((kProgressTextureCoords>>((index<<1)+1))&1), (float)((kProgressTextureCoords>>(index<<1))&1) );
		case kCCProgressTimerTypeRadialCCW:
			return ccp( (float)((kProgressTextureCoords>>(7-(index<<1)))&1), (float)((kProgressTextureCoords>>(7-((index<<1)+1)))&1));
		default:
			break;
		}
	}

	return CCPointZero;
}

void CCProgressTimer::draw(void)
{
	CCNode::draw();

	if(! m_pVertexData) 
	{
		return;
	}

	if(! m_pSprite) 
	{
		return;
	}

	ccBlendFunc bf = m_pSprite->getBlendFunc();
    bool newBlend = (bf.src != CC_BLEND_SRC || bf.dst != CC_BLEND_DST) ? true : false;
	if (newBlend) 
	{
		CCD3DCLASS->D3DBlendFunc(bf.src, bf.dst);
	}
	
	///	========================================================================
	//	Replaced [texture_ drawAtPoint:CCPointZero] with my own vertexData
	//	Everything above me and below me is copied from CCTextureNode's draw
	/*=
	glBindTexture(CC_TEXTURE_2D, m_pSprite->getTexture()->getName());
	glVertexPointer(2, CC_FLOAT, sizeof(ccV2F_C4B_T2F), &m_pVertexData[0].vertices);
	glTexCoordPointer(2, CC_FLOAT, sizeof(ccV2F_C4B_T2F), &m_pVertexData[0].texCoords);
	glColorPointer(4, CC_UNSIGNED_BYTE, sizeof(ccV2F_C4B_T2F), &m_pVertexData[0].colors);
	
	if(m_eType == kCCProgressTimerTypeRadialCCW || m_eType == kCCProgressTimerTypeRadialCW)
	{
		glDrawArrays(CC_TRIANGLE_FAN, 0, m_nVertexDataCount);
	} else 
	if (	m_eType == kCCProgressTimerTypeHorizontalBarLR ||
			m_eType == kCCProgressTimerTypeHorizontalBarRL ||
			m_eType == kCCProgressTimerTypeVerticalBarBT ||
			m_eType == kCCProgressTimerTypeVerticalBarTB) 
	{
		glDrawArrays(CC_TRIANGLE_STRIP, 0, m_nVertexDataCount);
	}=*/
	//glDrawElements(CC_TRIANGLES, indicesCount_, CC_UNSIGNED_BYTE, indices_);
	///	========================================================================
	mDXProgressTimer.Render(m_pVertexData,m_nVertexDataCount,m_eType,m_pSprite);
	if( newBlend )
    {
		CCD3DCLASS->D3DBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
    }
}


CCDXProgressTimer::CCDXProgressTimer()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_vertexBuffer = 0;
	mIsInit = FALSE;
}
CCDXProgressTimer::~CCDXProgressTimer()
{
	FreeBuffer();
}
void CCDXProgressTimer::FreeBuffer()
{
	CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_matrixBuffer);
	CC_SAFE_RELEASE_NULL_DX(m_layout);
	CC_SAFE_RELEASE_NULL_DX(m_pixelShader);
	CC_SAFE_RELEASE_NULL_DX(m_vertexShader);
}

void CCDXProgressTimer::setIsInit(bool isInit)
{
	mIsInit = isInit;
}

void CCDXProgressTimer::RenderVertexBuffer()
{
	unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;
	CCID3D11DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	CCID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void CCDXProgressTimer::initVertexBuffer(ccV2F_C4B_T2F *vertexData,int& vertexDataCount,CCProgressTimerType eType)
{
	CC_SAFE_RELEASE_NULL_DX(m_vertexBuffer);

	int tmpVertexDataCount = vertexDataCount;
	if(eType == kCCProgressTimerTypeRadialCCW || eType == kCCProgressTimerTypeRadialCW)
	{
		switch(vertexDataCount)
		{
		case 3:
			m_nVertexDataCount2=3;
			break;
		case 4:
			m_nVertexDataCount2=6;
			break;
		case 5:
			m_nVertexDataCount2=9;
			break;
		case 6:
			m_nVertexDataCount2=12;
			break;
		case 7:
			m_nVertexDataCount2=15;
			break;
		}
		tmpVertexDataCount = m_nVertexDataCount2;
	}

	
	VertexType* vertices = new VertexType[tmpVertexDataCount];
	if (!vertices)
	{
		return;
	}
	memset(vertices, 0, (sizeof(VertexType) * tmpVertexDataCount));
	if(eType == kCCProgressTimerTypeRadialCCW || eType == kCCProgressTimerTypeRadialCW)
	{
		switch(vertexDataCount)
		{
		case 3:
			{
				//int a[]={0,1,2};
				int a[]={0,1,2};
				for ( int j=0; j<tmpVertexDataCount; j++ )
				{
					vertices[j].position = XMFLOAT3(vertexData[a[j]].vertices.x, vertexData[a[j]].vertices.y, 0.f);
					vertices[j].texture = XMFLOAT2(vertexData[a[j]].texCoords.u, vertexData[a[j]].texCoords.v);
					vertices[j].color = XMFLOAT4(vertexData[a[j]].colors.r/255.f, vertexData[a[j]].colors.g/255.f, vertexData[a[j]].colors.b/255.f, vertexData[a[j]].colors.a/255.f);
				}
			}
			break;
		case 4:
			{
				//int a[]={0,1,2,0,3,1};
				int a[]={0,1,2,0,2,3};
				for ( int j=0; j<tmpVertexDataCount; j++ )
				{
					vertices[j].position = XMFLOAT3(vertexData[a[j]].vertices.x, vertexData[a[j]].vertices.y, 0.f);
					vertices[j].texture = XMFLOAT2(vertexData[a[j]].texCoords.u, vertexData[a[j]].texCoords.v);
					vertices[j].color = XMFLOAT4(vertexData[a[j]].colors.r/255.f, vertexData[a[j]].colors.g/255.f, vertexData[a[j]].colors.b/255.f, vertexData[a[j]].colors.a/255.f);
				}
			}
			break;
		case 5:
			{
				//int a[]={0,1,2,0,3,4,0,1};
				int a[]={0,1,2,0,2,3,0,3,4};
				for ( int j=0; j<tmpVertexDataCount; j++ )
				{
					vertices[j].position = XMFLOAT3(vertexData[a[j]].vertices.x, vertexData[a[j]].vertices.y, 0.f);
					vertices[j].texture = XMFLOAT2(vertexData[a[j]].texCoords.u, vertexData[a[j]].texCoords.v);
					vertices[j].color = XMFLOAT4(vertexData[a[j]].colors.r/255.f, vertexData[a[j]].colors.g/255.f, vertexData[a[j]].colors.b/255.f, vertexData[a[j]].colors.a/255.f);
				}
			}			
			break;
		case 6:
			{
				//int a[]={0,1,2,0,3,4,0,5,1};
				int a[]={0,1,2,0,2,3,0,3,4,0,4,5};
				for ( int j=0; j<tmpVertexDataCount; j++ )
				{
					vertices[j].position = XMFLOAT3(vertexData[a[j]].vertices.x, vertexData[a[j]].vertices.y, 0.f);
					vertices[j].texture = XMFLOAT2(vertexData[a[j]].texCoords.u, vertexData[a[j]].texCoords.v);
					vertices[j].color = XMFLOAT4(vertexData[a[j]].colors.r/255.f, vertexData[a[j]].colors.g/255.f, vertexData[a[j]].colors.b/255.f, vertexData[a[j]].colors.a/255.f);
				}
			}
			break;
		case 7:
			{
				//int a[]={0,1,2,0,3,4,0,5,6,0,1};
				int a[]={0,1,2,0,2,3,0,3,4,0,4,5,0,5,6};
				for ( int j=0; j<tmpVertexDataCount; j++ )
				{
					vertices[j].position = XMFLOAT3(vertexData[a[j]].vertices.x, vertexData[a[j]].vertices.y, 0.f);
					vertices[j].texture = XMFLOAT2(vertexData[a[j]].texCoords.u, vertexData[a[j]].texCoords.v);
					vertices[j].color = XMFLOAT4(vertexData[a[j]].colors.r/255.f, vertexData[a[j]].colors.g/255.f, vertexData[a[j]].colors.b/255.f, vertexData[a[j]].colors.a/255.f);
				}
			}
			break;
		}
	}
	else
	{
		for(int i=0; i<vertexDataCount; i++)
		{
			vertices[i].position = XMFLOAT3(vertexData[i].vertices.x, vertexData[i].vertices.y, 0.f);

			vertices[i].texture = XMFLOAT2(vertexData[i].texCoords.u, vertexData[i].texCoords.v);

			vertices[i].color = XMFLOAT4(vertexData[i].colors.r/255.f, vertexData[i].colors.g/255.f, vertexData[i].colors.b/255.f, vertexData[i].colors.a/255.f);
		}
	}
	vertexDataCount = tmpVertexDataCount;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexDataTmp;
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*tmpVertexDataCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexDataTmp.pSysMem = vertices;
	vertexDataTmp.SysMemPitch = 0;
	vertexDataTmp.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = CCID3D11Device->CreateBuffer(&vertexBufferDesc, &vertexDataTmp, &m_vertexBuffer);
	if(FAILED(result))
	{
		CC_SAFE_DELETE_ARRAY(vertices);
		return ;
	}

	CC_SAFE_DELETE_ARRAY(vertices);
}

bool CCDXProgressTimer::InitializeShader()
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D11_BUFFER_DESC matrixBufferDesc;
	// Initialize the pointers this function will use to null.
	errorMessage = 0;

	BasicLoader^ loader = ref new BasicLoader(CCID3D11Device);
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	loader->LoadShader(
		L"CCProgressTimerVertexShader.cso",
		layoutDesc,
		ARRAYSIZE(layoutDesc),
		&m_vertexShader,
		&m_layout
		);

	loader->LoadShader(
		L"CCProgressTimerPixelShader.cso",
		&m_pixelShader
		);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = CCID3D11Device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void CCDXProgressTimer::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	// MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool CCDXProgressTimer::SetShaderParameters(XMMATRIX &viewMatrix,XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = CCID3D11DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	CCID3D11DeviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	CCID3D11DeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	CCID3D11DeviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void CCDXProgressTimer::RenderShader(int& vertexDataCount,CCSprite *pSprite)
{
	CCID3D11DeviceContext->IASetInputLayout(m_layout);
	CCID3D11DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	CCID3D11DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	CCID3D11DeviceContext->PSSetSamplers(0, 1, pSprite->getTexture()->GetSamplerState());
	CCID3D11DeviceContext->Draw(vertexDataCount,0);
	return;
}


void CCDXProgressTimer::Render(ccV2F_C4B_T2F *vertexData,int& vertexDataCount,CCProgressTimerType eType,CCSprite *pSprite)
{
	if ( !mIsInit )
	{
		mIsInit = TRUE;
		FreeBuffer();
		InitializeShader();
	}
	initVertexBuffer(vertexData,vertexDataCount,eType);
	XMMATRIX viewMatrix, projectionMatrix;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	CCD3DCLASS->GetViewMatrix(viewMatrix);
	CCD3DCLASS->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderVertexBuffer();

	// Set the shader parameters that it will use for rendering.
	 SetShaderParameters(viewMatrix, projectionMatrix, pSprite->getTexture()->getTextureResource());

	// Now render the prepared buffers with the shader.
	RenderShader(vertexDataCount,pSprite);
}

} // namespace cocos2d
