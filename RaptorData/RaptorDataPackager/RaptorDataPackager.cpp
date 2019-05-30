/***************************************************************************/
/*                                                                         */
/*  RaptorDataPackager.cpp                                                 */
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


#include "stdafx.h"

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "PackageIO.h"

#include "zlib.h"


static bool filesOK = true;
static const unsigned int CHUNK = 16384;

static unsigned char in[CHUNK];
static unsigned char out[CHUNK];



int main(int argc, char* argv[])
{
	printf("Welcome to the simple Raptor Zip Data Packager...\n\n");

	if (argc < 3)
	{
		printf("Usage:\n");
		printf("RaptorDataPackager [-C] <package_name> <filename_1> [filename_2] ... [filename_n]\n");
		printf("    if -C flag is provided, data will be compressed before added to the package\n");
		return -1;
	}

	PackageHeader_t header = PackageHeader_t();
	header.nbFHeaders = argc - 2;

	struct STAT packagestatus;
	int argpos = 1;

	if (!strcmp(argv[argpos],"-C") || !strcmp(argv[argpos],"-c"))
	{
		printf("Package will be compressed.\n");
		header.compression = Z_BEST_COMPRESSION;
		argpos++;
		header.nbFHeaders--;
	}

	header.fHeaders = new PackageFileHeader_t[header.nbFHeaders];
	for (unsigned int j=0;j<header.nbFHeaders;j++)
		header.fHeaders[j].fname = NULL;
	
	header.pckName = NULL;
	int res = STAT(argv[argpos],&packagestatus);
	if (res >= 0)
	{
		printf("A package with the same name already exist, rename the package or move it to another place. Exiting.\n");
		clean(header);
		return -1;
	}
	else
	{
		printf("Package name: %s\n",argv[argpos]);
		header.pckName = STRDUP(argv[argpos++]);
	}

    
	unsigned int offset = 0;
	unsigned int nbFHeaders = 0;
	printf("Scanning file...\n");
	while ((argpos < argc) && (nbFHeaders < header.nbFHeaders))
	{
		printf("file: %s...\n",argv[argpos]);

		struct STAT filestatus;
		res = STAT(argv[argpos],&filestatus);
		if (res < 0)
		{
			if (errno == ENOENT)
				printf("    file does not exits, check paths.\n");
			else
				printf("    unknown error occured attempting to access file.\n");
			filesOK = false;
		}
		else
		{
			if (header.compression != Z_NO_COMPRESSION)
			{
				char zfname[_MAX_PATH];
				memset(zfname,0,_MAX_PATH);
				STRCPY(zfname,argv[argpos],_MAX_PATH);
				STRCAT(zfname,".zip",_MAX_PATH);
				if (zipfile(argv[argpos],zfname,header.compression))
				{
					res = STAT(zfname,&filestatus);
					header.fHeaders[nbFHeaders].fname = STRDUP(argv[argpos]);
					header.fHeaders[nbFHeaders].fsize = filestatus.st_size;
					header.fHeaders[nbFHeaders].offset = offset;
					offset += filestatus.st_size;
					nbFHeaders++;

					//char uzfname[_MAX_PATH];
					//strcpy(uzfname,"_");
					//strcat(uzfname,zfname);
					//uzfname[strlen(zfname)-3] = 0;
					//unzipfile(zfname,uzfname);
				}
				else
				{
					printf("    unknown error occured trying to compress file.\n");
					filesOK = false;
				}
			}
			else
			{
				header.fHeaders[nbFHeaders].fname = STRDUP(argv[argpos]);
				header.fHeaders[nbFHeaders].fsize = filestatus.st_size;
				header.fHeaders[nbFHeaders].offset = offset;
				offset += filestatus.st_size;
				nbFHeaders++;
			}
		}
		argpos++;
	}
	
	header.nbFHeaders = nbFHeaders;

	if (!filesOK)
	{
		printf("\nErrors found, exiting.\n");
		clean(header);
		return -1;
	}

	printf("\nCreating package %s...\n",header.pckName);
    

	int package = OPEN(header.pckName,O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,S_IREAD);
	if (package < 0)
	{
		printf("Failed to create package file, exiting.\n");
		clean(header);
		return -1;
	}

	printf("\nWriting package header... ");
	unsigned int intBuffer = strlen(header.pckName);
	WRITE(package,&intBuffer,sizeof(unsigned int));
	WRITE(package,header.pckName,intBuffer);
	intBuffer = header.compression;
	WRITE(package,&intBuffer,sizeof(unsigned int));
	intBuffer = header.nbFHeaders;
	WRITE(package,&intBuffer,sizeof(unsigned int));
	for (unsigned int k=0;k<header.nbFHeaders;k++)
	{
		intBuffer = strlen(header.fHeaders[k].fname);
		WRITE(package,&intBuffer,sizeof(unsigned int));
		WRITE(package,header.fHeaders[k].fname,intBuffer);
		intBuffer = header.fHeaders[k].fsize;
		WRITE(package,&intBuffer,sizeof(unsigned int));
		intBuffer = header.fHeaders[k].offset;
		WRITE(package,&intBuffer,sizeof(unsigned int));
	}
	printf("done.\n");

	printf("\nWriting package data... ");
	for (unsigned int l=0;l<header.nbFHeaders;l++)
	{
		char zfname[_MAX_PATH];
		char *fname = header.fHeaders[l].fname;
		if (header.compression != Z_NO_COMPRESSION)
		{
			STRCPY(zfname,header.fHeaders[l].fname,_MAX_PATH);
			STRCAT(zfname,".zip",_MAX_PATH);
			fname = zfname;
		}

		int src = OPEN(fname,O_BINARY|O_RDONLY,S_IWRITE);
		if ( src < 0)
		{
			printf("Failed to read package data %s, exiting.\n",fname);
			clean(header);
			CLOSE(package);
			return -1;
		}

		int rsize = CHUNK;
		while (CHUNK == (rsize = READ(src,in,CHUNK)))
			WRITE(package,in,CHUNK);
		WRITE(package,in,rsize);

		CLOSE(src);
	}
	printf("done.\n");

	printf("\nPackage %s built successfully. Bye.\n\n",header.pckName);
	CLOSE(package);
	clean(header);

	return 0;
}

