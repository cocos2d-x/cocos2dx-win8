/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

//#include "CCDirector.h"
#include <ppltasks.h>

#include "CCCommon.h"
#include "CCString.h"

#include <wrl.h>
#include <wincodec.h>

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>

using namespace Windows::Storage;
using namespace Windows::ApplicationModel;
using namespace std;

NS_CC_BEGIN;

// record the resource path
static char s_pszResourcePath[MAX_PATH] = {0};

void _CheckPath()
{
	if (! s_pszResourcePath[0])
	{
		//WCHAR  wszPath[MAX_PATH];
		//int nNum = WideCharToMultiByte(CP_ACP, 0, wszPath, 
		//	GetCurrentDirectoryW(sizeof(wszPath), wszPath), 
		//	s_pszResourcePath, MAX_PATH, NULL, NULL);
		//      s_pszResourcePath[nNum] = '\\';

		Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
		Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
		Platform::String^ resPath = installedLocation->Path + "\\Assets\\";
		std::string pathStr = CCUnicodeToUtf8(resPath->Data());
		strcpy_s(s_pszResourcePath, pathStr.c_str());
	}
}

void CCFileUtils::setResourcePath(const char *pszResourcePath)
{
    CCAssert(pszResourcePath != NULL, "[FileUtils setResourcePath] -- wrong resource path");
    CCAssert(strlen(pszResourcePath) <= MAX_PATH, "[FileUtils setResourcePath] -- resource path too long");

    strcpy_s(s_pszResourcePath, pszResourcePath);
}

bool CCFileUtils::isFileExist(const char * resPath)
{
    _CheckPath();
    bool ret = false;
    FILE * pf = 0;
    if (resPath && strlen(resPath) && (pf = fopen(resPath, "rb")))
    {
        ret = true;
        fclose(pf);
    }
    return ret;
}

const char* CCFileUtils::fullPathFromRelativePath(const char *pszRelativePath)
{
	_CheckPath();

    CCString * pRet = new CCString();
    pRet->autorelease();
    if ((strlen(pszRelativePath) > 1 && pszRelativePath[1] == ':'))
    {
        // path start with "x:", is absolute path
        pRet->m_sString = pszRelativePath;
    }
    else if (strlen(pszRelativePath) > 0 
        && ('/' == pszRelativePath[0] || '\\' == pszRelativePath[0]))
    {
        // path start with '/' or '\', is absolute path without driver name
		char szDriver[3] = {s_pszResourcePath[0], s_pszResourcePath[1], 0};
        pRet->m_sString = szDriver;
        pRet->m_sString += pszRelativePath;
    }
    else
    {
        pRet->m_sString = s_pszResourcePath;
        pRet->m_sString += pszRelativePath;
    }
//#if (CC_IS_RETINA_DISPLAY_SUPPORTED)
//    if (CC_CONTENT_SCALE_FACTOR() != 1.0f)
//    {
//        std::string hiRes = pRet->m_sString.c_str();
//        std::string::size_type pos = hiRes.find_last_of("/\\");
//        std::string::size_type dotPos = hiRes.find_last_of(".");
//        
//        if (std::string::npos != dotPos && dotPos > pos)
//        {
//            hiRes.insert(dotPos, CC_RETINA_DISPLAY_FILENAME_SUFFIX);
//        }
//        else
//        {
//            hiRes.append(CC_RETINA_DISPLAY_FILENAME_SUFFIX);
//        }
//        DWORD attrib = GetFileAttributesA(hiRes.c_str());
//        
//        if (attrib != INVALID_FILE_ATTRIBUTES && ! (FILE_ATTRIBUTE_DIRECTORY & attrib))
//        {
//            pRet->m_sString.swap(hiRes);
//        }
//    }
//#endif
	return pRet->m_sString.c_str();
}

const char *CCFileUtils::fullPathFromRelativeFile(const char *pszFilename, const char *pszRelativeFile)
{
	_CheckPath();
	// std::string relativeFile = fullPathFromRelativePath(pszRelativeFile);
	std::string relativeFile = pszRelativeFile;
	CCString *pRet = new CCString();
	pRet->autorelease();
	pRet->m_sString = relativeFile.substr(0, relativeFile.find_last_of("/\\") + 1);
	pRet->m_sString += pszFilename;
	return pRet->m_sString.c_str();
}

unsigned char* CCFileUtils::getFileDataPlatform(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
    const char *pszPath = fullPathFromRelativePath(pszFileName);

	FILE_STANDARD_INFO fileStandardInfo = { 0 };
	HANDLE hFile;
	DWORD bytesRead = 0;
	uint32 dwFileSize = 0;
	BYTE* pBuffer = 0;
	
	std::wstring path = CCUtf8ToUnicode(pszPath);


	do 
	{
		CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0};
		extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
		extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
		extendedParams.lpSecurityAttributes = nullptr;
		extendedParams.hTemplateFile = nullptr;

		// read the file from hardware
		hFile = ::CreateFile2(path.c_str(), GENERIC_READ, 0, OPEN_EXISTING, &extendedParams);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			break;
		}


		BOOL result = ::GetFileInformationByHandleEx(
			hFile,
			FileStandardInfo,
			&fileStandardInfo,
			sizeof(fileStandardInfo)
			);

		//Read error
		if ((result == 0) || (fileStandardInfo.EndOfFile.HighPart != 0))
		{
			break;
		}

		dwFileSize = fileStandardInfo.EndOfFile.LowPart;
		//for read text
		pBuffer = new BYTE[dwFileSize+1];
		pBuffer[dwFileSize] = 0;
		if (!ReadFile(hFile, pBuffer, dwFileSize, &bytesRead, nullptr))
		{
			break;
		}
		*pSize = bytesRead;

	} while (0);

	if (! pBuffer && getIsPopupNotify())
	{
		std::string title = "Notification";
		std::string msg = "Get data from file(";
		msg.append(pszPath).append(") failed!");

		//CCMessageBox(msg.c_str(), title.c_str());
		OutputDebugString(L"CCFileUtils_win8_metro.cpp: Get data from file failed!\n");
	}

    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

	return pBuffer;
}

void CCFileUtils::setResource(const char* pszZipFileName)
{
    CC_UNUSED_PARAM(pszZipFileName);
    CCAssert(0, "Have not implement!");
}

string CCFileUtils::getWriteablePath()
{
	//return the path of Appliction LocalFolor
	std::string ret;
	StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
	Platform::String^ folderPath = localFolder->Path + "\\";
	ret = CCUnicodeToUtf8(folderPath->Data());
	return ret;
}

NS_CC_END;
