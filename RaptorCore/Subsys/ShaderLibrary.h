/***************************************************************************/
/*                                                                         */
/*  ShaderLibrary.h                                                        */
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


#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
#define AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CShader;

//!
//!	This class provides a library or shader sources.
//! Theses sources are loaded into vertex or fragment 
//! program objects that can be reused.
//!	Available shaders depend on Raptor releases.
//!
class CShaderLibrary
{
public:
	typedef struct
	{
		const char *shader_name;
		const char *shader_fname;
		const char *class_name;
	} factory_shader;


	//!	Library constructor
	CShaderLibrary();

	//! Destroy the library
	~CShaderLibrary();

	//! Initialise default factory shaders.
	//! Depends on Raptor versions.
	//!	Returns true if library is initialized, false if failed or already initialized.
	bool glInitFactory(void);

	//! Returns the list off shaders names.
	//! Each name refers to a corresponding object
	//! in memory, that can be retrived through its persistence name.
	void getFactoryShaders(std::vector<std::string> &);

	//!	Adds a shader to this library: 
	//! provide the name of shader, the program source, and the Raptor shader class
	bool glAddToLibrary(const std::string& shader_name,
						const std::string& shader_source,
						const std::string& class_name);
	

private:
	//!	Load RaptorCore Shaders from currently loaded Raptor Data Packages.
	bool glLoadShadersFromDataPackage();

	//!	Library is initialised, enable harmless successive calls to glInitFactory.
	bool s_initialized;

	//!	Specific shaders created by the library.
	//!	These shaders are the more commonly used, so precreate them.
	CShader *m_pBlinnShader;
	CShader *m_pPhongShader;
	CShader *m_pBumpShader;
	CShader *m_pEMBMShader;
	CShader *m_pAOComputeShader;

	std::map<std::string, CShaderLibrary::factory_shader>	s_factoryShaders;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)

