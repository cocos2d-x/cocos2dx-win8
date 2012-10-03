/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
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

#include <Box2D/Common/b2Settings.h>
#include "pch.h"

#ifdef SHP
#include <FBaseSys.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#else
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#endif


using namespace std;


b2Version b2_version = {2, 2, 1};

// Memory allocators. Modify these to use your own allocator.
void* b2Alloc(int32 size)
{
	return malloc(size);
}

void b2Free(void* mem)
{
	free(mem);
}

// You can modify this to use your logging facility.
void b2Log(const char* string, ...)
{
#if defined(SHP)
	#ifdef _DEBUG
	__App_info(__PRETTY_FUNCTION__ , __LINE__, string);
	#endif
#else
	va_list args;
	va_start(args, string);
	vprintf(string, args);
	va_end(args);
#endif
}
