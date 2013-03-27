/****************************************************************************
Copyright (c) 2010-2011  cocos2d-x.org
Copyright (c) 2008, 2009 Jason Booth

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

// For licensing information relating to this distribution please see Third Party Notices file.

#ifndef __CCMOTION_STREAK_H__
#define __CCMOTION_STREAK_H__

#include "CCNode.h"
#include "CCProtocols.h"

namespace cocos2d {

class CCRibbon;
/**
* @brief CCMotionStreak manages a Ribbon based on it's motion in absolute space.
* You construct it with a fadeTime, minimum segment size, texture path, texture
* length and color. The fadeTime controls how long it takes each vertex in
* the streak to fade out, the minimum segment size it how many pixels the
* streak will move before adding a new ribbon segement, and the texture
* length is the how many pixels the texture is stretched across. The texture
* is vertically aligned along the streak segemnts. 
*
* Limitations:
*   CCMotionStreak, by default, will use the CC_SRC_ALPHA, CC_ONE_MINUS_SRC_ALPHA blending function.
*   This blending function might not be the correct one for certain textures.
*   But you can change it by using:
*     [obj setBlendFunc: (ccBlendfunc) {new_src_blend_func, new_dst_blend_func}];
*
* @since v0.8.1
*/
class CC_DLL CCMotionStreak : public CCNode, public CCTextureProtocol
{
	/** Ribbon used by MotionStreak (weak reference) */
	CC_PROPERTY_READONLY(CCRibbon*, m_pRibbon, Ribbon)
	//CCTextureProtocol methods
	CC_PROPERTY(CCTexture2D*, m_pTexture, Texture)
	CC_PROPERTY(ccBlendFunc, m_tBlendFunc, BlendFunc)
public:
	CCMotionStreak()
		: m_pRibbon(NULL)
        , m_pTexture(NULL)
        , m_fSegThreshold(0.0)
		, m_fWidth(0.0)		
	{}
	virtual ~CCMotionStreak(){}
    /** creates and initializes a motion streak with fade in seconds, minimum segments, stroke's width, color, texture filename */
    static CCMotionStreak* create(float fade, float minSeg, float stroke, ccColor3B color, const char* path);
    /** creates and initializes a motion streak with fade in seconds, minimum segments, stroke's width, color, texture */
    static CCMotionStreak* create(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture);

    /** initializes a motion streak with fade in seconds, minimum segments, stroke's width, color and texture filename */
    bool initWithFade(float fade, float minSeg, float stroke, ccColor3B color, const char* path);
    /** initializes a motion streak with fade in seconds, minimum segments, stroke's width, color and texture  */
    bool initWithFade(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture);

	/** polling function */
	void update(ccTime delta);

	 /** When fast mode is enabled, new points are added faster but with lower precision */
    inline bool isFastMode() { return m_bFastMode; }
    inline void setFastMode(bool bFastMode) { m_bFastMode = bFastMode; }
	inline bool isStartingPositionInitialized() { return m_bStartingPositionInitialized; }
    inline void setStartingPositionInitialized(bool bStartingPositionInitialized) 
    { 
        m_bStartingPositionInitialized = bStartingPositionInitialized; 
    }

	virtual void setColor(const ccColor3B& color);
    virtual const ccColor3B& getColor(void);
protected:
	float		m_fSegThreshold;
	float		m_fWidth;
	CCPoint		m_tLastLocation;
    bool        m_bFastMode;
	bool m_bStartingPositionInitialized;
private:
    float m_fStroke;
    float m_fFadeDelta;
    float m_fMinSeg;
	    /** texture used for the motion streak */
    //CCTexture2D* m_pTexture;
    //ccBlendFunc m_tBlendFunc;
    CCPoint m_tPositionR;
    ccColor3B m_tColor;
	unsigned int m_uMaxPoints;
    unsigned int m_uNuPoints;
    unsigned int m_uPreviousNuPoints;
	/** Pointers */
    CCPoint* m_pPointVertexes;
    float* m_pPointState;
	ccVertex2F* m_pVertices;
    CCubyte* m_pColorPointer;
    ccTex2F* m_pTexCoords;

};

} // namespace cocos2d

#endif //__CCMOTION_STREAK_H__
