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
#include "cocos2dorig.h"
#include "BasicTimer.h"
#include "Classes\AppDelegate.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

cocos2dorig::cocos2dorig() :
    m_windowClosed(false)
{
}

void cocos2dorig::Initialize(CoreApplicationView^ applicationView)
{
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &cocos2dorig::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &cocos2dorig::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &cocos2dorig::OnResuming);

    m_renderer = ref new DirectXRender();
}

void cocos2dorig::SetWindow(CoreWindow^ window)
{
    window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &cocos2dorig::OnWindowClosed);

    window->PointerPressed +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &cocos2dorig::OnPointerPressed);

    window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &cocos2dorig::OnPointerReleased);

    window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &cocos2dorig::OnPointerMoved);

    m_renderer->Initialize(CoreWindow::GetForCurrentThread(), 96.0f);
}

void cocos2dorig::Load(Platform::String^ entryPoint)
{
}

void cocos2dorig::Run()
{

    BasicTimer^ timer = ref new BasicTimer();
	//m_renderer->LoadBox2D();
	while(timer->Total < 2){
		timer->Update();
	}
    while (!m_windowClosed)
    {
        timer->Update();
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        //m_renderer->Update(timer->Total, timer->Delta);
        m_renderer->Render();
        m_renderer->Present(); // This call is synchronized to the display frame rate.
    }
}

void cocos2dorig::Uninitialize()
{
}

void cocos2dorig::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
}

void cocos2dorig::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    CoreWindow::GetForCurrentThread()->Activate();
}

void cocos2dorig::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save application state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations.
    // Be aware that a deferral may not be held indefinitely. After about five
    // seconds, the application will be forced to exit.
     SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

     // Insert your code here

     deferral->Complete();
}

void cocos2dorig::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
}

void cocos2dorig::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    // Insert event handling here
}

void cocos2dorig::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
    // Insert event handling here
}

void cocos2dorig::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
    // Insert event handling here
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new cocos2dorig();
}
namespace cocos2d
{
    // implement in CCApplication_win8_metro.cpp
    extern Windows::ApplicationModel::Core::IFrameworkView^ getSharedCCApplicationFrameworkView();
}

ref class CCApplicationFrameworkViewSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource 
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return cocos2d::getSharedCCApplicationFrameworkView();
    }
};

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    //auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    //CoreApplication::Run(direct3DApplicationSource);

	AppDelegate App;
	auto frameworkViewSource = ref new CCApplicationFrameworkViewSource();
	Windows::ApplicationModel::Core::CoreApplication::Run(frameworkViewSource);
	
    return 0;
}
