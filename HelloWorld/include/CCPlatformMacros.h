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

#ifndef __CC_PLATFORM_MACROS_H__
#define __CC_PLATFORM_MACROS_H__

/**
 * define some platform specific macros
 */
#include "ccConfig.h"
#include "CCPlatformConfig.h"

#define MacGLView					void
#define NSWindow					        void


/**
 * define a create function for a specific type, such as CCLayer
 * @__TYPE__ class type to add create(), such as CCLayer
 */
#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

/** @def CC_ENABLE_CACHE_TEXTTURE_DATA
Enable it if you want to cache the texture data.
Basically,it's only enabled in android

It's new in cocos2d-x since v0.99.5
*/
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #define CC_ENABLE_CACHE_TEXTTURE_DATA       1
#else
    #define CC_ENABLE_CACHE_TEXTTURE_DATA       0
#endif


// generic macros

// namespace cocos2d {}
#define NS_CC_BEGIN                     namespace cocos2d {
#define NS_CC_END                       }
#define USING_NS_CC                     using namespace cocos2d

/** CC_PROPERTY_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public virtual function, you should rewrite it first.
 The variables and methods declared after CC_PROPERTY_READONLY are all public.
 If you need protected or private, please declare.
 */
#define CC_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

#define CC_PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);

/** CC_PROPERTY is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public virtual functions, you should rewrite them first.
 The variables and methods declared after CC_PROPERTY are all public.
 If you need protected or private, please declare.
 */
#define CC_PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

#define CC_PROPERTY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);\
public: virtual void set##funName(const varType& var);

/** CC_SYNTHESIZE_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public inline function.
 The variables and methods declared after CC_SYNTHESIZE_READONLY are all public.
 If you need protected or private, please declare.
 */
#define CC_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define CC_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }

/** CC_SYNTHESIZE is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public  inline functions.
 The variables and methods declared after CC_SYNTHESIZE are all public.
 If you need protected or private, please declare.
 */
#define CC_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

#define CC_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }

#define CC_SYNTHESIZE_RETAIN(varType, varName, funName)    \
protected: varType varName; \
public: virtual varType get##funName(void) const { return varName; } \
public: virtual void set##funName(varType var)   \
{ \
    CC_SAFE_RETAIN(var); \
    CC_SAFE_RELEASE(varName); \
    varName = var; \
} 

#define CC_SAFE_DELETE(p)			if(p) { delete (p); (p) = 0; }
#define CC_SAFE_DELETE_ARRAY(p)    if(p) { delete[] (p); (p) = 0; }
#define CC_SAFE_FREE(p)			if(p) { free(p); (p) = 0; }
#define CC_SAFE_RELEASE(p)			if(p) { (p)->release(); }
#define CC_SAFE_RELEASE_NULL(p)	if(p) { (p)->release(); (p) = 0; }
#define CC_SAFE_RETAIN(p)			if(p) { (p)->retain(); }
#define CC_BREAK_IF(cond)			if(cond) break;
//dx!!!!
#define CC_SAFE_RELEASE_NULL_DX(p)	if(p) { (p)->Release(); (p) = 0; }

// cocos2d debug
#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define CCLOG(...)              
#define CCLOGINFO(...)         
#define CCLOGERROR(...)         

#elif COCOS2D_DEBUG == 1
#define CCLOG(format, ...)      cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   do {} while (0)

#elif COCOS2D_DEBUG > 1
#define CCLOG(format, ...)      cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   cocos2d::CCLog(format, ##__VA_ARGS__)
#endif // COCOS2D_DEBUG

// Lua engine debug
#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0 || CC_LUA_ENGINE_DEBUG == 0
#define LUALOG(...)
#else
#define LUALOG(format, ...)     cocos2d::CCLog(format, ##__VA_ARGS__)
#endif // Lua engine debug

// shared library declartor
#define CC_DLL 

#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
// assertion
#include <assert.h>
#define CC_ASSERT(cond)                assert(cond)
#else
// bada platform

#include <FBaseConfig.h>
#include <FBaseSys.h>

#undef CC_DLL
#define CC_DLL  _EXPORT_

#include "CCPlatformFunc_bada.h"

#ifdef _DEBUG
#define CC_ASSERT(cond)  (void)( (!!(cond)) || (badaAssert(__PRETTY_FUNCTION__ , __LINE__ , #cond),0) )
#else
#define CC_ASSERT(cond)  void(0)
#endif /* _DEBUG */
#endif

#define CC_UNUSED_PARAM(unusedparam)   (void)unusedparam

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#undef CC_UNUSED_PARAM
#define CC_UNUSED_PARAM(unusedparam)   //unusedparam
#endif


// platform depended macros

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)

    #undef CC_DLL
    #if defined(CC_EXPORT_WINDOWS)
        #define CC_DLL     __declspec(dllexport)
    #else 		/* use a DLL library */
        #define CC_DLL     __declspec(dllimport)
    #endif

#endif  // CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE && defined(_TRANZDA_VM_))

    #undef CC_DLL
    #if defined(SS_MAKEDLL)
        #define CC_DLL     __declspec(dllexport)
    #else 		/* use a DLL library */
        #define CC_DLL     __declspec(dllimport)
    #endif

#endif  // wophone VM


/*
 * only certain compilers support __attribute__((deprecated))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define CC_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
    #define CC_DEPRECATED_ATTRIBUTE __declspec(deprecated) 
#else
    #define CC_DEPRECATED_ATTRIBUTE
#endif 

#endif // __CC_PLATFORM_MACROS_H__
