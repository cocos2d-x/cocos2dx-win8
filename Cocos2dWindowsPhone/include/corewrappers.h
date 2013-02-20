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

#ifndef _WRL_COREWRAPPERS_H_
#define _WRL_COREWRAPPERS_H_

#ifdef _MSC_VER
#pragma once
#endif  // _MSC_VER

#include <windows.h>
#include <intsafe.h>
#include <winstring.h>
#include <roapi.h>
#ifdef BUILD_WINDOWS
#include <winrt.h>
#endif

#include <wrl\def.h>
#include <wrl\internal.h>

namespace Microsoft {
namespace WRL {
namespace Wrappers {

namespace HandleTraits
{
// Handle specializations for implemented RAII wrappers
struct HANDLENullTraits
{
    typedef HANDLE Type;

    inline static bool Close(_In_ Type h) throw()
    {
        return ::CloseHandle(h) != FALSE;
    }

    inline static Type GetInvalidValue() throw()
    { 
        return nullptr; 
    }
};

struct HANDLETraits
{
    typedef HANDLE Type;

    inline static bool Close(_In_ Type h) throw()
    {
        return ::CloseHandle(h) != FALSE;
    }

    inline static HANDLE GetInvalidValue() throw()
    { 
        return INVALID_HANDLE_VALUE; 
    }
};

struct FileHandleTraits : HANDLETraits
{
};

struct CriticalSectionTraits
{
    typedef CRITICAL_SECTION* Type;

    inline static Type GetInvalidValue() throw() 
    { 
        return nullptr; 
    }

    inline static void Unlock(_In_ Type cs) throw()
    {
        ::LeaveCriticalSection(cs);
    }
};

struct MutexTraits : HANDLENullTraits
{
    inline static void Unlock(_In_ Type h) throw()
    {
        if (::ReleaseMutex(h) == FALSE)
        {
            // If we cannot release mutex it indicates 
            // bug in somebody code thus we raise an exception
            ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
};

struct SemaphoreTraits : HANDLENullTraits
{
    inline static void Unlock(_In_ Type h) throw()
    {
        if (::ReleaseSemaphore(h, 1, NULL) == FALSE)
        {
            // If we cannot release semaphore it indicates 
            // bug in somebody code thus we raise an exception
            ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
};

struct EventTraits : HANDLENullTraits
{
};

struct SRWLockSharedTraits
{
    typedef SRWLOCK* Type;

    inline static Type GetInvalidValue() throw()
    {
        return nullptr; 
    }

    _Releases_shared_lock_(*srwlock)
    inline static void Unlock(_In_ Type srwlock) throw()
    {
        ::ReleaseSRWLockShared(srwlock);
    }
};

struct SRWLockExclusiveTraits
{
    typedef SRWLOCK* Type;

    inline static Type GetInvalidValue() throw()
    { 
        return nullptr; 
    }

    _Releases_exclusive_lock_(*srwlock)
    inline static void Unlock(_In_ Type srwlock) throw()
    {
        ::ReleaseSRWLockExclusive(srwlock);
    }
};

} // namespace HandleTraits

// Handle object implementation specialized with traits
template <typename HandleTraits>
class HandleT
{
public:
    explicit HandleT(typename HandleTraits::Type h = HandleTraits::GetInvalidValue()) throw() : 
        handle_(h)
    {
    }

    HandleT(_Inout_ HandleT&& h) throw() : handle_(h.handle_)
    {
        h.handle_ = HandleTraits::GetInvalidValue();
    }

    ~HandleT() throw()
    {
        Close();
    }

    HandleT& operator=(_Inout_ HandleT&& h) throw()
    {
        Close();
        handle_ = h.handle_;
        h.handle_ = HandleTraits::GetInvalidValue();
        return *this;
    }

    void Attach(typename HandleTraits::Type h) throw()
    {
        if (h != handle_)
        {
            Close();
            handle_ = h;
        }
    }

    typename HandleTraits::Type Detach() throw()
    {
        typename HandleTraits::Type h = handle_;
        handle_ = HandleTraits::GetInvalidValue();
        return h;
    }

    typename HandleTraits::Type Get() const throw()
    { 
        return handle_;
    }

    void Close() throw()
    {
        if (handle_ != HandleTraits::GetInvalidValue())
        {
            bool ret = InternalClose();
            if (!ret)
            {
                // If we cannot close the handle it indicates
                // bug in somebody code thus we raise an exception
                ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));            
            }
            handle_ = HandleTraits::GetInvalidValue();
        }
    }

    bool IsValid() const throw()
    {
        return handle_ != HandleTraits::GetInvalidValue();
    }

    typename HandleTraits::Type* GetAddressOf() throw()
    {
        return &handle_;
    }

    typename HandleTraits::Type* ReleaseAndGetAddressOf() throw()
    {
        Close();
        return &handle_;
    }

    typedef HandleTraits Traits;
protected:
    virtual bool InternalClose() throw()
    {
        return HandleTraits::Close(handle_);
    }
    
    typename HandleTraits::Type handle_;
private:
    HandleT(const HandleT&);
    HandleT& operator=(const HandleT&);

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);
};

// HandleT comparation operators
template<class T> 
bool operator==(const HandleT<T>& rhs, const HandleT<T>& lhs) throw()
{
    return rhs.Get() == lhs.Get();
}

template<class T> 
bool operator==(const typename HandleT<T>::Traits::Type& lhs, const HandleT<T>& rhs) throw()
{
    return lhs == rhs.Get();
}

template<class T> 
bool operator==(const HandleT<T>& lhs, const typename HandleT<T>::Traits::Type& rhs) throw()
{
    return lhs.Get() == rhs;
}

template<class T>
bool operator!=(const HandleT<T>& lhs, const HandleT<T>& rhs) throw()
{
    return lhs.Get() != rhs.Get();
}

template<class T> 
bool operator!=(const typename HandleT<T>::Traits::Type& lhs, const HandleT<T>& rhs) throw()
{
    return lhs != rhs.Get();
}

template<class T> 
bool operator!=(const HandleT<T>& lhs, const typename HandleT<T>::Traits::Type& rhs) throw()
{
    return lhs.Get() != rhs;
}

template<class T>
bool operator<(const HandleT<T>& lhs, const HandleT<T>& rhs) throw()
{
    return lhs.Get() < rhs.Get();
}

typedef HandleT<HandleTraits::FileHandleTraits>          FileHandle;


// Forward declarations
class CriticalSection;
class Mutex;
class Semaphore;
class SRWLock;

namespace Details {

// Lock object implementation customized with traits
template <typename SyncTraits>
class SyncLockT
{
public:
    SyncLockT(_Inout_ SyncLockT&& other) throw() : sync_(other.sync_)
    {
        other.sync_ = SyncTraits::GetInvalidValue();
    }

