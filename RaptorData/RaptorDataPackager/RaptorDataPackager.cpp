// RaptorDataPackager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <sys/types.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#include "PackageHeader.h"

#include "zlib.h"

static bool filesOK = true;
static const unsigned int CHUNK = 16384;

static unsigned char in[CHUNK];
static unsigned char out[CHUNK];


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
    int ret, flush;
    unsigned have;
    z_stream strm;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return false;

	FILE *source = fopen(fname,"rb");
	if (source == NULL)
		return false;

	FILE *dest = fopen(outfile,"wb");
	if (dest == NULL)
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
    return ( true );
}

bool unzipfile(const char* fname, const char* outfile)
{
    int ret;
    unsigned have;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return false;

	FILE *source = fopen(fname,"rb");
	if (source == NULL)
		return false;

	FILE *dest = fopen(outfile,"wb");
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
    if (argc < 3)
    {
        printf("Usage:\n");
        printf("RaptorDataPackager [-C] <package_name> <filename_1> [filename_2] ... [filename_n]\n");
		printf("    if -C flag is provided, data will be compressed before added to the package\n");
        return -1;
    }

    PackageHeader_t header = PackageHeader_t();
    header.nbFHeaders = argc - 2;

    struct _stat packagestatus;
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
    int res = _stat(argv[argpos],&packagestatus);
    if (res >= 0)
    {
        printf("A package with the same name already exist, rename the package or move it to another place. Exiting.\n");
        clean(header);
        return -1;
    }
    else
    {
		printf("Package name: %s\n",argv[argpos]);
        header.pckName = _strdup(argv[argpos++]);
    }

    
    unsigned int offset = 0;
	unsigned int nbFHeaders = 0;
    printf("Scanning file...\n");
	while ((argpos < argc) && (nbFHeaders < header.nbFHeaders))
    {
        printf("file: %s...\n",argv[argpos]);

        struct _stat filestatus;
        res = _stat(argv[argpos],&filestatus);
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
				strcpy(zfname,argv[argpos]);
				strcat(zfname,".zip");
				if (zipfile(argv[argpos],zfname,header.compression))
				{
					res = _stat(zfname,&filestatus);
					header.fHeaders[nbFHeaders].fname = _strdup(argv[argpos]);
					header.fHeaders[nbFHeaders].fsize = filestatus.st_size;
					header.fHeaders[nbFHeaders].offset = offset;
					offset += filestatus.st_size;
					nbFHeaders++;

					/*char uzfname[_MAX_PATH];
					strcpy(uzfname,"_");
					strcat(uzfname,zfname);
					uzfname[strlen(zfname)-3] = 0;
					unzipfile(zfname,uzfname);*/
				}
				else
				{
					printf("    unknown error occured trying to compress file.\n");
					filesOK = false;
				}
			}
			else
			{
				header.fHeaders[nbFHeaders].fname = _strdup(argv[argpos]);
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
    

    int package = _open(header.pckName,_O_CREAT|_O_TRUNC|_O_WRONLY|_O_BINARY,_S_IREAD);
    if (package < 0)
    {
        printf("Failed to create package file, exiting.\n");
        clean(header);
        return -1;
    }

    printf("\nWriting package header... ");
    unsigned int intBuffer = strlen(header.pckName);
    _write(package,&intBuffer,sizeof(unsigned int));
    _write(package,header.pckName,intBuffer);
	intBuffer = header.compression;
	_write(package,&intBuffer,sizeof(unsigned int));
    intBuffer = header.nbFHeaders;
    _write(package,&intBuffer,sizeof(unsigned int));
    for (unsigned int k=0;k<header.nbFHeaders;k++)
    {
        intBuffer = strlen(header.fHeaders[k].fname);
        _write(package,&intBuffer,sizeof(unsigned int));
        _write(package,header.fHeaders[k].fname,intBuffer);
        intBuffer = header.fHeaders[k].fsize;
        _write(package,&intBuffer,sizeof(unsigned int));
        intBuffer = header.fHeaders[k].offset;
        _write(package,&intBuffer,sizeof(unsigned int));
    }
    printf("done.\n");

    printf("\nWriting package data... ");
    for (unsigned int l=0;l<header.nbFHeaders;l++)
    {
		char zfname[_MAX_PATH];
		char *fname = header.fHeaders[l].fname;
		if (header.compression != Z_NO_COMPRESSION)
		{
			strcpy(zfname,header.fHeaders[l].fname);
			strcat(zfname,".zip");
			fname = zfname;
		}

        int src = _open(fname,_O_BINARY|_O_RDONLY);
        if ( src < 0)
        {
            printf("Failed to read package data %s, exiting.\n",fname);
            clean(header);
            _close(package);
            return -1;
        }

        int rsize = CHUNK;
        while (CHUNK == (rsize = _read(src,in,CHUNK)))
            _write(package,in,CHUNK);
        _write(package,in,rsize);

        _close(src);
    }
    printf("done.\n");

    printf("\nPackage %s built successfully. Bye.\n\n",header.pckName);
    _close(package);
    clean(header);

	return 0;
}

