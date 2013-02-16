// For licensing information relating to this distribution please see Third Party Notices file.

#pragma once

#include "BasicReaderWriter.h"

ref class BasicLoader
{
public:
	internal:
    BasicLoader(
        _In_ ID3D11Device* d3dDevice
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );


    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11PixelShader** shader
        );

	static void purgeCachedFileData();

   
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    BasicReaderWriter^ m_basicReaderWriter;

    template<class DeviceChildType>
    inline void SetDebugName(
        _In_ DeviceChildType* object,
        _In_ Platform::String^ name
        );

    Platform::String^ GetExtension(
        _In_ Platform::String^ filename
        );

    void CreateInputLayout(
        _In_reads_bytes_(bytecodeSize) byte* bytecode,
        _In_ uint32 bytecodeSize,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC* layoutDesc,
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11InputLayout** layout
        );

};
