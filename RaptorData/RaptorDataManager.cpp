/***************************************************************************/
/*                                                                         */
/*  RaptorDataManager.cpp                                                  */
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

#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "RaptorDataManager.h"
#endif

#ifndef __PACKAGE_HEADER__
    #include "RaptorDataPackager/PackageHeader.h"
#endif

#ifndef __PACKAGE_IO__
	#include "RaptorDataPackager/PackageIO.h"
#endif


#include <fcntl.h>
#include <sys/stat.h>



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
static const unsigned int BUFFER = 1024;
static const int NB_SHADERS = 64;


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

	for (size_t i = 0; i < m_packages.size(); i++)
	{
		Package& p = m_packages[i];
		if (p.package > 0)
			CLOSE(p.package);
		p.package = 0;
		p.headerSize = 0;
		PackageHeader_t *h = (PackageHeader_t*)p.header;
		clean(*h);
		p.header = NULL;
		delete h;
	}

	m_packages.clear();
}

CRaptorDataManager  *CRaptorDataManager::GetInstance(void)
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CRaptorDataManager();
		m_pInstance->managePackage("RaptorData.pck");
    }

    return m_pInstance;
}

bool CRaptorDataManager::managePackage(const std::string& packName)
{
	if (packName.length() < 4)
		return false;

	//	Check if not already opened or conflicting name.
	for (size_t i = 0; i < m_packages.size(); i++)
	{
		Package_t &pack = m_packages[i];
		PackageHeader_t *pHeader = (PackageHeader_t*)pack.header;
		if (NULL == pHeader)
			return false;

		std::string pckName = pHeader->pckName;
		if (packName == pckName)
			return false;
	}

	Package_t pack;
	pack.packPath = getPackPath(packName);
	pack.package = 0;
	pack.header = NULL;
	pack.headerSize = 0;

	if (openPackage(pack))
	{
		m_packages.push_back(pack);
		//	Erase previous files in case of updates
		return ClearExports(packName);
	}
	else
		return false;
}

bool CRaptorDataManager::ClearExports(const std::string& packName)
{
	for (size_t i = 0; i < m_packages.size(); i++)
	{
		Package_t &pack = m_packages[i];
		PackageHeader_t *pHeader = (PackageHeader_t*)pack.header;
		if (NULL == pHeader)
			continue;

		std::string pckName = pHeader->pckName;
		if (packName == pckName)
		{
			if (0 == pack.package)
				return false;

#if defined(WIN32)
			char buffer[BUFFER];
			GetEnvironmentVariable("TMP", buffer, BUFFER);
			std::string filename = buffer;
#elif defined(LINUX)
			std::string filename = "/tmp";
#endif

			PackageHeader_t &header = *pHeader;

			for (unsigned int k = 0; k<header.nbFHeaders; k++)
			{
				std::string fname = filename + "/" + header.fHeaders[k].fname;
				int res = UNLINK(fname.data());

				if (header.compression != Z_NO_COMPRESSION)
				{
					fname = fname + ".zip";
					res = UNLINK(fname.data());
				}
			}

			return true;
		}
	}

	return false;
}

bool CRaptorDataManager::openPackage(Package_t &pack)
{
	pack.package = OPEN(pack.packPath.c_str(), O_BINARY | O_RDONLY, S_IREAD);
	if (pack.package < 0)
		return false;

	//! Allocate new package and fill it.
	PackageHeader_t *pHeader = new PackageHeader_t;
	pack.header = pHeader;
	PackageHeader_t &header = *pHeader;


    char byteBuffer[BUFFER];
    memset(byteBuffer,0,BUFFER);
    unsigned int intBuffer = 0;

	//! Todo : remove name in header, add check to match package name.
    int nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
    nbr = READ(pack.package,byteBuffer,intBuffer);
	byteBuffer[BUFFER-1] = 0;
    header.pckName = STRDUP(byteBuffer);
	pack.headerSize += sizeof(unsigned int) + intBuffer;

	//!	Extract compression and number of file headers
	nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
	header.compression = intBuffer;
	pack.headerSize += sizeof(unsigned int);

    nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
    header.nbFHeaders = intBuffer;
    header.fHeaders = new PackageFileHeader_t[intBuffer];
	pack.headerSize += sizeof(unsigned int);

    for (unsigned int k=0;k<header.nbFHeaders;k++)
    {
        nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].fname = (char*)malloc(intBuffer+1);
		if (header.fHeaders[k].fname == NULL)
			return false;
        nbr = READ(pack.package,header.fHeaders[k].fname,intBuffer);
        header.fHeaders[k].fname[intBuffer] = 0;
		pack.headerSize += sizeof(unsigned int) + intBuffer;

        nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].fsize = intBuffer;
		pack.headerSize += sizeof(unsigned int);

        nbr = READ(pack.package,&intBuffer,sizeof(unsigned int));
        header.fHeaders[k].offset = intBuffer;
		pack.headerSize += sizeof(unsigned int);
    }

    return true;
}

std::string CRaptorDataManager::getPackPath(const std::string& packName)
{
	char buffer[BUFFER];
	std::string pakpath = "./";
	pakpath += packName;

#if defined(WIN32)
    DWORD pakPath = GetEnvironmentVariable("RAPTOR_ROOT",buffer,BUFFER);
    if (pakPath > 0)
    {
        pakpath = buffer;
		std::string::size_type st = pakpath.find("\"");
		while (std::string::npos != st)
		{
			pakpath.erase(st,1);
			st = pakpath.find("\"");
		}
		pakpath += "/Redist/Bin/";
    }
#elif defined(LINUX)
	char *pakPath = getenv("RAPTOR_ROOT");
    if (pakPath != NULL)
    {
        pakpath = pakPath;
		pakpath += "/Redist/Bin/";
    }
#endif

	pakpath += packName;
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

#if defined(WIN32)
	filename += "\\";
#else
    filename += "/";
#endif
    filename += fname;
	
	int dst = OPEN(filename.c_str(), O_RDONLY | O_BINARY, S_IREAD);
	if (dst > 0)
	{
		CLOSE(dst);
		return filename;
	}

	Package_t pack;
	PackageHeader_t* pHeader = NULL;
	PackageFileHeader_t *fHeader = NULL;

    bool res = false;
	for (size_t i = 0; !res && (i < m_packages.size()); i++)
	{
		pack = m_packages[i];
		pHeader = (PackageHeader_t*)pack.header;

		for (unsigned int j = 0; !res && (j < pHeader->nbFHeaders); j++)
		{
			fHeader = &(pHeader->fHeaders[j]);
			res = (0 == fname.compare(fHeader->fname));
		}
	}
    if (!res)
        return "";


	std::string tmpfname = (pHeader->compression != Z_NO_COMPRESSION) ?
							filename + ".zip" : filename;

	dst = OPEN(tmpfname.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IWRITE);
	if (dst < 0)
		return "";

    long pos = LSEEK(pack.package,fHeader->offset + pack.headerSize, SEEK_SET);
    if (pos == -1L)
        return "";
	else
		copyFile(dst, pack.package, fHeader->fsize);

    CLOSE(dst);

	if (pHeader->compression != Z_NO_COMPRESSION)
		unzipfile(tmpfname.c_str(),filename.c_str());

    return filename;
}

std::string CRaptorDataManager::readFile(const std::string &fname)
{
	int fileHandle = OPEN(fname.c_str(), O_BINARY | O_RDONLY, S_IREAD);
	if (fileHandle < 0)
		return "";

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

