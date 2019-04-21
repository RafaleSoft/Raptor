/***************************************************************************/
/*                                                                         */
/*  PackageHeader.h                                                        */
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


#ifndef __PACKAGE_HEADER__
#define __PACKAGE_HEADER__

typedef struct PackageFileHeader_t
{
    char *fname;
    unsigned int fsize;
    unsigned int offset;
} PackageFileHeader;

typedef struct PackageHeader_t
{
    char *pckName;
	unsigned int compression;
    unsigned int nbFHeaders;
    PackageFileHeader_t *fHeaders;
} PackageHeader;


#endif