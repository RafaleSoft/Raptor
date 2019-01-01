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