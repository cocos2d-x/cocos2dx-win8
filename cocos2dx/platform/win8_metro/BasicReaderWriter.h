//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// Define the arguments for the async callbacks.
delegate void ReadDataAsyncCallback(Platform::Array<byte>^, Windows::Foundation::AsyncStatus);
delegate void WriteDataAsyncCallback(Windows::Foundation::AsyncStatus);

ref class BasicReaderWriter
{
private:
    Windows::Storage::StorageFolder^ m_location;
    Platform::String^ m_locationPath;

public:
	internal:
    BasicReaderWriter();
	internal:
    BasicReaderWriter(
        _In_ Windows::Storage::StorageFolder^ folder
        );

    Platform::Array<byte>^ ReadData(
        _In_ Platform::String^ filename
        );

    void ReadDataAsync(
        _In_ Platform::String^ filename,
        _In_ ReadDataAsyncCallback^ callback
        );

    uint32 WriteData(
        _In_ Platform::String^ filename,
        _In_ Platform::Array<byte>^ fileData
        );

    void WriteDataAsync(
        _In_ Platform::String^ filename,
        _In_ Platform::Array<byte>^ fileData,
        _In_ WriteDataAsyncCallback^ callback
        );
};
