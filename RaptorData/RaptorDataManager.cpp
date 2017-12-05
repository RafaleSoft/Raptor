// RaptorDataManager.cpp: implementation of the CRaptorDataManager class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
#include "RaptorDataManager.h"
#endif

#ifndef __PACKAGE_HEADER__
    #include "RaptorDataPackager/PackageHeader.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32)
	#include <io.h>
	#define CLOSE(a)		_close(a)
	#define READ(a,b,c)		_read(a,b,c)
	#define WRITE(a,b,c)	_write(a,b,c)
	#define LSEEK(a,b,c)	_lseek(a,b,c)
	#define TELL(a)			_tell(a)
	#if _MSC_VER > 1200 
		#define OPEN(a,b,c)		_open(a,b,c)
		#define STRDUP(s)		_strdup(s)
		#define STRCAT(a,b)		strcat_s(a,b)
	#else
		#define OPEN(a,b,c)		_open(a,b,c)
		#define STRDUP(s)		strdup(s)
		#define STRCAT(a,b)		strcat(a,b)
	#endif
#elif defined(LINUX)
	#include <stdio.h>
	#include <sys/io.h>
	#define _O_BINARY		0
	#define _O_RDONLY		O_RDONLY
	#define _O_CREAT		O_CREAT
	#define _O_TRUNC		O_TRUNC
	#define _O_WRONLY		O_WRONLY
	#define _S_IREAD		S_IRUSR
	#define _S_IWRITE		S_IWUSR
	#define OPEN(a,b,c)		open(a,b,c)
	#define CLOSE(a)		close(a)
	#define READ(a,b,c)		read(a,b,c)
	#define WRITE(a,b,c)	write(a,b,c)
	#define STRDUP(s)		strdup(s)
	#define STRCAT(a,b)		strcat(a,b)
	#define LSEEK(a,b,c)	lseek(a,b,c)
	#define TELL(a)			tell(a)
#endif



//////////////////////////////////////////////////////////////////////
//
// RaptorData Shaders
//
//////////////////////////////////////////////////////////////////////
extern const char *embm_vp;
extern const char *embm_fp;
extern const char *embm_vp_nolight;
extern const char *embm_fp_nolight;
extern const char *shadowmap_fp;
extern const char *shadowmap_fp_pcf;
extern const char *shadowmap_fp_pcf_4x;
#if _MSC_VER > 1200 
extern const char *shadowmap_fp_pcf_16x;
#endif
extern const char *blender_8x_fp;
extern const char *blender_8x_Xvp;
extern const char *blender_8x_Yvp;



//////////////////////////////////////////////////////////////////////
//
// Compressed embedded files
//
//////////////////////////////////////////////////////////////////////
#include "zlib.h"

static bool doCompress = false;
static const unsigned int CHUNK = 16384;
static const unsigned int BUFFER = 1024;
static const int NB_SHADERS = 64;
static unsigned char in[CHUNK];
static unsigned char out[CHUNK];

//////////////////////////////////////////////////////////////////////
//
// RaptorData Binary Files
//
//////////////////////////////////////////////////////////////////////
static PackageHeader header;
static int package = 0;
static long packageheadpos = 0;

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

	FILE *source = NULL;
#if defined(WIN32)
	if (0 != fopen_s(&source,fname,"rb"))
		return false;
#elif defined(LINUX)
	 source = fopen(fname,"rb");
#endif
	if (NULL == source)
		return false;

	FILE *dest = NULL;
#if defined(WIN32)
	if (0 != fopen_s(&dest,outfile,"wb"))
		return false;
#elif defined(LINUX)
	 dest = fopen(outfile,"wb");
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorDataManager  *CRaptorDataManager::m_pInstance = NULL;

CRaptorDataManager::CRaptorDataManager()
{

}

CRaptorDataManager::~CRaptorDataManager()
{
    m_pInstance = NULL;
    if (package > 0)
        CLOSE(package);
    package = 0;
	packageheadpos = 0;
	clean(header);
}

CRaptorDataManager  *CRaptorDataManager::GetInstance(void)
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CRaptorDataManager();
    }

    return m_pInstance;
}

void CRaptorDataManager::ClearExports()
{
	if (package == 0)
        openPackage(getPackPath());
    if (package != 0)
	{
#if defined(WIN32)
		char buffer[BUFFER];
		GetEnvironmentVariable("TMP",buffer,BUFFER);
		std::string filename = buffer;
#elif defined(LINUX)
		std::string filename = "/tmp";
#endif
		for (unsigned int k=0;k<header.nbFHeaders;k++)
		{
			std::string fname = filename + "/" + header.fHeaders[k].fname;
			int res = _unlink(fname.data());

			if (header.compression != Z_NO_COMPRESSION)
			{
				fname = fname + ".zip";
				res = _unlink(fname.data());
			}
		}
	}
}

