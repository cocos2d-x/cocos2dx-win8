//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

class FontFileStream;

//  This class implements the font collection and font file loader interfaces
//  for DirectWrite to call to load document/application-specific font to the
//  system. The font loader's lifetime is tied to the factory it's registered to.
class FontLoader :  public IDWriteFontCollectionLoader,
	public IDWriteFontFileEnumerator,
	public IDWriteFontFileLoader
{
public:
	FontLoader(
		_In_ IDWriteFactory* dwriteFactory
		);

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID uuid,
		_Outptr_ void** object
		) override;

	virtual ULONG STDMETHODCALLTYPE AddRef() override;

	virtual ULONG STDMETHODCALLTYPE Release() override;

	// IDWriteFontCollectionLoader
	virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
		_In_ IDWriteFactory* factory,
		_In_reads_bytes_(fontCollectionKeySize) void const* fontCollectionKey,
		uint32 fontCollectionKeySize,
		_Outptr_ IDWriteFontFileEnumerator** fontFileEnumerator
		) override;

	// IDWriteFontFileEnumerator
	virtual HRESULT STDMETHODCALLTYPE MoveNext(OUT BOOL* hasCurrentFile) override;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(OUT IDWriteFontFile** currentFontFile) override;

	// IDWriteFontFileLoader
	virtual HRESULT STDMETHODCALLTYPE CreateStreamFromKey(
		_In_reads_bytes_(fontFileReferenceKeySize) void const* fontFileReferenceKey,
		uint32 fontFileReferenceKeySize,
		_Outptr_ IDWriteFontFileStream** fontFileStream
		) override;
	
	void LoadFont(std::wstring pszFileName);
	void GetFontCollection(_Outptr_result_maybenull_ IDWriteFontCollection** fontCollection);
private:
	// Reference counter
	ULONG m_refCount;

	// Number of font files in the storage location
	size_t m_fontFileCount;

	// A list of font file streams loaded
	std::vector<Microsoft::WRL::ComPtr<FontFileStream>> m_fontFileStreams;

	// Index to the current font file stream in the loaded list
	size_t m_fontFileStreamIndex;

	// Current DirectWrite font file being indexed
	Microsoft::WRL::ComPtr<IDWriteFontFile> m_currentFontFile;

	// DirectWrite factory
	Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;

	// Collection of all document fonts
	Microsoft::WRL::ComPtr<IDWriteFontCollection> m_fontCollection;

	std::vector<std::wstring> m_fontNameList;
};


class CustomFontContext
{
public:
	CustomFontContext(_In_ IDWriteFactory1* dwriteFactory);
	~CustomFontContext();
	void GetFontCollection(_Outptr_result_maybenull_ IDWriteFontCollection** fontCollection);
	void LoadFont(std::wstring pszFileName);
private:
	// Not copyable or assignable.
	CustomFontContext(CustomFontContext const&);
	void operator=(CustomFontContext const&);

	IDWriteFactory * m_dwriteFactory;
	FontLoader* m_fontLoader;
};