    ~SyncLockT() throw()
    {
        InternalUnlock();
    }

    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return sync_ != SyncTraits::GetInvalidValue();
    }

    friend class CriticalSection;
    friend class SRWLock;
protected:
    explicit SyncLockT(typename SyncTraits::Type sync = SyncTraits::GetInvalidValue()) throw() : sync_(sync)
    {
    }

    typename SyncTraits::Type sync_;
private:
    // Disallow copy and assignent
    SyncLockT(const SyncLockT&);
    SyncLockT& operator=(const SyncLockT&);


    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);

    void InternalUnlock() throw()
    {
        if (IsLocked())
        {
            // Instances of this class should be used on the stack
            // and should not be passed across threads.
            // Unlock can fail if it is called from the wrong thread
            // or with an Invalid Handle, both of which are bugs
            // Traits::Unlock should raise an SEH in case it cannot 
            // release the lock

// Cannot use _Analysis_assume_lock_held_(sync) 
// because template instantiations have differing
// levels of indirection to the lock
#pragma warning(suppress:26110)                       
            SyncTraits::Unlock(sync_);
            sync_ = SyncTraits::GetInvalidValue();
        }
    }
};

// Lock object implemenatation customzed with traits
template <typename SyncTraits>
class SyncLockWithStatusT
{
public:
    SyncLockWithStatusT(_Inout_ SyncLockWithStatusT&& other) throw() : sync_(other.sync_), status_(other.status_)
    {
        other.sync_ = SyncTraits::GetInvalidValue();
    }

    ~SyncLockWithStatusT() throw()
    {
        InternalUnlock();
    }

    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return sync_ != SyncTraits::GetInvalidValue() && status_ == 0;
    }
    // status value 0 indicates success
    DWORD GetStatus() const throw()
    {
        return status_;
    }

    friend class Mutex;
    friend class Semaphore;
protected:
    explicit SyncLockWithStatusT(typename SyncTraits::Type sync, DWORD status) throw() : 
        sync_(sync), status_(status)
    {
    }

    DWORD status_;
    typename SyncTraits::Type sync_;
private:
    // Disallow copy and assignent
    SyncLockWithStatusT(const SyncLockWithStatusT&);
    SyncLockWithStatusT& operator=(const SyncLockWithStatusT&);

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);