bool CRaptorDataManager::openPackage(const std::string& pakName)
{
#if _MSC_VER > 1200 
	errno_t err = _sopen_s(&package,pakName.c_str(),_O_BINARY|_O_RDONLY,_SH_DENYWR,_S_IREAD);
	if ((err != 0) || (package < 0))
		return false;
#else
    package = OPEN(pakName.c_str(),_O_BINARY|_O_RDONLY,_S_IREAD);
    if (package < 0)
        return false;
#endif

    char byteBuffer[BUFFER];
    memset(byteBuffer,0,BUFFER);
    unsigned int intBuffer = 0;
	packageheadpos = 0;

    int nbr = READ(package,&intBuffer,sizeof(unsigned int));
    nbr = READ(package,byteBuffer,intBuffer);
	byteBuffer[BUFFER-1] = 0;
    header.pckName = STRDUP(byteBuffer);
	packageheadpos += sizeof(unsigned int) + intBuffer;

	nbr = READ(package,&intBuffer,sizeof(unsigned int));
	header.compression = intBuffer;
	packageheadpos += sizeof(unsigned int);

    nbr = READ(package,&intBuffer,sizeof(unsigned int));
    header.nbFHeaders = intBuffer;
    header.fHeaders = new PackageFileHeader_t[intBuffer];
	packageheadpos += sizeof(unsigned int);

    for (unsigned int k=0;k<header.nbFHeaders;k++)
    {
        nbr = READ(package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].fname = (char*)malloc(intBuffer+1);
		if (header.fHeaders[k].fname == NULL)
			return false;
        nbr = READ(package,header.fHeaders[k].fname,intBuffer);
        header.fHeaders[k].fname[intBuffer] = 0;
		packageheadpos += sizeof(unsigned int) + intBuffer;

        nbr = READ(package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].fsize = intBuffer;
		packageheadpos += sizeof(unsigned int);

        nbr = READ(package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].offset = intBuffer;
		packageheadpos += sizeof(unsigned int);
    }

    return true;
}

std::string CRaptorDataManager::getPackPath()
{
	char buffer[BUFFER];
	std::string pakpath = "./RaptorData.pck";
#if defined(WIN32)
    DWORD pakPath = GetEnvironmentVariable("RAPTOR_ROOT",buffer,BUFFER);
    if (pakPath > 0)
    {
        pakpath = buffer;
		string::size_type st = pakpath.find("\"");
		while (string::npos != st)
		{
			pakpath.erase(st,1);
			st = pakpath.find("\"");
		}
        pakpath += "/Redist/Bin/RaptorData.pck";
    }
#elif defined(LINUX)
	char *pakPath = getenv("RAPTOR_ROOT");
    if (pakPath != NULL)
    {
        pakpath = pakPath;
        pakpath += "/Redist/Bin/RaptorData.pck";
    }
#endif

	return pakpath;
}

std::string CRaptorDataManager::ExportFile(const std::string& fname,
										   const std::string& path)
{
    std::string filename = path;
	char buffer[BUFFER];
    if (path.empty())
    {
#if defined(WIN32)
		GetEnvironmentVariable("TMP",buffer,BUFFER);
        filename = buffer;
#elif defined(LINUX)
		filename = "/tmp";
#endif
    }

    filename += "/";
    filename += fname;

	int dst = 0;
#if _MSC_VER > 1200 
	errno_t err = _sopen_s(&dst,filename.c_str(),_O_RDONLY|_O_BINARY,_SH_DENYWR,_S_IREAD);
	if ((err == 0) && (dst > 0))
	{
		CLOSE(dst);
		return filename;
	}
#else
    dst = OPEN(filename.c_str(),_O_RDONLY|_O_BINARY,_S_IREAD);
    if (dst > 0)
	{
		CLOSE(dst);
        return filename;
	}
#endif

    bool res = true;
    if (package == 0)
        res = openPackage(getPackPath());
    if (!res)
        return "";

    res = false;
    PackageFileHeader_t *fHeader = NULL;
    for (unsigned int i=0; !res && (i<header.nbFHeaders) ; i++)
    {
        fHeader = &header.fHeaders[i];
        res = (0 == fname.compare(fHeader->fname));
    }
    if (!res)
        return "";


	string tmpfname = (header.compression != Z_NO_COMPRESSION) ? 
						filename + ".zip" : filename;
	dst = 0;
#if _MSC_VER > 1200 
	err = _sopen_s(&dst,tmpfname.c_str(),_O_CREAT|_O_TRUNC|_O_WRONLY|_O_BINARY,_SH_DENYWR,_S_IWRITE);
	if ((err != 0) || (dst < 0))
		return "";
#else
    dst = OPEN(tmpfname.c_str(),_O_CREAT|_O_TRUNC|_O_WRONLY|_O_BINARY,_S_IWRITE);
    if ( dst < 0)
        return "";
#endif

    long pos = LSEEK(package,fHeader->offset + packageheadpos, SEEK_SET);
    if (pos == -1L)
        return "";

    int tsize = fHeader->fsize;
	int rsize = (tsize > CHUNK) ? CHUNK : tsize;
    while ((tsize > 0) && (rsize == READ(package,in,rsize)))
    {
        WRITE(dst,in,rsize);
        tsize -= rsize;
		rsize = (tsize > CHUNK) ? CHUNK : tsize;
    }

    CLOSE(dst);

	if (header.compression != Z_NO_COMPRESSION)
		unzipfile(tmpfname.c_str(),filename.c_str());

    return filename;
}

