/***************************************************************************/
/*                                                                         */
/*  PackageIO.cpp                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/



#include "StdAfx.h"
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "PackageIO.h"


static const unsigned int CHUNK = 16384;
static unsigned char in[CHUNK];
static unsigned char out[CHUNK];


#if defined(WIN32)
	FILE *msdn_fopen(const char *filename, const char *mode)
	{
		if ((NULL == filename) || (NULL == mode))
			return NULL;
		FILE* pFile = NULL;
		errno_t err = fopen_s(&pFile, filename, mode);
		if (0 == err)
			return pFile;
		else
			return NULL;
	}
	char *msdn_strcpy(char * destination, const char * source, size_t sizeInBytes)
	{
		errno_t err = strcpy_s(destination, sizeInBytes, source);
		if (0 == err)
			return destination;
		else
			return NULL;
	}
	char *msdn_strcat(char * destination, const char * source, size_t sizeInBytes)
	{
		errno_t err = strcat_s(destination, sizeInBytes, source);
		if (0 == err)
			return destination;
		else
			return NULL;
	}
	int msdn_open(const char *filename, int oflag, int pmode)
	{
		int pfh = -1;
		errno_t err = _sopen_s(&pfh, filename, oflag, _SH_DENYRW, pmode);
		if (0 == err)
			return pfh;
		else
			return -1;
	}
#endif



void clean(PackageHeader_t &PH)
{
	if (NULL != PH.fHeaders)
	{
		for (unsigned int j = 0; j < PH.nbFHeaders; j++)
		{
			PackageFileHeader &fHeader = PH.fHeaders[j];
			if (NULL != fHeader.fname)
				free(fHeader.fname);
		}

		delete[] PH.fHeaders;
		PH.fHeaders = 0;
	}

	PH.nbFHeaders = 0;
	PH.compression = 0;

	if (NULL != PH.pckName)
		free(PH.pckName);
	PH.pckName = 0;
}

bool copyFile(int dstFile, int srcFile, size_t fSize)
{
	//!	Check fSize valid and file descriptors valid (0,1,2 are for standard I/O).
	if ((0 == fSize) || (dstFile < 3) || (srcFile < 3))
		return false;

	//! TODO : check files are valid.

	size_t tsize = fSize;
	int rsize = (tsize > CHUNK) ? CHUNK : tsize;
	while ((tsize > 0) && (rsize == READ(srcFile, in, rsize)))
	{
		WRITE(dstFile, in, rsize);
		tsize -= rsize;
		rsize = (tsize > CHUNK) ? CHUNK : tsize;
	}

	return true;
}

bool zipfile(const char* fname, const char* outfile, int level)
{
	int flush = Z_NO_FLUSH;
	unsigned int have = 0;

	/* allocate deflate state */
	z_stream strm;
	memset(&strm, 0, sizeof(z_stream));
	int ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return false;

	FILE *source = FOPEN(fname, "rb");
	if (NULL == source)
		return false;

	FILE *dest = FOPEN(outfile, "wb");
	if (NULL == dest)
		return false;

	/* compress until end of file */
	do
	{
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source))
		{
			(void)deflateEnd(&strm);
			return false;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */

			if (ret == Z_STREAM_ERROR)
				return false;

			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				(void)deflateEnd(&strm);
				return false;
			}
		} while (strm.avail_out == 0);

		if (strm.avail_in != 0)     /* all input will be used */
			return false;

		/* done when last data in file processed */
	} while (flush != Z_FINISH);

	if (ret != Z_STREAM_END)        /* stream will be complete */
		return false;

	/* clean up and return */
	(void)deflateEnd(&strm);

	ret = fclose(source);
	ret = fclose(dest);
	return true;
}


bool unzipfile(const char* fname, const char* outfile)
{
	unsigned int have = 0;

	/* allocate inflate state */
	z_stream strm;
	memset(&strm, 0, sizeof(z_stream));

	int ret = inflateInit(&strm);
	if (ret != Z_OK)
		return false;

	FILE *source = NULL;
#if defined(WIN32)
	if (0 != fopen_s(&source, fname, "rb"))
		return false;
#elif defined(LINUX)
	source = fopen(fname, "rb");
#endif
	if (NULL == source)
		return false;

	FILE *dest = NULL;
#if defined(WIN32)
	if (0 != fopen_s(&dest, outfile, "wb"))
		return false;
#elif defined(LINUX)
	dest = fopen(outfile, "wb");
#endif
	if (NULL == dest)
		return false;

	/* decompress until deflate stream ends or end of file */
	do
	{
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source))
		{
			(void)inflateEnd(&strm);
			return false;
		}

		if (strm.avail_in == 0)
			break;

		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);

			if (ret == Z_STREAM_ERROR)  /* state not clobbered */
				return false;

			switch (ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return false;
			}

			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				(void)inflateEnd(&strm);
				return false;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);

	fclose(source);
	fclose(dest);

	return (ret == Z_STREAM_END);
}

