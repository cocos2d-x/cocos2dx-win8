/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2011      Zynga Inc.
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

#ifndef __CCMACROS_H__
#define __CCMACROS_H__

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "CCCommon.h"
#include "CCStdC.h"

#ifndef CCAssert
#define CCAssert(cond, msg)         CC_ASSERT(cond)
#endif  // CCAssert

#include "ccConfig.h"

/** @def CC_SWAP
simple macro that swaps 2 variables
*/
#define CC_SWAP(x, y, type)	\
{	type temp = (x);		\
	x = y; y = temp;		\
}


/** @def CCRANDOM_MINUS1_1
 returns a random float between -1 and 1
 */
#define CCRANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

/** @def CCRANDOM_0_1
 returns a random float between 0 and 1
 */
#define CCRANDOM_0_1() ((float)rand()/RAND_MAX)

/** @def CC_DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define CC_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def CC_RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define CC_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180
#define kCCRepeatForever (UINT_MAX -1)
/** @def CC_BLEND_SRC
default gl blend src function. Compatible with premultiplied alpha images.
*/
#if CC_OPTIMIZE_BLEND_FUNC_FOR_PREMULTIPLIED_ALPHA
    #define CC_BLEND_SRC CC_ONE
    #define CC_BLEND_DST CC_ONE_MINUS_SRC_ALPHA
#else
    #define CC_BLEND_SRC CC_SRC_ALPHA
    #define CC_BLEND_DST CC_ONE_MINUS_SRC_ALPHA
#endif // ! CC_OPTIMIZE_BLEND_FUNC_FOR_PREMULTIPLIED_ALPHA

/** @def CC_BLEND_DST
 default gl blend dst function. Compatible with premultiplied alpha images.
 */
#define CC_BLEND_DST CC_ONE_MINUS_SRC_ALPHA

/** @def CC_ENABLE_DEFAULT_CC_STATES
 GL states that are enabled:
	- CC_TEXTURE_2D
	- CC_VERTEX_ARRAY
	- CC_TEXTURE_COORD_ARRAY
	- CC_COLOR_ARRAY

#define CC_ENABLE_DEFAULT_CC_STATES() {				\
	glEnableClientState(CC_VERTEX_ARRAY);			\
	glEnableClientState(CC_COLOR_ARRAY);			\
	glEnableClientState(CC_TEXTURE_COORD_ARRAY);	\
	glEnable(CC_TEXTURE_2D);						\
}
 */
/** @def CC_DISABLE_DEFAULT_CC_STATES 
 Disable default GL states:
	- CC_TEXTURE_2D
	- CC_VERTEX_ARRAY
	- CC_TEXTURE_COORD_ARRAY
	- CC_COLOR_ARRAY

#define CC_DISABLE_DEFAULT_CC_STATES() {			\
	glDisable(CC_TEXTURE_2D);						\
	glDisableClientState(CC_COLOR_ARRAY);			\
	glDisableClientState(CC_TEXTURE_COORD_ARRAY);	\
	glDisableClientState(CC_VERTEX_ARRAY);			\
}
 */
#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif // FLT_EPSILON

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	        TypeName(const TypeName&);\
			void operator=(const TypeName&)

/**
@since v0.99.5
@todo upto-0.99.5 check the code  for retina
*/
#if CC_IS_RETINA_DISPLAY_SUPPORTED

/****************************/
/** RETINA DISPLAY ENABLED **/
/****************************/
/** @def CC_CONTENT_SCALE_FACTOR
On Mac it returns 1;
On iPhone it returns 2 if RetinaDisplay is On. Otherwise it returns 1
*/
#define CC_CONTENT_SCALE_FACTOR() CCDirector::sharedDirector()->getContentScaleFactor()

/** @def CC_RECT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define CC_RECT_PIXELS_TO_POINTS(__rect_in_pixels__)                                                                        \
    CCRectMake( (__rect_in_pixels__).origin.x / CC_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).origin.y / CC_CONTENT_SCALE_FACTOR(),    \
            (__rect_in_pixels__).size.width / CC_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).size.height / CC_CONTENT_SCALE_FACTOR() )

/** @def CC_RECT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define CC_RECT_POINTS_TO_PIXELS(__rect_in_points_points__)                                                                        \
    CCRectMake( (__rect_in_points_points__).origin.x * CC_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).origin.y * CC_CONTENT_SCALE_FACTOR(),    \
            (__rect_in_points_points__).size.width * CC_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).size.height * CC_CONTENT_SCALE_FACTOR() )

/** @def CC_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define CC_POINT_PIXELS_TO_POINTS(__pixels__)                                                                        \
CCPointMake( (__pixels__).x / CC_CONTENT_SCALE_FACTOR(), (__pixels__).y / CC_CONTENT_SCALE_FACTOR())

/** @def CC_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define CC_POINT_POINTS_TO_PIXELS(__points__)                                                                        \
CCPointMake( (__points__).x * CC_CONTENT_SCALE_FACTOR(), (__points__).y * CC_CONTENT_SCALE_FACTOR())

/** @def CC_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define CC_SIZE_PIXELS_TO_POINTS(__size_in_pixels__)                                                                        \
CCSizeMake( (__size_in_pixels__).width / CC_CONTENT_SCALE_FACTOR(), (__size_in_pixels__).height / CC_CONTENT_SCALE_FACTOR())

/** @def CC_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define CC_SIZE_POINTS_TO_PIXELS(__size_in_points__)                                                                        \
CCSizeMake( (__size_in_points__).width * CC_CONTENT_SCALE_FACTOR(), (__size_in_points__).height * CC_CONTENT_SCALE_FACTOR())

#else // retina disabled

/*****************************/
/** RETINA DISPLAY DISABLED **/
/*****************************/

