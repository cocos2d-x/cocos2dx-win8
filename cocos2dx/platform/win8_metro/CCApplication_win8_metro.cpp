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

#include "CCApplication.h"

#include "DirectXRender.h"
#include "CCDirector.h"
#include "SimpleAudioEngine.h"

NS_CC_BEGIN;

ref class CCFrameworkView sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
    CCFrameworkView();
    
    // IFrameworkView Methods
    virtual void Initialize( Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
    virtual void SetWindow( Windows::UI::Core::CoreWindow^ window);
    virtual void Load(Platform::String^ entryPoint);
    virtual void Run();
    virtual void Uninitialize();

    // Event Handlers
    void OnActivated(
        _In_ Windows::ApplicationModel::Core::CoreApplicationView^ applicationView,
        _In_ Windows::ApplicationModel::Activation::IActivatedEventArgs^ args
        );

    void OnWindowActivationChanged(
        _In_ Windows::UI::Core::CoreWindow^ sender,
        _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
        );

    void OnSuspending(
        _In_ Platform::Object^ sender,
        _In_ Windows::ApplicationModel::SuspendingEventArgs^ args
        );

    void OnResuming(
        _In_ Platform::Object^ sender,
        _In_ Platform::Object^ args
        );

    void OnLogicalDpiChanged(
        _In_ Platform::Object^ sender
        );

	void OnVisibilityChanged(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::VisibilityChangedEventArgs^ args
		);

	void OnWindowClosed(
    _In_ Windows::UI::Core::CoreWindow^ sender,
    _In_ Windows::UI::Core::CoreWindowEventArgs^ args
    );

private:
    DirectXRender^ m_renderer;
	bool m_windowVisible;
	bool m_windowClosed;
};

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;

CCFrameworkView::CCFrameworkView()
	:m_windowVisible(false)
	,m_windowClosed(false)
{
    CCLog("CCFrameworkView::+CCFrameworkView()");
    CCLog("CCFrameworkView::-CCFrameworkView()");
}

void CCFrameworkView::Initialize(
     CoreApplicationView^ applicationView
    )
{
    CCLog("CCFrameworkView::+Initialize()");
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &CCFrameworkView::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &CCFrameworkView::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &CCFrameworkView::OnResuming);

    m_renderer = ref new DirectXRender();
    CCLog("CCFrameworkView::-Initialize()");
}

void CCFrameworkView::SetWindow(
    _In_ CoreWindow^ window
    )
{
	CCLog("CCFrameworkView::+SetWindow()");
	window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &CCFrameworkView::OnVisibilityChanged);

	window->Closed +=
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &CCFrameworkView::OnWindowClosed);

	DisplayProperties::LogicalDpiChanged +=
		ref new DisplayPropertiesEventHandler(this, &CCFrameworkView::OnLogicalDpiChanged);

	m_renderer->Initialize(window, DisplayProperties::LogicalDpi);
	CCApplication::sharedApplication().initInstance();
	CCLog("CCFrameworkView::-SetWindow()");
}

void CCFrameworkView::Load(
    Platform::String^ entryPoint
    )
{
    CCLog("CCFrameworkView::+Load()");
    CCLog("CCFrameworkView::-Load()");
}

