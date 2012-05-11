#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11_1.h>
#include <d2d1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "CCCommon.h"
#include "CCGeometry.h"
#include <stack>
#include <map>

NS_CC_BEGIN;

class CCSet;
class CCTouch;
class EGLTouchDelegate;

class CC_DLL CCEGLView
{
public:

    CCEGLView();
    virtual ~CCEGLView();

    ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView();

    CCSize  getSize();
    CCSize  getSizeInPixel();
    bool    isOpenGLReady();
    void    release();
    void    setTouchDelegate(EGLTouchDelegate * pDelegate);
    void    swapBuffers();
    bool    canSetContentScaleFactor();
    void    setContentScaleFactor(float contentScaleFactor);
    void    setDesignResolution(int dx, int dy);

	virtual bool Create();

    void setViewPortInPoints(float x, float y, float w, float h);
    void setScissorInPoints(float x, float y, float w, float h);

    void setIMEKeyboardState(bool bOpen);

    void getScreenRectInView(CCRect& rect);
    void setScreenScale(float factor);

    void SetBackBufferRenderTarget();
	void clearRender(ID3D11RenderTargetView* renderTargetView);	
	void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix);
	void GetViewMatrix(DirectX::XMMATRIX& viewMatrix);
	void SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix);
	void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix);

	void GetClearColor(float* color);
	//d3d
	void D3DOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void D3DPerspective(FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf);
	void D3DLookAt(float fEyeX, float fEyeY, float fEyeZ, float fLookAtX, float fLookAtY, float fLookAtZ, float fUpX, float fUpY, float fUpZ);
	void D3DTranslate(float x, float y, float z);
	void D3DRotate(float angle, float x, float y, float z);
	void D3DScale(float x, float y, float z);
	void D3DMultMatrix(const float *m);
	void D3DBlendFunc(int sfactor, int dfactor);
	void D3DViewport(int x, int y, int width, int height);
	void D3DScissor(int x,int y,int w,int h);
	void D3DMatrixMode(int matrixMode);
	void D3DLoadIdentity();
	void D3DPushMatrix();
	void D3DPopMatrix();
	void D3DDepthFunc(int func);
	void D3DClearColor(float r, float b, float g, float a);

    // static function
    /**
    @brief	get the shared main open gl window
    */
	static CCEGLView& sharedOpenGLView();

    // metro only
    void OnWindowSizeChanged();
    void OnCharacterReceived(unsigned int keyCode);
	void OnPointerPressed(int id, const CCPoint& point);
    void OnPointerReleased(int id, const CCPoint& point);
    void OnPointerMoved(int id, const CCPoint& point);
protected:

private:
    ID3D11Device1*           m_d3dDevice;
    ID3D11DeviceContext1*    m_d3dContext;
    IDXGISwapChain1*         m_swapChain;
    ID3D11RenderTargetView*  m_renderTargetView;
    ID3D11DepthStencilView*  m_depthStencilView;

    typedef std::map<int, CCSet*> SetMap;
    SetMap              m_pSets;
    typedef std::map<int, CCTouch*> TouchMap;
    TouchMap            m_pTouches;

    EGLTouchDelegate*   m_pDelegate;
	
    CCSize              m_sizeInPoints;
    float               m_fScreenScaleFactor;
    RECT                m_rcViewPort;

    float               m_fWinScaleX;
    float               m_fWinScaleY;
    int                 m_initWinWidth;
    int                 m_initWinHeight;

    float m_color[4];
    int mMatrixMode;

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_viewMatrix;

	struct MatrixStruct
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
	std::stack<MatrixStruct> m_MatrixStack;
    
    int m_oldViewState;
};

NS_CC_END;
