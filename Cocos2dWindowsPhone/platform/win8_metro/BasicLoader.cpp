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
#include "BasicLoader.h"
#include "DirectXHelper.h"
#include <memory>
#include <map>

using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;


unsigned int BKDRHash2(wchar_t *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}


class CacheDataInfo
{
public:
    CacheDataInfo()
    : data(NULL)
    , size(0)
    {
    }

    ~CacheDataInfo()
    {
       if(data){
		   delete[] data;
	   }
	   data=0;
    }

    unsigned char* data;
    unsigned long size;
};

class CacheHandler
{
    std::map<unsigned int, CacheDataInfo*> * s_pCache;
public:
    CacheHandler() : s_pCache(0)
    {

    }

    ~CacheHandler()
    {
		BasicLoader::purgeCachedFileData();
        delete s_pCache;
    }

    std::map<unsigned int, CacheDataInfo*>& getCache()
    {
        if (! s_pCache)
        {
            s_pCache = new std::map<unsigned int, CacheDataInfo*>;
        }
        return *s_pCache;
    }
};

static CacheHandler s_CacheHandler;


BasicLoader::BasicLoader(
    _In_ ID3D11Device* d3dDevice
    //_In_opt_ IWICImagingFactory2* wicFactory
    ) : 
    m_d3dDevice(d3dDevice)
   // m_wicFactory(wicFactory)
{
    // Create a new BasicReaderWriter to do raw file I/O.
    m_basicReaderWriter = ref new BasicReaderWriter();
}

template<class DeviceChildType>
inline void BasicLoader::SetDebugName(
    _In_ DeviceChildType* object,
    _In_ Platform::String^ name
    )
{
//#if defined(_DEBUG)
//    // Only assign debug names in debug builds.
//
//    char nameString[1024];
//    int nameStringLength = WideCharToMultiByte(
//        CP_ACP,
//        0,
//        name->Data(),
//        -1, 
//        nameString,
//        1024,
//        nullptr,
//        nullptr
//        );
//
//    if (nameStringLength == 0 )
//    {
//        char defaultNameString[] = "BasicLoaderObject";
//        DX::ThrowIfFailed(
//            object->SetPrivateData(
//                WKPDID_D3DDebugObjectName,
//                sizeof(defaultNameString) - 1,
//                defaultNameString
//                )
//            );
//    }
//    else
//    {
//        DX::ThrowIfFailed(
//            object->SetPrivateData(
//                WKPDID_D3DDebugObjectName,
//                nameStringLength - 1,
//                nameString
//                )
//            );
//    }
//#endif
}

Platform::String^ BasicLoader::GetExtension(
    _In_ Platform::String^ filename
    )
{
    int lastDotIndex = -1;
    for (int i = filename->Length() - 1; i >= 0 && lastDotIndex == -1; i--)
    {
        if (*(filename->Data() + i) == '.')
        {
            lastDotIndex = i;
        }
    }
    if (lastDotIndex != -1)
    {
        std::unique_ptr<wchar_t[]> extension(new wchar_t[filename->Length() - lastDotIndex]);
        for (unsigned int i = 0; i < filename->Length() - lastDotIndex; i++)
        {
            extension[i] = tolower(*(filename->Data() + lastDotIndex + 1 + i));
        }
        return ref new Platform::String(extension.get());
    }
    return "";
}

void BasicLoader::CreateInputLayout(
    _In_reads_bytes_(bytecodeSize) byte* bytecode,
    _In_ uint32 bytecodeSize,
    _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC* layoutDesc,
    _In_ uint32 layoutDescNumElements,
    _Out_ ID3D11InputLayout** layout
    )
{
    if (layoutDesc == nullptr)
    {
        // If no input layout is specified, use the BasicVertex layout.
        const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        DX::ThrowIfFailed(
            m_d3dDevice->CreateInputLayout(
                basicVertexLayoutDesc,
                ARRAYSIZE(basicVertexLayoutDesc),
                bytecode,
                bytecodeSize,
                layout
                )
            );
    }
    else
    {
        DX::ThrowIfFailed(
            m_d3dDevice->CreateInputLayout(
                layoutDesc,
                layoutDescNumElements,
                bytecode,
                bytecodeSize,
                layout
                )
            );
    }
}

