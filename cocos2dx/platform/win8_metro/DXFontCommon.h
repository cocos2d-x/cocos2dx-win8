
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
//----------------------------------------------------------------------------

#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.s

// Windows header files
#include <windows.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <shcore.h>

#include <objidl.h>
#include <strsafe.h>
#include <wrl.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include <ppltasks.h>
#include <agile.h>
#include <collection.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <memory>
#include <vector>
#include <list>

#include "DirectXHelper.h"
#include "FontFileStream.h"
#include "FontLoader.h"