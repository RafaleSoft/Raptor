// RaptorDataPackager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <sys/types.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "PackageHeader.h"

#include "zlib.h"

static bool filesOK = true;
static const unsigned int CHUNK = 16384;

static unsigned char in[CHUNK];
static unsigned char out[CHUNK];


#if defined(WIN32)
	#include <io.h>
	#include <share.h>
	static FILE *msdn_fopen(const char *filename,const char *mode)
	{
		if ((NULL == filename) || (NULL == mode))
			return NULL;
		FILE* pFile = NULL;
		errno_t err = fopen_s(&pFile,filename,mode);
		if (0 == err)
			return pFile;
		else
			return NULL;
	}
	static char *msdn_strcpy(char * destination, const char * source, size_t sizeInBytes)
	{
		errno_t err = strcpy_s(destination,sizeInBytes,source);
		if (0 == err)
			return destination;
		else
			return NULL;
	}
	static char *msdn_strcat(char * destination, const char * source, size_t sizeInBytes)
	{
		errno_t err = strcat_s(destination,sizeInBytes,source);
		if (0 == err)
			return destination;
		else
			return NULL;
	}
	static int msdn_open(const char *filename,int oflag,int pmode)
	{
		int pfh = -1;
		errno_t err = _sopen_s(&pfh,filename,oflag,_SH_DENYRW,pmode);
		if (0 == err)
			return pfh;
		else
			return -1;
	}
	#define FOPEN(a,b) msdn_fopen(a,b)
	#define STRCPY(a,b,s) msdn_strcpy(a,b,s)
	#define STRCAT(a,b,s) msdn_strcat(a,b,s)
	#define OPEN(f,o,m) msdn_open(f,o,m)
	#define STAT _stat
	#define STRDUP _strdup
	#define CLOSE(f) _close(f)
	#define WRITE(a,b,c) _write(a,b,c)
	#define READ(a,b,c) _read(a,b,c)
	#define O_BINARY _O_BINARY
	#define O_CREAT _O_CREAT
	#define O_TRUNC _O_TRUNC
	#define O_RDONLY _O_RDONLY
	#define O_WRONLY _O_WRONLY
	#define S_IREAD _S_IREAD
	#define S_IWRITE _S_IWRITE
#elif defined(LINUX)
	#include <linux/limits.h>
	#define FOPEN(a,b) fopen(a,b)
	#define STRCPY(a,b,s) strcpy(a,b)
	#define STRCAT(a,b,s) strcat(a,b)
	#define OPEN(f,o,m) open(f,o,m)
	#define STAT stat
	#define STRDUP strdup
	#define CLOSE(f) close(f)
	#define WRITE(a,b,c) write(a,b,c)
	#define READ(a,b,c) read(a,b,c)
	#define O_BINARY 0
	#define _MAX_PATH PATH_MAX
#endif

void clean(PackageHeader_t &header)
{
	if (NULL != header.fHeaders)
	{
		for (unsigned int j = 0; j < header.nbFHeaders; j++)
		{
			PackageFileHeader &fHeader = header.fHeaders[j];
			if (NULL != fHeader.fname)
				free(fHeader.fname);
		}

		delete[] header.fHeaders;
		header.fHeaders = 0;
	}

	header.nbFHeaders = 0;
	header.compression = 0;

	if (NULL != header.pckName)
		free(header.pckName);
	header.pckName = 0;
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

	FILE *dest = FOPEN(outfile,"wb");
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
		} 
		while (strm.avail_out == 0);

		if (strm.avail_in != 0)     /* all input will be used */
			return false;

	/* done when last data in file processed */
	} 
	while (flush != Z_FINISH);

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

	FILE *source = FOPEN(fname,"rb");
	if (source == NULL)
		return false;

	FILE *dest = FOPEN(outfile,"wb");
	if (dest == NULL)
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
		} 
		while (strm.avail_out == 0);

	/* done when inflate() says it's done */
	} 
	while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);

	fclose(source);
	fclose(dest);

	return (ret == Z_STREAM_END);
}

int main(int argc, char* argv[])
{
	printf("Welcone to the simple Raptor Zip Data Packager...\n\n");

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

