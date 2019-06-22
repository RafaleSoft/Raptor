/***************************************************************************/
/*                                                                         */
/*  ShaderLibrary.h                                                        */
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


#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
#define AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CVertexProgram;
class CFragmentProgram;
class CGeometryProgram;
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
	static CShaderLibrary*	GetInstance(void);

	//!	Destroy the library.
	void destroy();

	//! Initialise default factory shaders.
	//! Depends on Raptor versions.
	//!	Returns true if library is initialized, false if failed or already initialized.
	bool glInitFactory(void);

	//!	Returns the default null shader.
	const CShader& getNullShader(void)
	{ return *m_pNullShader; };

	//! Returns the list off shaders names.
	//! Each name refers to a corresponding object
	//! in memory, that can be retrived through its persistence name.
	void getFactoryShaders(vector<std::string> &);

	//!	Adds a shader to this library: 
	//! provide the name of shader, the program source, and the Raptor shader class
	bool glAddToLibrary(const std::string& shader_name,
						const std::string& shader_source,
						const std::string& class_name);
	

private:
	//!	Unavailable instance constructor
	CShaderLibrary();

	//! Unavailable instance destructor;
	virtual ~CShaderLibrary();

	//!	Load RaptorCore Shaders from currently loaded Raptor Data Packages.
	bool glLoadShadersFromDataPackage();

	//! The instance.
	static CShaderLibrary *m_pInstance;

	//!	A default shader.
	static CShader *m_pNullShader;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)