void BasicLoader::LoadShader(
	_In_ Platform::String^ filename,
	_In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
	_In_ uint32 layoutDescNumElements,
	_Out_ ID3D11VertexShader** shader,
	_Out_opt_ ID3D11InputLayout** layout
	)
{
	unsigned char * pBuffer = NULL;
	do{
		// get data from cache 
		unsigned int hashKey = BKDRHash2((wchar_t*)filename->Data());
		CacheDataInfo* pTemp = NULL;
		std::map<unsigned int, CacheDataInfo*>::iterator it = s_CacheHandler.getCache().find(hashKey);
		if (it != s_CacheHandler.getCache().end())
		{
			pTemp = it->second;
			DX::ThrowIfFailed(
				m_d3dDevice->CreateVertexShader(
				pTemp->data,
				pTemp->size,
				nullptr,
				shader
				)
				);


			SetDebugName(*shader, filename);

			if (layout != nullptr)
			{
				CreateInputLayout(
					pTemp->data,
					pTemp->size,
					layoutDesc,
					layoutDescNumElements,
					layout
					);

				SetDebugName(*layout, filename);
			}
			break;
		}

		Platform::Array<byte>^ bytecode = m_basicReaderWriter->ReadData(filename);

		DX::ThrowIfFailed(
			m_d3dDevice->CreateVertexShader(
			bytecode->Data,
			bytecode->Length,
			nullptr,
			shader
			)
			);


		SetDebugName(*shader, filename);

		if (layout != nullptr)
		{
			CreateInputLayout(
				bytecode->Data,
				bytecode->Length,
				layoutDesc,
				layoutDescNumElements,
				layout
				);

			SetDebugName(*layout, filename);
		}

		//cache the data
		pBuffer = bytecode->Data;
		CacheDataInfo* pCache = new CacheDataInfo();
		pCache->data = new unsigned char[bytecode->Length];
		pCache->size = bytecode->Length;
		memcpy(pCache->data, pBuffer, pCache->size);
		s_CacheHandler.getCache().insert(std::pair<unsigned int, CacheDataInfo*>(hashKey, pCache));

	}while(0);
}

void BasicLoader::LoadShader(
	_In_ Platform::String^ filename,
	_Out_ ID3D11PixelShader** shader
	)
{
	unsigned char * pBuffer = NULL;
	do{
		// get data from cache 
		unsigned int hashKey = BKDRHash2((wchar_t*)filename->Data());
		CacheDataInfo* pTemp = NULL;
		std::map<unsigned int, CacheDataInfo*>::iterator it = s_CacheHandler.getCache().find(hashKey);
		if (it != s_CacheHandler.getCache().end())
		{
			pTemp = it->second;
			DX::ThrowIfFailed(
				m_d3dDevice->CreatePixelShader(
				pTemp->data,
				pTemp->size,
				nullptr,
				shader
				)
				);
			break;
		}

		Platform::Array<byte>^ bytecode = m_basicReaderWriter->ReadData(filename);
		DX::ThrowIfFailed(
			m_d3dDevice->CreatePixelShader(
			bytecode->Data,
			bytecode->Length,
			nullptr,
			shader
			)
			);

		//cache the data
		pBuffer = bytecode->Data;
		CacheDataInfo* pCache = new CacheDataInfo();
		pCache->data = new unsigned char[bytecode->Length];
		pCache->size = bytecode->Length;
		memcpy(pCache->data, pBuffer, pCache->size);
		s_CacheHandler.getCache().insert(std::pair<unsigned int, CacheDataInfo*>(hashKey, pCache));
	}while(0);

	SetDebugName(*shader, filename);
}



void BasicLoader::purgeCachedFileData()
{
    std::map<unsigned int, CacheDataInfo*>::iterator it;
    for (it = s_CacheHandler.getCache().begin(); it != s_CacheHandler.getCache().end(); it++)
    {
        CacheDataInfo* pCache = it->second;
		if(pCache){
			delete pCache;
		}
    }
    s_CacheHandler.getCache().clear();
}



