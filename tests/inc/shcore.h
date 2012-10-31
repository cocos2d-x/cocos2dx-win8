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

#if (NTDDI_VERSION >= NTDDI_WIN8)

#include <objidl.h> // for IStream

#pragma region Application Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)

STDAPI CreateRandomAccessStreamOnFile(_In_ PCWSTR filePath, _In_ DWORD accessMode, _In_ REFIID riid, _COM_Outptr_ void **ppv);


typedef enum
{
    BSOS_DEFAULT = 0,             // when creating a byte seeker over a stream, base randomaccessstream behavior on the STGM mode from IStream::Stat.
    BSOS_PREFERDESTINATIONSTREAM  // in addition, utilize IDestinationStreamFactory::GetDestinationStream.
} BSOS_OPTIONS;

STDAPI CreateRandomAccessStreamOverStream(_In_ IStream *stream, _In_ BSOS_OPTIONS options, _In_ REFIID riid, _COM_Outptr_ void **ppv);
STDAPI CreateStreamOverRandomAccessStream(_In_ IUnknown *randomAccessStream, _In_ REFIID riid, _COM_Outptr_ void **ppv);

#ifdef __cplusplus_winrt
namespace Windows {
    namespace Storage {
        namespace Streams {
            interface class IRandomAccessStream;
        } /* end namespace */
    } /* end namespace */
} /* end namespace */

// Helper that avoids the need to cast IRandomAccessStream^ to IUnknown*.
__inline HRESULT CreateStreamOverRandomAccessStream(_In_ ::Windows::Storage::Streams::IRandomAccessStream ^randomAccessStream, _In_ REFIID riid, _COM_Outptr_ void **ppv)
{
    return CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown *>(randomAccessStream), riid, ppv);
}
#endif // __cplusplus_winrt


#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) */
#pragma endregion

#endif // (NTDDI_VERSION >= NTDDI_WIN8)

