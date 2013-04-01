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

#include "pch.h"

#include <agile.h>
#include "CCApplication.h"

#include "DirectXRender.h"
#include "CCDirector.h"

using namespace Windows::UI::Core;



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

	void OnBackButtonPressed(
		Platform::Object^ sender, 
		Windows::Phone::UI::Input::BackPressedEventArgs^ args
		);
private:
	
    DirectXRender^ m_renderer;
//    Platform::Agile<Windows::ApplicationModel::Core::CoreApplicationView> m_applicationView;
	Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
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

	HardwareButtons::BackPressed += ref new EventHandler<BackPressedEventArgs^>(this, &CCFrameworkView::OnBackButtonPressed); 


    m_renderer = ref new DirectXRender();
    CCLog("CCFrameworkView::-Initialize()");
}

void CCFrameworkView::SetWindow(
    _In_ CoreWindow^ window
    )
{
    CCLog("CCFrameworkView::+SetWindow()");
	m_window = window;
    //window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
	//这时需要将设备的分辨率读出保存
	DeviceResolutionInPixels res;
	switch (DisplayProperties::ResolutionScale) 
	{
		case ResolutionScale::Scale100Percent: 
			{
				res = DeviceResolutionInPixels_WVGA;
				break;
			}
		case ResolutionScale::Scale150Percent: 
			{
				res = DeviceResolutionInPixels_720p;
				break;
			}
		case ResolutionScale::Scale160Percent: 
			{
				res = DeviceResolutionInPixels_WXGA;
				break;
			}
		default:
			{
				res = DeviceResolutionInPixels_WVGA;
				break;
			}
	}
	CCApplication::sharedApplication()->setDeviceResolutionInPixels(res);

    DisplayProperties::LogicalDpiChanged +=
        ref new DisplayPropertiesEventHandler(this, &CCFrameworkView::OnLogicalDpiChanged);

    m_renderer->Initialize(window, DisplayProperties::LogicalDpi);
    CCApplication::sharedApplication()->initInstance();
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
    bool inited = false;

    while (1)
    {
		if (nullptr == m_window)
		{
			// sleep
			continue;
		}

		if (false == inited)
		{
			inited = (CCApplication::sharedApplication()->applicationDidFinishLaunching());
			if (false == inited)
			{
				// init falied
				break;
			}
		}

        // if windows closed exit app
        if (true == m_renderer->GetWindowsClosedState())
        {
            break;
        }

        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        CCDirector::sharedDirector()->mainLoop();
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

void CCFrameworkView::OnBackButtonPressed(Object^ sender, BackPressedEventArgs^ args)
{	
	if (CCApplication::sharedApplication()) 
	{
		CCApplication::sharedApplication()->deviceBackBttonPressed(sender, args);
    } else 
	{
        // Do nothing. Leave args->Handled set to the current value, false.
    }
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

CCApplication::CCApplication():
	m_deviceResolutionInPixels(DeviceResolutionInPixels_Invalid)
{
    CC_ASSERT(! s_pSharedApplication);
    s_pSharedApplication = this;
}

CCApplication::~CCApplication()
{
    CC_ASSERT(this == s_pSharedApplication);
    s_pSharedApplication = NULL;
}

void CCApplication::setDeviceResolutionInPixels(DeviceResolutionInPixels res)
{
	CC_ASSERT(m_deviceResolutionInPixels == DeviceResolutionInPixels_Invalid);
	m_deviceResolutionInPixels = res;
}

DeviceResolutionInPixels CCApplication::getdeviceResolutionInPixels()
{
	return m_deviceResolutionInPixels;
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
CCApplication* CCApplication::sharedApplication()
{
    CC_ASSERT(s_pSharedApplication);
    return s_pSharedApplication;
}

ccLanguageType CCApplication::getCurrentLanguage()
{
	//http://msdn.microsoft.com/zh-cn/library/windowsphone/develop/jj244362(v=vs.105).aspx
	ccLanguageType ret = kLanguageEnglish;
	
	ULONG numLanguages = 0;
    DWORD cchLanguagesBuffer = 0;
    BOOL hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, NULL, &cchLanguagesBuffer);

    if (hr) {
          WCHAR* pwszLanguagesBuffer = new WCHAR[cchLanguagesBuffer];
          hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, pwszLanguagesBuffer, &cchLanguagesBuffer);
          if (hr) 
		  {
				wchar_t* primary = NULL;
				wchar_t* sub = NULL;

				primary = wcstok(pwszLanguagesBuffer, L"-");
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
				delete pwszLanguagesBuffer;
        }
    }
   
    return ret;
}

NS_CC_END;
