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

// For licensing information relating to this distribution please see Third Party Notices file.

#ifndef __PLATFOMR_CCCC_H__
#define __PLATFOMR_CCCC_H__

//
// Common layer for OpenGL stuff
//

#include "CCEGLView.h"

#define CC_GLVIEW                   cocos2d::CCEGLView
//#define ccglOrtho					glOrthof
//#define	ccglClearDepth				glClearDepthf
#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX) || defined(CC_BADA_2_0)

#else

#endif

#include "CCCommon.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "OpenGLES/ES1/gl.h"
#include "OpenGLES/ES1/glext.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
// CC_GLEXT_PROTOTYPES isn't defined in glplatform.h on android ndk r7 
// we manually define it here
#include <GLES/glplatform.h>
#ifndef CC_GLEXT_PROTOTYPES
#define CC_GLEXT_PROTOTYPES 1
#endif
// normal process
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WIN8_METRO)
//=#include <GLES/gl.h>
//=#include <GLES/glext.h>
typedef unsigned int CCenum;
typedef unsigned char CCboolean;
typedef unsigned int CCbitfield;
typedef signed char CCbyte;
typedef short CCshort;
typedef int CCint;
typedef int CCsizei;
typedef unsigned char CCubyte;
typedef unsigned short CCushort;
typedef unsigned int CCuint;
typedef float CCfloat;
typedef float CCclampf;
typedef double CCdouble;
typedef double CCclampd;
typedef void CCvoid;

#define CC_TRUE                           1
#define CC_FALSE                          0

/* TextureMagFilter */
#define CC_NEAREST                        0x2600
#define CC_LINEAR                         0x2601
#define CC_NEAREST_MIPMAP_NEAREST         0x2700
#define CC_LINEAR_MIPMAP_NEAREST          0x2701
#define CC_NEAREST_MIPMAP_LINEAR          0x2702
#define CC_LINEAR_MIPMAP_LINEAR           0x2703
#define CC_TEXTURE_MAG_FILTER             0x2800
#define CC_TEXTURE_MIN_FILTER             0x2801
#define CC_TEXTURE_WRAP_S                 0x2802
#define CC_TEXTURE_WRAP_T                 0x2803
#define CC_CLAMP                          0x2900
#define CC_REPEAT                         0x2901
#define CC_CLAMP_TO_EDGE                  0x812F

/* BlendingFactorDest */
#define CC_ZERO                           0
#define CC_ONE                            1
#define CC_SRC_COLOR                      0x0300
#define CC_ONE_MINUS_SRC_COLOR            0x0301
#define CC_SRC_ALPHA                      0x0302
#define CC_ONE_MINUS_SRC_ALPHA            0x0303
#define CC_DST_ALPHA                      0x0304
#define CC_ONE_MINUS_DST_ALPHA            0x0305
#define CC_DST_COLOR                      0x0306
#define CC_ONE_MINUS_DST_COLOR            0x0307
#define CC_SRC_ALPHA_SATURATE             0x0308

#define CC_MODELVIEW                      0x1700
#define CC_PROJECTION                     0x1701

/* AlphaFunction */
#define CC_NEVER                          0x0200
#define CC_LESS                           0x0201
#define CC_EQUAL                          0x0202
#define CC_LEQUAL                         0x0203
#define CC_GREATER                        0x0204
#define CC_NOTEQUAL                       0x0205
#define CC_GEQUAL                         0x0206
#define CC_ALWAYS                         0x0207

#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#include <GL/gl.h>
#include "GL/glext.h"

//declare here while define in CCEGLView_linux.cpp

#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
#include <IwGL.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)
#include "GLES/gl.h"
#include "GLES/glext.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

NS_CC_BEGIN;

/*
OpenGL GLU implementation
*/

//typedef float CCfloat;

/** OpenGL gluLookAt implementation */
/*
void CC_DLL gluLookAt(CCfloat fEyeX, CCfloat fEyeY, CCfloat fEyeZ,
               CCfloat fLookAtX, CCfloat fLookAtY, CCfloat fLookAtZ,
               CCfloat fUpX, CCfloat fUpY, CCfloat fUpZ);
*/
/** OpenGL gluPerspective implementation */
//void CC_DLL gluPerspective(CCfloat fovy, CCfloat aspect, CCfloat zNear, CCfloat zFar);

NS_CC_END;

#endif // __PLATFOMR_CCCC_H__