    void InternalUnlock() throw()
    {
        if (IsLocked())
        {
            // Instances of this class should be used on the stack
            // and should not be passed across threads.
            // Unlock can fail if it is called from the wrong thread
            // or with an Invalid Handle, both of which are bugs
            // Traits::Unlock should raise an SEH in case it cannot 
            // release the lock
 
// Cannot use _Analysis_assume_lock_held_(sync) 
// because template instantiations have differing
// levels of indirection to the lock
#pragma warning(suppress:26110)            
            SyncTraits::Unlock(sync_);
            sync_ = SyncTraits::GetInvalidValue();
        }
    }
};

} // namespace Details

// Critical section implementation
class CriticalSection
{
public:
    typedef Details::SyncLockT<HandleTraits::CriticalSectionTraits> SyncLock;

    explicit CriticalSection(ULONG spincount = 0) throw()
    {
        ::InitializeCriticalSectionEx(&cs_, spincount, 0);
    }

    ~CriticalSection() throw()
    {
        ::DeleteCriticalSection(&cs_);
    }

    SyncLock Lock() throw()
    {
        return Lock(&cs_);
    }

    static SyncLock Lock(_In_ CRITICAL_SECTION* cs) throw()
    {
        ::EnterCriticalSection(cs);
        return SyncLock(cs);
    }

    SyncLock TryLock() throw()
    {
        return TryLock(&cs_);
    }

    static SyncLock TryLock(_In_ CRITICAL_SECTION* cs) throw()
    {
        if (::TryEnterCriticalSection(cs))
        {
            return SyncLock(cs);
        }
        return SyncLock();
    }

    bool IsValid() const throw()
    {
        return true;
    }
protected:
    CRITICAL_SECTION cs_;
private:
    // Disallow copy and assignent
    CriticalSection(const CriticalSection&);
    CriticalSection& operator=(const CriticalSection&);

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);
};

// Mutex handle implemenation
class Mutex : public HandleT<HandleTraits::MutexTraits>
{
public:
    typedef Details::SyncLockWithStatusT<HandleTraits::MutexTraits> SyncLock;

    explicit Mutex(HANDLE h) throw() : HandleT(h)
    {
    }

    Mutex(_Inout_ Mutex&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Mutex& operator=(_Inout_ Mutex&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }

    SyncLock Lock(DWORD milliseconds = INFINITE) throw()
    {
        return Lock(Get(), milliseconds);
    }

    static SyncLock Lock(HANDLE h, DWORD milliseconds = INFINITE) throw()
    {
        DWORD const status = ::WaitForSingleObjectEx(h, milliseconds, FALSE);
        return SyncLock(h, status == WAIT_OBJECT_0 ? 0 : status);
    }    
private:
    void Close();
    HANDLE Detach();
    void Attach(HANDLE);
    HANDLE* GetAddressOf();
    HANDLE* ReleaseAndGetAddressOf();
};

// Semaphore handle implementation
class Semaphore : public HandleT<HandleTraits::SemaphoreTraits>
{
public:
    typedef Details::SyncLockWithStatusT<HandleTraits::SemaphoreTraits> SyncLock;

    explicit Semaphore(HANDLE h) throw() : HandleT(h)
    {
    }    

    Semaphore(_Inout_ Semaphore&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Semaphore& operator=(_Inout_ Semaphore&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }

    SyncLock Lock(DWORD milliseconds = INFINITE) throw()
    {
        return Lock(Get(), milliseconds);
    }

    static SyncLock Lock(HANDLE h, DWORD milliseconds = INFINITE) throw()
    {
        DWORD const status = ::WaitForSingleObjectEx(h, milliseconds, FALSE);
        return SyncLock(h, status == WAIT_OBJECT_0 ? 0 : status);
    }
private:
    void Close();
    HANDLE Detach();
    void Attach(HANDLE);
    HANDLE* GetAddressOf();
    HANDLE* ReleaseAndGetAddressOf();
};

// Event handle implementation
class Event : public HandleT<HandleTraits::EventTraits>
{
public:
    explicit Event(HANDLE h = HandleT::Traits::GetInvalidValue()) throw() : HandleT(h)
    {
    }

    Event(_Inout_ Event&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Event& operator=(_Inout_ Event&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }
};

// SRW lock implementaion
class SRWLock
{
public:
    typedef Details::SyncLockT<HandleTraits::SRWLockExclusiveTraits> SyncLockExclusive;
    typedef Details::SyncLockT<HandleTraits::SRWLockSharedTraits> SyncLockShared;

    SRWLock() throw()
    {
        ::InitializeSRWLock(&SRWLock_);
    }

