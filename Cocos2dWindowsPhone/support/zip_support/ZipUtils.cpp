/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* (see each file to see the different copyright owners)
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

#include "pch.h"
#include <zlib.h>
#include <assert.h>
#include <stdlib.h>

#include "ZipUtils.h"
#include "ccMacros.h"
#include "CCFileUtils.h"

namespace cocos2d
{
	// memory in iPhone is precious
	// Should buffer factor be 1.5 instead of 2 ?
    #define BUFFER_INC_FACTOR (2)

	int ZipUtils::ccInflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint)
	{
		/* ret value */
		int err = Z_OK;

		int bufferSize = outLenghtHint;
		*out = new unsigned char[bufferSize];

		z_stream d_stream; /* decompression stream */	
		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;

		d_stream.next_in  = in;
		d_stream.avail_in = inLength;
		d_stream.next_out = *out;
		d_stream.avail_out = bufferSize;

		/* window size to hold 256k */
		if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
			return err;

		for (;;) 
		{
			err = inflate(&d_stream, Z_NO_FLUSH);

			if (err == Z_STREAM_END)
			{
				break;
			}

			switch (err) 
			{
			case Z_NEED_DICT:
				err = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&d_stream);
				return err;
		    }

			// not enough memory ?
			if (err != Z_STREAM_END) 
			{
                delete [] *out;
                *out = new unsigned char[bufferSize * BUFFER_INC_FACTOR];

				/* not enough memory, ouch */
				if (! *out ) 
				{
					CCLOG("cocos2d: ZipUtils: realloc failed");
					inflateEnd(&d_stream);
					return Z_MEM_ERROR;
				}

				d_stream.next_out = *out + bufferSize;
				d_stream.avail_out = bufferSize;
				bufferSize *= BUFFER_INC_FACTOR;
			}
		}

		*outLength = bufferSize - d_stream.avail_out;
		err = inflateEnd(&d_stream);
		return err;
	}

	int ZipUtils::ccInflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int outLengthHint)
	{
		unsigned int outLength = 0;
		int err = ccInflateMemoryWithHint(in, inLength, out, &outLength, outLengthHint);

		if (err != Z_OK || *out == NULL) {
			if (err == Z_MEM_ERROR)
			{
				CCLOG("cocos2d: ZipUtils: Out of memory while decompressing map data!");
			} else 
			if (err == Z_VERSION_ERROR)
			{
				CCLOG("cocos2d: ZipUtils: Incompatible zlib version!");
			} else 
			if (err == Z_DATA_ERROR)
			{
				CCLOG("cocos2d: ZipUtils: Incorrect zlib compressed data!");
			}
			else
			{
				CCLOG("cocos2d: ZipUtils: Unknown error while decompressing map data!");
			}

			delete[] *out;
			*out = NULL;
			outLength = 0;
		}

		return outLength;
	}

	int ZipUtils::ccInflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out)
	{
		// 256k for hint
		return ccInflateMemoryWithHint(in, inLength, out, 256 * 1024);
	}

	int ZipUtils::ccInflateGZipFile(const char *path, unsigned char **out)
	{
		int len;
		unsigned int offset = 0;

		CCAssert(out, "");
		CCAssert(&*out, "");

		gzFile inFile = gzopen(path, "rb");
		if( inFile == NULL ) {
			CCLOG("cocos2d: ZipUtils: error open gzip file: %s", path);
			return -1;
		}

		/* 512k initial decompress buffer */
		unsigned int bufferSize = 512 * 1024;
		unsigned int totalBufferSize = bufferSize;

		*out = (unsigned char*)malloc( bufferSize );
		if( ! out ) 
		{
			CCLOG("cocos2d: ZipUtils: out of memory");
			return -1;
		}

		for (;;) {
			len = gzread(inFile, *out + offset, bufferSize);
			if (len < 0) 
			{
				CCLOG("cocos2d: ZipUtils: error in gzread");
				free( *out );
				*out = NULL;
				return -1;
			}
			if (len == 0)
			{
				break;
			}

			offset += len;

			// finish reading the file
			if( (unsigned int)len < bufferSize )
			{
				break;
			}

			bufferSize *= BUFFER_INC_FACTOR;
			totalBufferSize += bufferSize;
			unsigned char *tmp = (unsigned char*)realloc(*out, totalBufferSize );

			if( ! tmp ) 
			{
				CCLOG("cocos2d: ZipUtils: out of memory");
				free( *out );
				*out = NULL;
				return -1;
			}

			*out = tmp;
		}

		if (gzclose(inFile) != Z_OK)
		{
			CCLOG("cocos2d: ZipUtils: gzclose failed");
		}

		return offset;
	}

	int ZipUtils::ccInflateCCZFile(const char *path, unsigned char **out)
	{
 		CCAssert(out, "");
 		CCAssert(&*out, "");
 
 		// load file into memory
 		unsigned char *compressed = NULL;
        
        int fileLen = 0;
        compressed = CCFileUtils::getFileData(path, "rb", (unsigned long *)(&fileLen));
 		// int fileLen  = CCFileUtils::ccLoadFileIntoMemory( path, &compressed );

 		if( fileLen < 0 ) 
 		{
 			CCLOG("cocos2d: Error loading CCZ compressed file");
            return -1;
 		}
 
 		struct CCZHeader *header = (struct CCZHeader*) compressed;
 
 		// verify header
 		if( header->sig[0] != 'C' || header->sig[1] != 'C' || header->sig[2] != 'Z' || header->sig[3] != '!' ) 
 		{
 			CCLOG("cocos2d: Invalid CCZ file");
 			delete [] compressed;
 			return -1;
 		}
 
 		// verify header version
 		unsigned int version = CC_SWAP_INT16_BIG_TO_HOST( header->version );
 		if( version > 2 ) 
 		{
 			CCLOG("cocos2d: Unsupported CCZ header format");
 			delete [] compressed;
 			return -1;
 		}
 
 		// verify compression format
 		if( CC_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB ) 
 		{
 			CCLOG("cocos2d: CCZ Unsupported compression method");
 			delete [] compressed;
 			return -1;
 		}
 
 		unsigned int len = CC_SWAP_INT32_BIG_TO_HOST( header->len );
 
 		*out = (unsigned char*)malloc( len );
 		if(! *out )
 		{
 			CCLOG("cocos2d: CCZ: Failed to allocate memory for texture");
 			delete [] compressed;
 			return -1;
 		}
 
 
 		unsigned long destlen = len;
 		unsigned long source = (unsigned long) compressed + sizeof(*header);
 		int ret = uncompress(*out, &destlen, (Bytef*)source, fileLen - sizeof(*header) );
 
 		delete [] compressed;
 
 		if( ret != Z_OK )
 		{
 			CCLOG("cocos2d: CCZ: Failed to uncompress data");
 			free( *out );
 			*out = NULL;
 			return -1;
 		}
  
 		return len;
	}

} // end of namespace cocos2d
