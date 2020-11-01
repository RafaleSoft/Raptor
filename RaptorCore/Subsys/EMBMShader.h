/***************************************************************************/
/*                                                                         */
/*  EMBMShader.h                                                           */
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


#if !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
#define AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_BUMPSHADER_H__6201C4A1_1F09_41C4_836F_2AAC79D36A42__INCLUDED_)
	#include "BumpShader.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CBumpLightObserver;


class CEMBMShader : public CBumpShader
{
public:
	//!	Default constructor.
	CEMBMShader(void);

	//! Destructor.
	virtual ~CEMBMShader(void);

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Implements base glRender.
	virtual void glRender(void);

	//! Initialize all shadings, must be called before use.
	void glInit();

	//!	A state accessor to switch between strict bumping to environment bumping
	void enableEmbm(bool enable);

	//!	Return current status.
	bool isEnabled(void) const { return m_bEnabled; };


private:
	//!	Copy constructor for cloning.
	CEMBMShader(const CEMBMShader& shader);

	//!	The enabled status
	bool	m_bEnabled;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_EMBMSHADER_H__99A5AF45_D5C7_4F43_851C_A31FC52DB237__INCLUDED_)
