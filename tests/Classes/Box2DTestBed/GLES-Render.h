#ifndef RENDER_H
#define RENDER_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace std;
USING_NS_CC;

struct b2AABB;

enum DXBox2DDrawingType
{
	DrawingBox2DPoints  = 0,
	DrawingBox2DLines   = 1,
	DrawingBox2DTrangle  = 2,
	DrawingBox2DPolyClosed  = 3,
	DrawingBox2DPolyOpened  = 4
};
// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class GLESDebugDraw : public b2Draw
{
	struct MatrixBufferType
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	float32 mRatio;
public:
	GLESDebugDraw();
	~GLESDebugDraw();
	GLESDebugDraw( float32 ratio );

	virtual void DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color);

	virtual void DrawSolidPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color);

	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	virtual void DrawTransform(const b2Transform& xf);

    virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    virtual void DrawString(int x, int y, const char* string, ...); 

    virtual void DrawAABB(b2AABB* aabb, const b2Color& color);

public:

	//BOOL initialized;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_vertexBuffer;
	//ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_matrixBuffer;
	DirectX::XMFLOAT4 m_currentColor;
	DXBox2DDrawingType m_DarwingType;
	int	m_vertexAmount;
	ccVertex2F *m_vertices;

	void Drawing(ccVertex2F *vertices, unsigned int numberOfPoints, DXBox2DDrawingType Type);

	void RenderVertexBuffer();
	bool SetShaderParameters(DirectX::XMMATRIX &viewMatrix, DirectX::XMMATRIX &projectionMatrix);
	void RenderShader();
	void Render();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage,WCHAR* shaderFilename);

	void initVertexBuffer(unsigned int numberOfPoints);
	bool InitializeShader(WCHAR* fxFilename);

};


#endif
