#include <pch.h>
#include "AppDelegate.h"
#include "exception\CCException.h"
#include "CCCommon.h"
using namespace Platform;
USING_NS_CC;

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
	try
	{
		AppDelegate app;
		auto frameworkViewSource = ref new CCApplicationFrameworkViewSource();
		Windows::ApplicationModel::Core::CoreApplication::Run(frameworkViewSource);
	}
	catch(COMException^ e)
	{
        switch (e->HResult)
		{
		case kCCExceptionNoSupportDX11:
			// no video card support DirectX 11
            CCMessageBox("You don't have any video card support DirectX 11.", "Error");
			break;
		default:
			break;
		}
	}
    return 0;
}
