/****************************************************************************
Copyright (c) 2012 cocos2d-x.org

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

#pragma once

#include "pch.h"
#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <XInput.h>
#include "DirectXHelper.h"
#include "DXTextPainter.h"

// forward declare
namespace cocos2d
{
    class CCEGLView;
	class CCImage;
}

#pragma warning (disable: 4449) // warns that class is not consumable by JS because it is not sealed

// Helper class that initializes the DirectX APIs in the sample apps.
public ref class DirectXRender 
{
public:
	internal:
    DirectXRender();

    void Initialize(Windows::UI::Core::CoreWindow^ window, float dpi);
    void CreateDeviceIndependentResources();
    void CreateDeviceResources();
    void SetDpi(float dpi);
    void UpdateForWindowSizeChange();
    void CreateWindowSizeDependentResources();
    void Render();
    void Present();
	void SetBackBufferRenderTarget();
    bool GetWindowsClosedState();
	bool GetWindowIsVisible();
    static DirectXRender^ SharedDXRender();

private:
	    Platform::Agile<Windows::UI::Core::CoreWindow>  m_window;
		DXTextPainter^									m_textPainter;

private:
    bool SetRasterState();

private:
    friend class cocos2d::CCEGLView;
	friend class cocos2d::CCImage;



    // Declare Direct2D Objects
    Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmap;

    // Declare DirectWrite & Windows Imaging Component Objects
    Microsoft::WRL::ComPtr<IDWriteFactory1>         m_dwriteFactory;
    Microsoft::WRL::ComPtr<IWICImagingFactory2>     m_wicFactory;

    // Direct3D Objects
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Windows::Foundation::Size                       m_renderTargetSize;
    Windows::Foundation::Rect                       m_windowBounds;
    float                                           m_dpi;


    bool m_windowClosed;

protected:
    void OnWindowClosed(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::CoreWindowEventArgs^ args
        );

    void OnWindowVisibilityChanged(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args
        );

    void OnWindowSizeChanged(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
        );
    
    void OnPointerPressed(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::PointerEventArgs^ args
        );

    void OnPointerReleased(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::PointerEventArgs^ args
        );

    void OnPointerMoved(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::PointerEventArgs^ args
        );

    void OnCharacterReceived(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::CharacterReceivedEventArgs^ args
        );

};

#pragma warning (default: 4449)
