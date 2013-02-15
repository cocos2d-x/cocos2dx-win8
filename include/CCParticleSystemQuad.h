/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Leonardo Kasperavičius
Copyright (c) 2011      ynga Inc.
 
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

#ifndef __CC_PARTICLE_SYSTEM_QUAD_H__
#define __CC_PARTICLE_SYSTEM_QUAD_H__

#include  "CCParticleSystem.h"
#include <fstream>
#include <string>
namespace cocos2d {

class CCDXParticleSystemQuad;
class CCSpriteFrame;
/** @brief CCParticleSystemQuad is a subclass of CCParticleSystem

It includes all the features of ParticleSystem.

Special features and Limitations:	
- Particle size can be any float number.
- The system can be scaled
- The particles can be rotated
- On 1st and 2nd gen iPhones: It is only a bit slower that CCParticleSystemPoint
- On 3rd gen iPhone and iPads: It is MUCH faster than CCParticleSystemPoint
- It consumes more RAM and more GPU memory than CCParticleSystemPoint
- It supports subrects
@since v0.8
*/
class CC_DLL CCParticleSystemQuad : public CCParticleSystem
{
protected:
	ccV2F_C4B_T2F_Quad	*m_pQuads;		// quads to be rendered
	CCushort			*m_pIndices;	// indices
#if CC_USES_VBO
	CCuint				m_uQuadsID;	// VBO id
#endif
public:
	CCParticleSystemQuad();
	virtual ~CCParticleSystemQuad();

    /** creates an initializes a CCParticleSystemQuad from a plist file.
    This plist files can be creted manually or with Particle Designer:  
    */
    static CCParticleSystemQuad * particleWithFile(const char *plistFile);

	/** initialices the indices for the vertices*/
	void initIndices();

    /** initilizes the texture with a rectangle measured Points */
	void initTexCoordsWithRect(const CCRect& rect);

	/** Sets a new CCSpriteFrame as particle.
	WARNING: this method is experimental. Use setTexture:withRect instead.
	@since v0.99.4
	*/
	void setDisplayFrame(CCSpriteFrame *spriteFrame);

    /** Sets a new texture with a rect. The rect is in Points.
	@since v0.99.4
	*/
	void setTextureWithRect(CCTexture2D *texture, const CCRect& rect);
	// super methods
	virtual bool initWithTotalParticles(unsigned int numberOfParticles);
	virtual void setTexture(CCTexture2D* texture);
	virtual void updateQuadWithParticle(tCCParticle* particle, const CCPoint& newPosition);
	virtual void postStep();
	virtual void draw();

	int m_vertexCount, m_indexCount;

	static CCDXParticleSystemQuad mDXParticleSystemQuad;
};


class CC_DLL CCDXParticleSystemQuad
{
public:
	CCDXParticleSystemQuad();
	~CCDXParticleSystemQuad();

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_vertexBuffer;

	void FreeBuffer();
	void setIsInit(bool isInit);
	bool InitializeShader();

	void initVertexAndIndexBuffer(unsigned short* indices,unsigned int uTotalParticles);
	void RenderVertexBuffer(ccV2F_C4B_T2F_Quad *quad,unsigned int uTotalParticles);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool SetShaderParameters(DirectX::XMMATRIX &viewMatrix, DirectX::XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture);
	void RenderShader(unsigned int particleIdx,CCTexture2D* texture);
	void Render(ccV2F_C4B_T2F_Quad *quad,unsigned short* indices,unsigned int uTotalParticles,unsigned int particleIdx,CCTexture2D* texture);

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
	struct VertexType
	{
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texture;
	};

	unsigned int m_uMaxTotalParticles;
	bool m_bIsInit;
};

}// namespace cocos2d

#endif //__CC_PARTICLE_SYSTEM_QUAD_H__