void CCFrameworkView::Run()
{
    CCLog("CCFrameworkView::+Run()");

    // if applicationDidFinishLaunching return false, exist.
	if(!CCApplication::sharedApplication().applicationDidFinishLaunching())
		return;

    while (!m_windowClosed)
    {
		if (m_windowVisible)
        {
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			CCDirector::sharedDirector()->mainLoop();
			CocosDenshion::SimpleAudioEngine::sharedEngine()->render();
		}else{
			// The window is not visible, so just wait for next event and respond to it.
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
    }
//    m_renderer->OnSuspending();  // the app is exiting so do the same thing as would if app was being suspended.
    CCLog("CCFrameworkView::-Run()");
}

void CCFrameworkView::Uninitialize()
{
    CCLog("CCFrameworkView::+Uninitialize()");
	m_renderer = nullptr;
    CCLog("CCFrameworkView::-Uninitialize()");
}

void CCFrameworkView::OnActivated(
    _In_ CoreApplicationView^ applicationView,
    _In_ IActivatedEventArgs^ args
    )
{
    CCLog("CCFrameworkView::+OnActivated()");
    CoreWindow::GetForCurrentThread()->Activated += 
        ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>(this, &CCFrameworkView::OnWindowActivationChanged);
    CoreWindow::GetForCurrentThread()->Activate();
	m_windowVisible = true;
    CCLog("CCFrameworkView::-OnActivated()");
}

void CCFrameworkView::OnWindowActivationChanged(
    _In_ Windows::UI::Core::CoreWindow^ sender,
    _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
    )
{
    CCLog("CCFrameworkView::+OnWindowActivationChanged()");
    if (args->WindowActivationState == CoreWindowActivationState::Deactivated)
    {
        //m_renderer->OnFocusChange(false);
        // CCApplication::sharedApplication().applicationDidEnterBackground();
    }
    else if (args->WindowActivationState == CoreWindowActivationState::CodeActivated 
        || args->WindowActivationState == CoreWindowActivationState::PointerActivated)
    {
        // CCApplication::sharedApplication().applicationWillEnterForeground();
        //m_renderer->OnFocusChange(true);
    }
    CCLog("CCFrameworkView::-OnWindowActivationChanged()");
}

void CCFrameworkView::OnSuspending(
    _In_ Platform::Object^ sender,
    _In_ SuspendingEventArgs^ args
    )
{
    CCLog("CCFrameworkView::+OnSuspending()");
    // Save application state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations.
    // Be aware that a deferral may not be held indefinitely. After about five
    // seconds, the application will be forced to exit.

    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
    //m_renderer->OnSuspending();
    deferral->Complete();
    CCLog("CCFrameworkView::-OnSuspending()");
}
 
void CCFrameworkView::OnResuming(
    _In_ Platform::Object^ sender,
    _In_ Platform::Object^ args
    )
{
    CCLog("CCFrameworkView::+OnResuming()");
    //m_renderer->OnResuming();
    CCLog("CCFrameworkView::-OnResuming()");
}

void CCFrameworkView::OnLogicalDpiChanged(
    _In_ Platform::Object^ sender
    )
{
    CCLog("CCFrameworkView::+OnLogicalDpiChanged()");
    m_renderer->SetDpi(DisplayProperties::LogicalDpi);
    CCLog("CCFrameworkView::-OnLogicalDpiChanged()");
}

void CCFrameworkView::OnVisibilityChanged(
    _In_ CoreWindow^ sender,
    _In_ VisibilityChangedEventArgs^ args
    )
{
    m_windowVisible = args->Visible;
	if (m_windowVisible)
	{
		CCApplication::sharedApplication().applicationWillEnterForeground();
	} 
	else
	{
		CCApplication::sharedApplication().applicationDidEnterBackground();
	}
}

void CCFrameworkView::OnWindowClosed(
    _In_ CoreWindow^ sender,
    _In_ CoreWindowEventArgs^ args
    )
{
    m_windowClosed = true;
	CCLog("CCFrameworkView::OnWindowClosed");
}


Windows::ApplicationModel::Core::IFrameworkView^ getSharedCCApplicationFrameworkView()
{
    return ref new CCFrameworkView();
}

////////////////////////////////////////////////////////////////////////////////
// implement CCApplication
////////////////////////////////////////////////////////////////////////////////

// sharedApplication pointer
CCApplication * s_pSharedApplication = 0;

CCApplication::CCApplication()
{
    CC_ASSERT(! s_pSharedApplication);
    s_pSharedApplication = this;
}

CCApplication::~CCApplication()
{
    CC_ASSERT(this == s_pSharedApplication);
    s_pSharedApplication = NULL;
}

void CCApplication::setAnimationInterval(double interval)
{
    // app need do nothing on metro. frame control in run.
}

CCApplication::Orientation CCApplication::setOrientation(Orientation orientation)
{
    return orientation;
}

void CCApplication::statusBarFrame(CCRect * rect)
{
    if (rect)
    {
        // Windows doesn't have status bar.
        *rect = CCRectMake(0, 0, 0, 0);
    }
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
CCApplication& CCApplication::sharedApplication()
{
    CC_ASSERT(s_pSharedApplication);
    return *s_pSharedApplication;
}

ccLanguageType CCApplication::getCurrentLanguage()
{
    ccLanguageType ret = kLanguageEnglish;

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};

    if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH))
    {
        wchar_t* primary = NULL;
        wchar_t* sub = NULL;

        primary = wcstok(localeName, L"-");
        sub = wcstok(NULL, L"-");
        
        if (wcscmp(primary, L"zh") == 0)
        {
            //Chinese, Simplified Chinese and Traditional Chinese
            if (wcscmp(sub, L"TW") == 0 ||
                wcscmp(sub, L"HK") == 0 ||
                wcscmp(sub, L"MO") == 0)
            {
                ret = kLanguageChinese_Traditional;
            }
            else
            {
                ret = kLanguageChinese_Simplified;
            }
        }
        else if (wcscmp(primary, L"ja") == 0)
        {
            ret = kLanguageJapanese;
        }
        else if (wcscmp(primary, L"fr") == 0)
        {
            ret = kLanguageFrench;
        }
        else if (wcscmp(primary, L"it") == 0)
        {
            ret = kLanguageItalian;
        }
        else if (wcscmp(primary, L"de") == 0)
        {
            ret = kLanguageGerman;
        }
        else if (wcscmp(primary, L"es") == 0)
        {
            ret = kLanguageSpanish;
        }
        else if (wcscmp(primary, L"ru") == 0)
        {
            ret = kLanguageRussian;
        }
    }

    return ret;
}

NS_CC_END;
