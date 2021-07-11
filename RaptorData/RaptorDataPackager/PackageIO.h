/***************************************************************************/
/*                                                                         */
/*  PackageIO.h                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __PACKAGE_IO__
#define __PACKAGE_IO__

#ifndef __PACKAGE_HEADER__
	#include "PackageHeader.h"
#endif


#include <stdio.h>

#if defined(WIN32)

	//! System headers	
	#include <io.h>
	#include <share.h>

	//! Flags
	#define O_BINARY _O_BINARY
	#define O_CREAT _O_CREAT
	#define O_TRUNC _O_TRUNC
	#define O_RDONLY _O_RDONLY
	#define O_WRONLY _O_WRONLY
	#define S_IREAD _S_IREAD
	#define S_IWRITE _S_IWRITE
	
	//! File functions
	int msdn_open(const char *filename, int oflag, int pmode);
	FILE *msdn_fopen(const char *filename, const char *mode);
	
	#define STAT _stat
	#define LSEEK(a,b,c)	_lseek(a,b,c)
	#define TELL(a)			_tell(a)
	#define FOPEN(a,b)		msdn_fopen(a,b)
	#define OPEN(a,b,c)		msdn_open(a,b,c)
	#define READ(a,b,c)		_read(a,b,c)
	#define WRITE(a,b,c)	_write(a,b,c)
	#define CLOSE(a)		_close(a)
	#define UNLINK(a)		_unlink(a)

	//! String functions
#if _MSC_VER > 1200
	char *msdn_strcpy(char * destination, const char * source, size_t sizeInBytes);
	char *msdn_strcat(char * destination, const char * source, size_t sizeInBytes);

	#define STRDUP(s)		_strdup(s)
	#define STRCPY(a,b,s)	msdn_strcpy(a,b,s)
	#define STRCAT(a,b,s)	msdn_strcat(a,b,s)
#else
	#define STRDUP(s)		strdup(s)
	#define STRCAT(a,b,s)	strcat(a,b)
#endif

#elif defined(LINUX)

	//! System headers	
	#include <sys/io.h>
	#include <unistd.h>
	#include <string.h>
	#include <linux/limits.h>
	
	//! Flags
	#define O_BINARY 0
	#define _MAX_PATH PATH_MAX
	
	//! File Functions
	#define STAT stat
	#define FOPEN(a,b)		fopen(a,b)
	#define OPEN(a,b,c)		open(a,b,c)
	#define LSEEK(a,b,c)	lseek(a,b,c)
	#define TELL(a)			tell(a)
	#define READ(a,b,c)		read(a,b,c)
	#define WRITE(a,b,c)	write(a,b,c)
	#define CLOSE(a)		close(a)
	#define UNLINK(a)		unlink(a)

	//! String functions
	#define STRDUP(s)		strdup(s)
	#define STRCAT(a,b,s)	strcat(a,b)
	#define STRCPY(a,b,s)	strcpy(a,b)

#endif



//!	Release all allocated resources for the package header PH.
void clean(PackageHeader_t &PH);

//!	Copy fSize from src file to dstfile.
bool copyFile(int dstFile, int srcFile, size_t fSize);

//!	zip a file with requested compression level.
bool zipfile(const char* fname, const char* outfile, int level);

//!	unzip a file with zlib.
bool unzipfile(const char* fname, const char* outfile);


#endif // __PACKAGE_IO__

