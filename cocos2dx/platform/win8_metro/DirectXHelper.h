#pragma once

#include <wrl.h>
#include <ppl.h>
#include <ppltasks.h>

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw Platform::Exception::CreateException(hr);
        }
    }

	struct ByteArray { Platform::Array<byte>^ data; };

	// function that reads from a binary file asynchronously
	inline Concurrency::task<ByteArray> ReadDataAsync(Platform::String^ filename)
	{
		using namespace Windows::Storage;
		using namespace Concurrency;
		
		auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
		
		task<StorageFile^> getFileTask(folder->GetFileAsync(filename));

		auto readBufferTask = getFileTask.then([] (StorageFile^ f) 
		{
			return FileIO::ReadBufferAsync(f);
		});

		auto byteArrayTask = readBufferTask.then([] (Streams::IBuffer^ b) -> ByteArray 
		{
			auto a = ref new Platform::Array<byte>(b->Length);
			Streams::DataReader::FromBuffer(b)->ReadBytes(a);
			ByteArray ba = { a };
			return ba;
		});

		return byteArrayTask;
	}
}