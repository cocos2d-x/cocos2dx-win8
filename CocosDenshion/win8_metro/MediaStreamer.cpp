//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DirectXHelper.h"
#include "MediaStreamer.h"

using namespace Windows::ApplicationModel;

MediaStreamer::MediaStreamer()
{
    m_reader = nullptr;
    m_audioType = nullptr;
    ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));

    m_installedLocation = Package::Current->InstalledLocation;
    m_installedLocationPath = Platform::String::Concat(m_installedLocation->Path, "\\Assets\\");
}

MediaStreamer::~MediaStreamer()
{
}

void MediaStreamer::Initialize(__in const WCHAR* url)
{
    Microsoft::WRL::ComPtr<IMFMediaType> outputMediaType;
    Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
  
    DX::ThrowIfFailed(
        MFStartup(MF_VERSION)
        );

	WCHAR filePath[MAX_PATH] = {0};
	if ((wcslen(url) > 1 && url[1] == ':'))
	{
		// path start with "x:", is absolute path
		wcscat(filePath, url);
	}
	else if (wcslen(url) > 0 
		&& (L'/' == url[0] || L'\\' == url[0]))
	{
		// path start with '/' or '\', is absolute path without driver name
		wcscat(filePath, m_installedLocationPath->Data());
		// remove '/' or '\\'
		wcscat(filePath, (const WCHAR*)url[1]);
	}else
	{
		wcscat(filePath, m_installedLocationPath->Data());
		wcscat(filePath, url);
	}

    DX::ThrowIfFailed(
        MFCreateSourceReaderFromURL(filePath, nullptr, &m_reader)
        );

    // Set the decoded output format as PCM
    // XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
    // When using MF, this sample always decodes into PCM.

    DX::ThrowIfFailed(
        MFCreateMediaType(&mediaType)
        );

    DX::ThrowIfFailed(
        mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)
        );

    DX::ThrowIfFailed(
        mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)
        );

    DX::ThrowIfFailed(
        m_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, mediaType.Get())
        );

    // Get the complete WAVEFORMAT from the Media Type
    DX::ThrowIfFailed(
        m_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &outputMediaType)
        );

    uint32 formatSize = 0;
    WAVEFORMATEX* waveFormat;
    DX::ThrowIfFailed(
        MFCreateWaveFormatExFromMFMediaType(outputMediaType.Get(), &waveFormat, &formatSize)
        );
    CopyMemory(&m_waveFormat, waveFormat, sizeof(m_waveFormat));
    CoTaskMemFree(waveFormat);

    // Get the total length of the stream in bytes
    PROPVARIANT var;
    DX::ThrowIfFailed(
        m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var)
        );
    LONGLONG duration = var.uhVal.QuadPart;
    double durationInSeconds = (duration / static_cast<double>(10000000)); // duration is in 100ns units, convert to seconds
    m_maxStreamLengthInBytes = static_cast<unsigned int>(durationInSeconds * m_waveFormat.nAvgBytesPerSec);

    // Round up the buffer size to the nearest four bytes
    m_maxStreamLengthInBytes = (m_maxStreamLengthInBytes + 3) / 4 * 4;
}

bool MediaStreamer::GetNextBuffer(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength)
{
    Microsoft::WRL::ComPtr<IMFSample> sample;
    Microsoft::WRL::ComPtr<IMFMediaBuffer> mediaBuffer;
    BYTE *audioData = nullptr;
    DWORD sampleBufferLength = 0;
    DWORD flags = 0;

    *bufferLength = 0;

    if (m_reader == nullptr)
    {
        return false;
    }

    DX::ThrowIfFailed(
        m_reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample)
        );

    if (sample == nullptr)
    {
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DX::ThrowIfFailed(
        sample->ConvertToContiguousBuffer(&mediaBuffer)
        );

    DX::ThrowIfFailed(
        mediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength)
        );

    // If buffer isn't large enough, dump sample
    if (sampleBufferLength <= maxBufferSize)
    {
        CopyMemory(buffer, audioData, sampleBufferLength);
        *bufferLength = sampleBufferLength;
    }
    else
    {
        OutputDebugString(L"Sample buffer dumped");
    }
    
    if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MediaStreamer::ReadAll(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength)
{
    uint32 valuesWritten = 0;
    uint32 sampleBufferLength = 0;

    if (m_reader == nullptr)
    {
        return;
    }

    *bufferLength = 0;
    // If buffer isn't large enough, return
    if (maxBufferSize < m_maxStreamLengthInBytes)
    {
        return;
    }
 
    while (!GetNextBuffer(buffer + valuesWritten, maxBufferSize - valuesWritten, &sampleBufferLength))
    {
        valuesWritten += sampleBufferLength;
    }

    *bufferLength = valuesWritten + sampleBufferLength;
}

void MediaStreamer::Restart()
{
    if (m_reader == nullptr)
    {
        return;
    }

    PROPVARIANT var = {0};
    var.vt = VT_I8;

    DX::ThrowIfFailed(
        m_reader->SetCurrentPosition(GUID_NULL, var)
    );
}