#define CC_CONTENT_SCALE_FACTOR() 1
#define CC_RECT_PIXELS_TO_POINTS(__pixels__) __pixels__
#define CC_RECT_POINTS_TO_PIXELS(__points__) __points__

#endif

/**********************/
/** Profiling Macros **/
/**********************/
#if CC_ENABLE_PROFILERS

#define CC_PROFILER_DISPLAY_TIMERS() CCProfiler::sharedProfiler()->displayTimers()
#define CC_PROFILER_PURGE_ALL() CCProfiler::sharedProfiler()->releaseAllTimers()

#define CC_PROFILER_START(__name__) CCProfilingBeginTimingBlock(__name__)
#define CC_PROFILER_STOP(__name__) CCProfilingEndTimingBlock(__name__)
#define CC_PROFILER_RESET(__name__) CCProfilingResetTimingBlock(__name__)

#define CC_PROFILER_START_CATEGORY(__cat__, __name__) do{ if(__cat__) CCProfilingBeginTimingBlock(__name__); } while(0)
#define CC_PROFILER_STOP_CATEGORY(__cat__, __name__) do{ if(__cat__) CCProfilingEndTimingBlock(__name__); } while(0)
#define CC_PROFILER_RESET_CATEGORY(__cat__, __name__) do{ if(__cat__) CCProfilingResetTimingBlock(__name__); } while(0)

#define CC_PROFILER_START_INSTANCE(__id__, __name__) do{ CCProfilingBeginTimingBlock( [NSString stringWithFormat:@"%08X - %@", __id__, __name__] ); } while(0)
#define CC_PROFILER_STOP_INSTANCE(__id__, __name__) do{ CCProfilingEndTimingBlock(    [NSString stringWithFormat:@"%08X - %@", __id__, __name__] ); } while(0)
#define CC_PROFILER_RESET_INSTANCE(__id__, __name__) do{ CCProfilingResetTimingBlock( [NSString stringWithFormat:@"%08X - %@", __id__, __name__] ); } while(0)


#else

#define CC_PROFILER_DISPLAY_TIMERS() do {} while (0)
#define CC_PROFILER_PURGE_ALL() do {} while (0)

#define CC_PROFILER_START(__name__)  do {} while (0)
#define CC_PROFILER_STOP(__name__) do {} while (0)
#define CC_PROFILER_RESET(__name__) do {} while (0)

#define CC_PROFILER_START_CATEGORY(__cat__, __name__) do {} while(0)
#define CC_PROFILER_STOP_CATEGORY(__cat__, __name__) do {} while(0)
#define CC_PROFILER_RESET_CATEGORY(__cat__, __name__) do {} while(0)

#define CC_PROFILER_START_INSTANCE(__id__, __name__) do {} while(0)
#define CC_PROFILER_STOP_INSTANCE(__id__, __name__) do {} while(0)
#define CC_PROFILER_RESET_INSTANCE(__id__, __name__) do {} while(0)

#endif



/**
Helper marcos which converts 4-byte little/big endian 
integral number to the machine native number representation
 
It should work same as apples CFSwapInt32LittleToHost(..)
*/

/// when define returns true it means that our architecture uses big endian
#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define CC_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define CC_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define CC_SWAP_INT32_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP32(i) : (i) )
#define CC_SWAP_INT16_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP16(i) : (i) )
#define CC_SWAP_INT32_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i) : CC_SWAP32(i) )
#define CC_SWAP_INT16_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i):  CC_SWAP16(i) )


/** @def CC_INCREMENT_GL_DRAWS_BY_ONE
 Increments the GL Draws counts by one.
 The number of calls per frame are displayed on the screen when the CCDirector's stats are enabled.
 */
extern unsigned int CC_DLL g_uNumberOfDraws;
#define CC_INCREMENT_GL_DRAWS(__n__) g_uNumberOfDraws += __n__

/*******************/
/** Notifications **/
/*******************/
/** @def CCAnimationFrameDisplayedNotification
 Notification name when a CCSpriteFrame is displayed
 */
#define CCAnimationFrameDisplayedNotification "CCAnimationFrameDisplayedNotification"

#endif // __CCMACROS_H__