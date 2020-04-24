/***************************************************************************/
/*                                                                         */
/*  RaptorDataManager.h                                                    */
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


#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
#define AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
	#if defined(EXPORT_RAPTOR_DATA_MANAGER)
		#define RAPTORDATA_API __declspec(dllexport)
	#else
		#if !defined(RAPTORDATA_API)
			#define RAPTORDATA_API _declspec(dllimport)
		#endif
	#endif
#else
	#define RAPTORDATA_API
#endif


#include <string>
#include <vector>

class RAPTORDATA_API CRaptorDataManager  
{
public:
	typedef enum
	{
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		VERTEX_PROGRAM,
		FRAGMENT_PROGRAM,
		GEOMETRY_PROGRAM,
		COMPUTE_PROGRAM,
		TESSELATION_PROGRAM,
		VIDEO,
		AUDIO,
		TEXT
	} FILE_TYPE;

    static CRaptorDataManager  *GetInstance(void);

	virtual ~CRaptorDataManager();

    //! Exports a file in a data package. The file is extracted to the specified path 
    //! or to system TMP if path is empty
    //! @return the filepath if export succeeded, an empty string if errors found.
    std::string exportFile(const std::string& fname, const std::string& topath = "");

    //! Export the about video ( Win32 only, Unix platform in next releases )
    void exportRaptorVideo(const std::string& path);

	//!	Remove all exported files from a managed package for a fresh update.
	//!	@param packName : the name of the package to clear exports of.
	//! @return false if package not managed.
	bool clearExports(const std::string& packName);

	//!	Defines a package name, different from the default 'RaptorData.pck'
	bool managePackage(const std::string& packName);

	//!	Returns the names of the packages managed
	std::vector<std::string> getManagedPackages(void) const;

	//! Returns the names of the files contained in the given package.
	//! If packName is empty, returns all the files from all packages.
	std::vector<std::string> getManagedFiles(const std::string &packName) const;



private:
    CRaptorDataManager();

	typedef struct Package_t
	{
		int				package;
		std::string		packPath;
		size_t			headerSize;
		void			*header;
	} Package;

	bool openPackage(Package_t &pack);

	//!	Read an opened file and return the text content.
	//!	TODO: read long files.
	std::string readFile(const std::string &fname);

	//!	Returns the path of the given package filename relative to RAPTOR_ROOT.
	std::string getPackPath(const std::string& packName);

	//!	The only one DAta Manager Instance.
    static CRaptorDataManager  *m_pInstance;

	//!	The list of packages managed.
#pragma warning(disable:4251)	// dll interface
	std::vector<Package>	m_packages;
};

#endif // !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