    ~SRWLock() throw()
    {
    }

    SyncLockExclusive LockExclusive() throw()
    {
        return LockExclusive(&SRWLock_);
    }

    static SyncLockExclusive LockExclusive(_In_ SRWLOCK* lock) throw()
    {
        ::AcquireSRWLockExclusive(lock);
        return SyncLockExclusive(lock);
    }

    SyncLockExclusive TryLockExclusive() throw()
    {
        return TryLockExclusive(&SRWLock_);
    }

    static SyncLockExclusive TryLockExclusive(_In_ SRWLOCK* lock) throw()
    {
        if (::TryAcquireSRWLockExclusive(lock))
        {
            return SyncLockExclusive(lock);
        }
        return SyncLockExclusive();
    }

    SyncLockShared LockShared() throw()
    {
        return LockShared(&SRWLock_);
    }

    static SyncLockShared LockShared(_In_ SRWLOCK* lock) throw()
    {
        ::AcquireSRWLockShared(lock);
        return SyncLockShared(lock);
    }

    SyncLockShared TryLockShared() throw()
    {
        return TryLockShared(&SRWLock_);
    }

    static SyncLockShared TryLockShared(_In_ SRWLOCK* lock) throw()
    {
        if (::TryAcquireSRWLockShared(lock))
        {
            return SyncLockShared(lock);
        }
        return SyncLockShared();
    }
protected:
    SRWLOCK SRWLock_;
private:
    // Disallow copy and assignent
    SRWLock(const SRWLock&);
    SRWLock& operator=(const SRWLock&);

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);
};

class HStringReference;

class HString
{
public:
    HString(_In_opt_ HSTRING hstr = nullptr) throw() : hstr_(hstr)
    {
    }

    HString(_Inout_ HString&& other) throw() : hstr_(other.hstr_)
    {
        other.hstr_ = nullptr;
    }

    ~HString() throw()
    {
        Release();
    }

    HString& operator=(_Inout_ HString&& other) throw()
    {
        hstr_ = other.hstr_;
        other.hstr_ = nullptr;
        return *this;
    }

    HRESULT Set(_In_z_ const wchar_t* str) throw()
    {
        unsigned int length;
        HRESULT hr = SizeTToUInt32(::wcslen(str), &length);
        if (SUCCEEDED(hr))
        {
            hr = Set(str, length);
        }
        return hr;
    }

    HRESULT Set(_In_z_ const wchar_t* str, unsigned int len) throw()
    {
        Release();
        return ::WindowsCreateString(str, len, &hstr_);
    }

    HRESULT Set(const HSTRING& str) throw()
    {
        Release();
        return ::WindowsDuplicateString(str, &hstr_);
    }

    void Attach(_In_opt_ HSTRING hstr) throw()
    {
        ::WindowsDeleteString(hstr_);
        hstr_ = hstr;
    }

    HSTRING Detach() throw()
    {
        HSTRING tmp = hstr_;
        hstr_ = nullptr;
        return tmp;
    }

    HSTRING* GetAddressOf() throw()
    {
        Release();
        return &hstr_;
    }

    HSTRING Get() const throw()
    {
        return hstr_;
    }

    void Release() throw()
    {
        ::WindowsDeleteString(hstr_);
        hstr_ = nullptr;
    }

    bool IsValid() const throw()
    {
        return hstr_ != nullptr;
    }

    const wchar_t* GetRawBuffer(_Out_opt_ unsigned int* length)
    {
        return WindowsGetStringRawBuffer(hstr_, length);
    }

    template<unsigned int sizeDest>
    static HStringReference MakeReference(wchar_t const (&str)[sizeDest]);

    template<unsigned int sizeDest>
    static HStringReference MakeReference(wchar_t const (&str)[sizeDest], unsigned int len);
protected:
    HSTRING hstr_;
private:
    HString(_In_ const HString&) throw();
    HString& operator=(_In_ const HString&) throw();

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);
}; 

class HStringReference
{
private:
    void CreateReference(const wchar_t* str, unsigned int bufferLen, unsigned int len)
    {
        __WRL_ASSERT__(len < bufferLen);
        if (len >= bufferLen)
        {
            len = bufferLen - 1;
        }

        HRESULT hr = ::WindowsCreateStringReference(str, len, &header_, &hstr_);
        // Failfast if developers try to create a reference to a non-NUL terminated string
        if (FAILED(hr))
        {
            ::Microsoft::WRL::Details::RaiseException(hr);
        }
    }

    HStringReference()
    {
    }

    HStringReference(const HStringReference&) throw();
    HStringReference& operator=(HStringReference&) throw();