std::string CRaptorDataManager::readFile(const std::string &fname)
{
    int fileHandle = 0;

#if _MSC_VER > 1200 
	errno_t err = _sopen_s(&fileHandle,fname.c_str(),_O_BINARY|_O_RDONLY,_SH_DENYWR,_S_IREAD);
	if ((err != 0) || (fileHandle < 0))
		return "";
#else
    fileHandle = OPEN(fname.c_str(),_O_BINARY|_O_RDONLY,_S_IREAD);
    if (fileHandle < 0)
        return "";
#endif

	long fpos = LSEEK(fileHandle,0, SEEK_END);
    if (fpos == -1L)
        return "";

    if (LSEEK(fileHandle,0,SEEK_SET))
    {
        CLOSE(fileHandle);
        return "";
    }

    char *code = new char[fpos+1];
    memset(code,0,fpos+1);
    size_t readSize = 0;
    if (fpos == (readSize=READ(fileHandle,code,fpos)))
		return std::string(code);
	else
		return"";
}

#define LOAD_SHADER(file,shader_name) \
	fname = ExportFile(file); \
	if (!fname.empty()) \
	{ \
		std::string shader = readFile(fname); \
		if (shaders[ns] == NULL) \
			shaders[ns++] = STRDUP(shader.c_str()); \
		shaders[ns++] = shader_name; \
	}

const char * const *CRaptorDataManager::GetShaderList(void) 
{
    static const char * shaders[NB_SHADERS];
	memset(shaders,0,NB_SHADERS);

	int ns = 0;
	std::string fname = "";

	// BUMP LIGHTING
	LOAD_SHADER("bump_0light.vp","BUMP_0LIGHT_VTX_SHADER")
	LOAD_SHADER("bump_0light.fp","BUMP_0LIGHT_TEX_SHADER")
	LOAD_SHADER("bump.vp","BUMP_VTX_SHADER")
	LOAD_SHADER("bump.fp","BUMP_TEX_SHADER")
	LOAD_SHADER("bump_att.vp","BUMP_ATT_VTX_SHADER")
	LOAD_SHADER("bump_att.fp","BUMP_ATT_TEX_SHADER")
	LOAD_SHADER("bump_att_2light.vp","BUMP_ATT_VTX_SHADER_2LIGHTS")
	LOAD_SHADER("bump_att_2light.fp","BUMP_ATT_TEX_SHADER_2LIGHTS")
	LOAD_SHADER("bump_att_3light.vp","BUMP_ATT_VTX_SHADER_3LIGHTS")
	LOAD_SHADER("bump_att_3light.fp","BUMP_ATT_TEX_SHADER_3LIGHTS")

	// EMBM LIGHTING
	LOAD_SHADER("embm_0light.vp","EMBM_0LIGHT_VTX_SHADER")
	LOAD_SHADER("embm_0light.fp","EMBM_0LIGHT_TEX_SHADER")
	LOAD_SHADER("embm.vp","EMBM_TEX_SHADER")
	LOAD_SHADER("embm.fp","EMBM_VTX_SHADER")

	// SHADOWS
	LOAD_SHADER("projection.fp","PROJECTION_TEX_SHADER")
	LOAD_SHADER("shadowmap.fp","SHADOWMAP_TEX_SHADER")
	LOAD_SHADER("shadowmap_pcf.fp","SHADOWMAP_TEX_SHADER_PCF")
	LOAD_SHADER("shadowmap_pcf_4x.fp","SHADOWMAP_TEX_SHADER_PCF_4X")
	LOAD_SHADER("shadowmap_pcf_16x.fp","SHADOWMAP_TEX_SHADER_PCF_16X")
    
	//BLENDERS 
	LOAD_SHADER("blender_8x.fp","BLENDER_8X")
	LOAD_SHADER("blenderX_8x.vp","BLENDER_8X_XOFFSETS")
	LOAD_SHADER("blenderY_8x.vp","BLENDER_8X_YOFFSETS")    

	// BLINN LIGHTING
	LOAD_SHADER("blinn.vs","PPIXEL_BLINN_VTX_PROGRAM")
	LOAD_SHADER("blinn.ps","PPIXEL_BLINN_TEX_PROGRAM")

	// PHONG LIGHTING
	LOAD_SHADER("phong.vs","PPIXEL_PHONG_VTX_PROGRAM")
	LOAD_SHADER("phong.ps","PPIXEL_PHONG_TEX_PROGRAM")
	// BUMP LIGHTING
	LOAD_SHADER("bump.vs","PPIXEL_BUMP_VTX_PROGRAM")
	LOAD_SHADER("bump.ps","PPIXEL_BUMP_TEX_PROGRAM")
	// AMBIENT OCCLUSION
	LOAD_SHADER("AO.vs","AMBIENT_OCCLUSION_VTX_PROGRAM")
	LOAD_SHADER("AO.ps","AMBIENT_OCCLUSION_TEX_PROGRAM")

	return shaders;
}

