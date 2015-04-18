// RaptorDataManager.h: interface for the CRaptorDataManager class.
//
//////////////////////////////////////////////////////////////////////

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
using std::string;

class RAPTORDATA_API CRaptorDataManager  
{
public:
    static CRaptorDataManager  *getInstance(void);

	virtual ~CRaptorDataManager();

    //! Exports a file in a data package. The file is extracted to the specified path 
    //! or to system TMP if path is empty
    //! @return the filepath if export succeeded, an empty string if errors found.
    std::string ExportFile(const std::string& fname, const std::string& topath = "");

    //! Export the about video ( Win32 only, Unix platform in next releases
    void ExportRaptorVideo(const std::string& path);

    //! Returns default the list of shaders embedded with Raptor
	const char *const * GetShaderList(void);


private:
    CRaptorDataManager();

    bool openPackage(const std::string& pakName);

	std::string readFile(const std::string &fname);

    static CRaptorDataManager  *m_pInstance;
};

#endif // !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
