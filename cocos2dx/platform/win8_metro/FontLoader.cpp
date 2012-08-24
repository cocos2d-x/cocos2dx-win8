//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "DXFontCommon.h"
#include "CCFileUtils.h"
using namespace cocos2d;

using namespace Microsoft::WRL;
using namespace concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::FileProperties;
using namespace Platform::Collections;

FontLoader::FontLoader(
	_In_ IDWriteFactory* dwriteFactory
	) :
m_refCount(),
	m_fontFileCount(0),
	m_fontFileStreams(),
	m_fontFileStreamIndex(),
	m_currentFontFile(),
	m_dwriteFactory(dwriteFactory)
{
}

#include<string>
using namespace std;

//string to wstring
wstring string2wstring(string str)
{
	wstring result;

	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];

	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';

	result.append(buffer);
	delete[] buffer;
	return result;
}

//wstring to string
string wstring2string(wstring wstr)
{
	string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);

	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';

	result.append(buffer);
	delete[] buffer;
	return result;
}
void FontLoader::LoadFont(std::wstring pszFileName)
{
	for ( std::vector<std::wstring>::iterator i=m_fontNameList.begin(); i!=m_fontNameList.end(); i++ )
	{
		std::wstring tmp = *i;
		if ( tmp == pszFileName )
		{
			return;
		}
	}
	m_fontNameList.push_back(pszFileName);

	 CCFileData tmpData(wstring2string(pszFileName).c_str(),"r+b");
	 if ( &tmpData )
	 {
		 m_fontFileCount++;
	 }
	 Platform::Array<byte>^ fileData = ref new Platform::Array<byte>(tmpData.getBuffer(),tmpData.getSize());

	 ComPtr<FontFileStream> fontFileStream(new FontFileStream(fileData));
	 m_fontFileStreams.push_back(fontFileStream);
}

HRESULT STDMETHODCALLTYPE FontLoader::QueryInterface(
	REFIID uuid,
	_Outptr_ void** object
	)
{
	if (    uuid == IID_IUnknown
		||  uuid == __uuidof(IDWriteFontCollectionLoader)
		||  uuid == __uuidof(IDWriteFontFileEnumerator)
		||  uuid == __uuidof(IDWriteFontFileLoader)
		)
	{
		*object = this;
		AddRef();
		return S_OK;
	}
	else
	{
		*object = nullptr;
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE FontLoader::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
}

ULONG STDMETHODCALLTYPE FontLoader::Release()
{
	ULONG newCount = static_cast<ULONG>(InterlockedDecrement(&m_refCount));

	if (newCount == 0)
		delete this;

	return newCount;
}

//  Called by DirectWrite to create an enumerator for the fonts in the font collection.
//  The font collection key being passed in is the same key the application passes to
//  DirectWrite when calling CreateCustomFontCollection API.
//
HRESULT STDMETHODCALLTYPE FontLoader::CreateEnumeratorFromKey(
	_In_ IDWriteFactory* factory,
	_In_reads_bytes_(fontCollectionKeySize) void const* fontCollectionKey,
	uint32 fontCollectionKeySize,
	_Outptr_ IDWriteFontFileEnumerator** fontFileEnumerator
	)
{
	*fontFileEnumerator = ComPtr<IDWriteFontFileEnumerator>(this).Detach();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE FontLoader::MoveNext(OUT BOOL* hasCurrentFile)
{
	*hasCurrentFile = FALSE;

	if (m_fontFileStreamIndex < m_fontFileCount)
	{
		DX::ThrowIfFailed(
			m_dwriteFactory->CreateCustomFontFileReference(
			&m_fontFileStreamIndex,
			sizeof(size_t),
			this,
			&m_currentFontFile
			)
			);

		*hasCurrentFile = TRUE;
		++m_fontFileStreamIndex;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE FontLoader::GetCurrentFontFile(OUT IDWriteFontFile** currentFontFile)
{
	*currentFontFile = ComPtr<IDWriteFontFile>(m_currentFontFile.Get()).Detach();
	return S_OK;
}

//  Called by DirectWrite to create a font file stream. The font file reference
//  key being passed in is the same key the application passes to DirectWrite
//  when calling CreateCustomFontFileReference.
//
HRESULT STDMETHODCALLTYPE FontLoader::CreateStreamFromKey(
	_In_reads_bytes_(fontFileReferenceKeySize) void const* fontFileReferenceKey,
	uint32 fontFileReferenceKeySize,
	_Outptr_ IDWriteFontFileStream** fontFileStream
	)
{
	if (fontFileReferenceKeySize != sizeof(size_t))
	{
		return E_INVALIDARG;
	}

	size_t fontFileStreamIndex = *(static_cast<size_t const*>(fontFileReferenceKey));

	*fontFileStream = ComPtr<IDWriteFontFileStream>(m_fontFileStreams.at(fontFileStreamIndex).Get()).Detach();

	return S_OK;
}

void FontLoader::GetFontCollection(_Outptr_result_maybenull_ IDWriteFontCollection** fontCollection)
{
// 	if (m_fontCollection != nullptr)
// 	{
// 		*fontCollection = ComPtr<IDWriteFontCollection>(m_fontCollection).Detach();
// 	}
// 	else 
	{
		if (m_fontCollection != nullptr)
		{
			m_fontCollection = nullptr;
		}

		// The font collection key is simply set to zero because the loader only
		// represents a single collection of fonts used in this document.
		size_t fontCollectionKey=0;
		m_fontFileStreamIndex = 0;
		DX::ThrowIfFailed(
			m_dwriteFactory->CreateCustomFontCollection(
			this,
			&fontCollectionKey,
			sizeof(size_t)*m_fontFileCount*2,
			&m_fontCollection
			)
			);

		*fontCollection = ComPtr<IDWriteFontCollection>(m_fontCollection).Detach();
	}
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

CustomFontContext::CustomFontContext(_In_ IDWriteFactory1* dwriteFactory)
{
	m_dwriteFactory = dwriteFactory;

	m_fontLoader = new FontLoader(m_dwriteFactory);

	m_dwriteFactory->RegisterFontFileLoader(m_fontLoader);
	m_dwriteFactory->RegisterFontCollectionLoader(m_fontLoader);
}

CustomFontContext::~CustomFontContext()
{
	m_dwriteFactory->UnregisterFontCollectionLoader(m_fontLoader);
	m_dwriteFactory->UnregisterFontFileLoader(m_fontLoader);
	if ( m_fontLoader )
	{
		delete m_fontLoader;
		m_fontLoader = NULL;
	}
}

void CustomFontContext::GetFontCollection(_Outptr_result_maybenull_ IDWriteFontCollection** fontCollection)
{
	m_fontLoader->GetFontCollection(fontCollection);
}

void CustomFontContext::LoadFont(std::wstring pszFileName)
{
	m_fontLoader->LoadFont(pszFileName);
}