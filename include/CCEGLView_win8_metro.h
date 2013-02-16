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

#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11_1.h>
//#include <d2d1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "CCCommon.h"
#include "CCGeometry.h"
#include <stack>
#include <vector>
#include <map>

NS_CC_BEGIN;

class CCSet;
class CCTouch;
class EGLTouchDelegate;

template<typename T, size_t TALIGN=16, size_t TBLOCK=8>
class aligned_allocator
{
public:
	typedef T * pointer;
	typedef const T * const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	T * address(T& r) const 
	{
		return &r;
	}
	const T * address(const T& s) const 
	{
		return &s;
	}
	size_t max_size() const 
	{
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}
	template <typename U> struct rebind 
	{
		typedef aligned_allocator<U> other;
	};
	bool operator!=(const aligned_allocator& other) const 
	{
		return !(*this == other);
	}
	bool operator==(const aligned_allocator& other) const 
	{
		return true;
	}
	void construct(pointer p, const T &val) 
	{
		new (p) T(val);
	}
	void destroy(pointer p) 
	{
		p->~T();
	}
	aligned_allocator() 
	{
	}
	aligned_allocator(const aligned_allocator &) 
	{
	}
	template<typename U> aligned_allocator(const aligned_allocator<U>&) 
	{
	}
	~aligned_allocator() 
	{
	}
	pointer allocate(size_t n) 
	{
		return allocate(n, NULL);
	}
	pointer allocate(size_t n, const void *hint) 
	{
		pointer p = NULL;
		size_t count = sizeof(T) * n;
		size_t count_left = count % TBLOCK;
		if (0 != count_left) {
			count += TBLOCK - count_left;
		}
		if (!hint) {
			p = reinterpret_cast<pointer>(_aligned_malloc(count, TALIGN));
		}
		else {
			p = reinterpret_cast<pointer>(_aligned_realloc((void *)hint, count, TALIGN));
		}
		return p;
	}
	void deallocate(pointer p, size_t) 
	{
		_aligned_free(p);
	}
};

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
	void	render();
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
#if defined(_XM_SSE_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
	std::stack<MatrixStruct, std::deque<MatrixStruct, aligned_allocator<MatrixStruct> > > m_MatrixStack;
#else 
	std::stack<MatrixStruct> m_MatrixStack;
#endif
    int m_oldViewState;
};

NS_CC_END;