    // can't construct with a non const value, blocked to avoid dangerous use
    HStringReference(_In_ PWSTR);

    // No new operator
    static void * operator new(size_t);

    // No delete operator
    static void operator delete(void *, size_t);

public:
    template<unsigned int sizeDest>
    HStringReference(wchar_t const (&str)[sizeDest]) throw()
    {
        CreateReference(str, sizeDest, sizeDest - 1);
    }

    template<unsigned int sizeDest>
    HStringReference(wchar_t const (&str)[sizeDest], unsigned int len) throw()
    {
        CreateReference(str, sizeDest, len);
    }

    HStringReference(_In_ PCWSTR value)
    {
        unsigned int const valueLen = static_cast<unsigned int>(wcslen(value));
        CreateReference(value, valueLen + 1, valueLen);
    }

    HStringReference(_Inout_ HStringReference&& other) throw() :
        header_(other.header_),
        hstr_(reinterpret_cast<HSTRING>(&header_)) // special fix up that assumes understanding of reference HSTRINGS
    {
        other.hstr_ = nullptr;
    }

    HStringReference& operator=(_Inout_ HStringReference&& other) throw()
    {
        header_ = other.header_;
        hstr_ = reinterpret_cast<HSTRING>(&header_); // special fix up that assumes understanding of reference HSTRINGS
        other.hstr_ = nullptr;
        return *this;
    }

    HSTRING Get() const throw()
    {
        return hstr_;
    }

    const wchar_t* GetRawBuffer(_Out_opt_ unsigned int* length)
    {
        return WindowsGetStringRawBuffer(hstr_, length);
    }

    friend class HString;

protected:
    HSTRING_HEADER header_;
    HSTRING hstr_;
};

template<unsigned int sizeDest>
inline HStringReference HString::MakeReference(wchar_t const (&str)[sizeDest]) throw()
{
    HStringReference hstringRef;
    hstringRef.CreateReference(str, sizeDest, sizeDest - 1);
    return hstringRef;
}

template<unsigned int sizeDest>
inline HStringReference HString::MakeReference(wchar_t const (&str)[sizeDest], unsigned int len) throw()
{
    HStringReference hstringRef;
    hstringRef.CreateReference(str, sizeDest, len);
    return hstringRef;
}

namespace Details {

inline INT32 CompareStringOrdinal(HSTRING lhs, HSTRING rhs)
{
    INT32 result = 0;
    // Ignore the HRESULT from the following call
    ::WindowsCompareStringOrdinal(lhs, rhs, &result);
    return  result;
}

} // namespace Detailss

// Specialization for HString
inline bool operator==(const HString& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 0;
}

inline bool operator==(const HString& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 0;
}

inline bool operator==(const HStringReference& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 0;
}

inline bool operator==(const HSTRING& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs, rhs.Get()) == 0;
}

inline bool operator==(const HString& lhs, const HSTRING& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs) == 0;
}

inline bool operator!=(const HString& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) != 0;
}

inline bool operator!=(const HStringReference& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) != 0;
}

inline bool operator!=(const HString& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) != 0;
}

inline bool operator!=(const HSTRING& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs, rhs.Get()) != 0;
}

inline bool operator!=(const HString& lhs, const HSTRING& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs) != 0;
}

inline bool operator<(const HString& lhs, const HString& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 1;
}

// Specialization for HStringReference
inline bool operator==(const HStringReference& rhs, const HStringReference& lhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 0;
}

inline bool operator==(const HSTRING& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs, rhs.Get()) == 0;
}

inline bool operator==(const HStringReference& lhs, const HSTRING& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs) == 0;
}

inline bool operator!=(const HStringReference& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) != 0;
}

inline bool operator!=(const HSTRING& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs, rhs.Get()) != 0;
}

inline bool operator!=(const HStringReference& lhs, const HSTRING& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs) != 0;
}

inline bool operator<(const HStringReference& lhs, const HStringReference& rhs) throw()
{
    return Details::CompareStringOrdinal(lhs.Get(), rhs.Get()) == 1;
}

class RoInitializeWrapper
{
    HRESULT _hr;
public:
    RoInitializeWrapper(RO_INIT_TYPE flags)
    {
        _hr = ::Windows::Foundation::Initialize(flags);
    }
    ~RoInitializeWrapper()
    {
        if (SUCCEEDED(_hr))
        {
            ::Windows::Foundation::Uninitialize();
        }
    }
    operator HRESULT()
    {
        return _hr;
    }
};

} } } // namespace ::Microsoft::WRL::Wrappers

#endif // _WRL_COREWRAPPERS_H_
