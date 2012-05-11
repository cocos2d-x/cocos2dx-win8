/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "CCParticleSystemPoint.h"
#include "platform/CCGL.h"
#include "CCDirector.h"

namespace cocos2d {

#if (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX) && (CC_TARGET_PLATFORM != CC_PLATFORM_QNX)

//implementation CCParticleSystemPoint
bool CCParticleSystemPoint::initWithTotalParticles(unsigned int numberOfParticles)
{
	if( CCParticleSystem::initWithTotalParticles(numberOfParticles) )
	{
		m_pVertices = new ccPointSprite[m_uTotalParticles];

		if( ! m_pVertices )
		{
			CCLOG("cocos2d: Particle system: not enough memory");
			this->release();
			return false;
		}

#if CC_USES_VBO
		/*
		glGenBuffers(1, &m_uVerticesID);

		// initial binding
		glBindBuffer(CC_ARRAY_BUFFER, m_uVerticesID);
		glBufferData(CC_ARRAY_BUFFER, sizeof(ccPointSprite)*m_uTotalParticles, m_pVertices, CC_DYNAMIC_DRAW);
		glBindBuffer(CC_ARRAY_BUFFER, 0);
		*/
#endif
		return true;
	}
	return false;
}
CCParticleSystemPoint::~CCParticleSystemPoint()
{
    CC_SAFE_DELETE(m_pVertices);
#if CC_USES_VBO
	//glDeleteBuffers(1, &m_uVerticesID);
#endif
}

// implementation CCParticleSystemPoint
CCParticleSystemPoint * CCParticleSystemPoint::particleWithFile(const char *plistFile)
{
    CCParticleSystemPoint *pRet = new CCParticleSystemPoint();
    if (pRet && pRet->initWithFile(plistFile))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet)
        return pRet;
}

void CCParticleSystemPoint::updateQuadWithParticle(tCCParticle* particle, const CCPoint& newPosition)
{
	// place vertices and colos in array
    m_pVertices[m_uParticleIdx].pos = vertex2(newPosition.x, newPosition.y);
	m_pVertices[m_uParticleIdx].size = particle->size;
	ccColor4B color = {(CCubyte)(particle->color.r * 255), (CCubyte)(particle->color.g * 255), (CCubyte)(particle->color.b * 255), 
		(CCubyte)(particle->color.a * 255)};
	m_pVertices[m_uParticleIdx].color = color;
}
void CCParticleSystemPoint::postStep()
{
#if CC_USES_VBO
	/*
	glBindBuffer(CC_ARRAY_BUFFER, m_uVerticesID);
	glBufferSubData(CC_ARRAY_BUFFER, 0, sizeof(ccPointSprite)*m_uParticleCount, m_pVertices);
	glBindBuffer(CC_ARRAY_BUFFER, 0);
	*/
#endif
}
void CCParticleSystemPoint::draw()
{
	CCParticleSystem::draw();

	if (m_uParticleIdx==0)
	{
		return;
	}

	// Default GL states: CC_TEXTURE_2D, CC_VERTEX_ARRAY, CC_COLOR_ARRAY, CC_TEXTURE_COORD_ARRAY
	// Needed states: CC_TEXTURE_2D, CC_VERTEX_ARRAY, CC_COLOR_ARRAY
	// Unneeded states: CC_TEXTURE_COORD_ARRAY
	/*
	glDisableClientState(CC_TEXTURE_COORD_ARRAY);

	glBindTexture(CC_TEXTURE_2D, m_pTexture->getName());

	glEnable(CC_POINT_SPRITE_OES);
	glTexEnvi( CC_POINT_SPRITE_OES, CC_COORD_REPLACE_OES, CC_TRUE );
	*/

#define kPointSize sizeof(m_pVertices[0])

#if CC_USES_VBO
	//glBindBuffer(CC_ARRAY_BUFFER, m_uVerticesID);

#if CC_ENABLE_CACHE_TEXTTURE_DATA
    //glBufferData(CC_ARRAY_BUFFER, sizeof(ccPointSprite)*m_uTotalParticles, m_pVertices, CC_DYNAMIC_DRAW);
#endif
	/*
	glVertexPointer(2,CC_FLOAT,kPointSize,0);

	glColorPointer(4, CC_UNSIGNED_BYTE, kPointSize,(CCvoid*)offsetof(ccPointSprite,color) );

	glEnableClientState(CC_POINT_SIZE_ARRAY_OES);

	glPointSizePointerOES(CC_FLOAT,kPointSize,(CCvoid*) offsetof(ccPointSprite,size) );
	*/
#else // Uses Vertex Array List
	/*
    int offset = (int)m_pVertices;
    glVertexPointer(2,CC_FLOAT, kPointSize, (CCvoid*) offset);

    int diff = offsetof(ccPointSprite, color);
    glColorPointer(4, CC_UNSIGNED_BYTE, kPointSize, (CCvoid*) (offset+diff));

    glEnableClientState(CC_POINT_SIZE_ARRAY_OES);
    diff = offsetof(ccPointSprite, size);
    glPointSizePointerOES(CC_FLOAT, kPointSize, (CCvoid*) (offset+diff));
	*/
#endif 
	/*
    bool newBlend = (m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST) ? true : false;
	if( newBlend ) 
	{
		glBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
	}

	glDrawArrays(CC_POINTS, 0, m_uParticleIdx);

	// restore blend state
	if( newBlend )
		glBlendFunc( CC_BLEND_SRC, CC_BLEND_DST);
	*/
#if CC_USES_VBO
	// unbind VBO buffer
	//glBindBuffer(CC_ARRAY_BUFFER, 0);
#endif
	/*
	glDisableClientState(CC_POINT_SIZE_ARRAY_OES);
	glDisable(CC_POINT_SPRITE_OES);

	// restore GL default state
	glEnableClientState(CC_TEXTURE_COORD_ARRAY);
	*/
}

// Non supported properties

//
// SPIN IS NOT SUPPORTED
//
void CCParticleSystemPoint::setStartSpin(float var)
{
	CCAssert(var == 0, "PointParticleSystem doesn't support spinning");
	CCParticleSystem::setStartSpin(var);
}
void CCParticleSystemPoint::setStartSpinVar(float var)
{
	CCAssert(var == 0, "PointParticleSystem doesn't support spinning");
	CCParticleSystem::setStartSpinVar(var);
}
void CCParticleSystemPoint::setEndSpin(float var)
{
	CCAssert(var == 0, "PointParticleSystem doesn't support spinning");
	CCParticleSystem::setEndSpin(var);
}
void CCParticleSystemPoint::setEndSpinVar(float var)
{
	CCAssert(var == 0, "PointParticleSystem doesn't support spinning");
	CCParticleSystem::setEndSpinVar(var);
}
//
// SIZE > 64 IS NOT SUPPORTED
//
void CCParticleSystemPoint::setStartSize(float size)
{
	CCAssert(size >= 0 && size <= CC_MAX_PARTICLE_SIZE, "PointParticleSystem only supports 0 <= size <= 64");
	CCParticleSystem::setStartSize(size);
}
void CCParticleSystemPoint::setEndSize(float size)
{
	CCAssert( (size == kCCParticleStartSizeEqualToEndSize) ||
		( size >= 0 && size <= CC_MAX_PARTICLE_SIZE), "PointParticleSystem only supports 0 <= size <= 64");
	CCParticleSystem::setEndSize(size);
}

#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)

}// namespace cocos2d
