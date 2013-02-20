/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2010      Lam Pham

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

#ifndef __MISC_NODE_CCPROGRESS_TIMER_H__
#define __MISC_NODE_CCPROGRESS_TIMER_H__

#include "CCSprite.h"

namespace cocos2d
{
	class CCDXProgressTimer;
/** Types of progress
 @since v0.99.1
 */
typedef enum {
	/// Radial Counter-Clockwise 
	kCCProgressTimerTypeRadialCCW,
	/// Radial ClockWise
	kCCProgressTimerTypeRadialCW,
	/// Horizontal Left-Right
	kCCProgressTimerTypeHorizontalBarLR,
	/// Horizontal Right-Left
	kCCProgressTimerTypeHorizontalBarRL,
	/// Vertical Bottom-top
	kCCProgressTimerTypeVerticalBarBT,
	/// Vertical Top-Bottom
	kCCProgressTimerTypeVerticalBarTB,
} CCProgressTimerType;

/**
 @brief CCProgresstimer is a subclass of CCNode.
 It renders the inner sprite according to the percentage.
 The progress can be Radial, Horizontal or vertical.
 @since v0.99.1
 */
class CC_DLL CCProgressTimer : public CCNode
{
public:
	~CCProgressTimer(void);

	/**	Change the percentage to change progress. */
	inline CCProgressTimerType getType(void) { return m_eType; }

	/** Percentages are from 0 to 100 */
	inline float getPercentage(void) {return m_fPercentage; }

	/** The image to show the progress percentage, retain */
	inline CCSprite* getSprite(void) { return m_pSprite; }

	bool initWithFile(const char *pszFileName);
	bool initWithTexture(CCTexture2D *pTexture);

	void setPercentage(float fPercentage);
	void setSprite(CCSprite *pSprite);
	void setType(CCProgressTimerType type);

	virtual void draw(void);

public:
	static CCProgressTimer* progressWithFile(const char *pszFileName);
	static CCProgressTimer* progressWithTexture(CCTexture2D *pTexture);

protected:
	ccVertex2F vertexFromTexCoord(const CCPoint& texCoord);
	void updateProgress(void);
	void updateBar(void);
	void updateRadial(void);
	void updateColor(void);
	CCPoint boundaryTexCoord(char index);

protected:
	CCProgressTimerType m_eType;
	float m_fPercentage;
	CCSprite *m_pSprite;
	int m_nVertexDataCount;
	ccV2F_C4B_T2F *m_pVertexData;

	static CCDXProgressTimer mDXProgressTimer;

};

class CC_DLL CCDXProgressTimer
{
public:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	CCDXProgressTimer();
	~CCDXProgressTimer();
	void FreeBuffer();
	void initVertexBuffer(ccV2F_C4B_T2F *vertexData,int& vertexDataCount,CCProgressTimerType eType);
	void setIsInit(bool isInit);
	void RenderVertexBuffer();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool InitializeShader();
	bool SetShaderParameters(DirectX::XMMATRIX &viewMatrix,DirectX::XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture);
	void RenderShader(int& vertexDataCount,CCSprite *pSprite);
	void Render(ccV2F_C4B_T2F *vertexData,int& vertexDataCount,CCProgressTimerType eType,CCSprite *pSprite);
	int m_nVertexDataCount2;
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texture;
	};
	bool mIsInit;
};

}

#endif //__MISC_NODE_CCPROGRESS_TIMER_H__
