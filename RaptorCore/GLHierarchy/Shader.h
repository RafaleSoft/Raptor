/***************************************************************************/
/*                                                                         */
/*  Shader.h                                                               */
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


#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
#define AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "ObjectReference.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CMaterial;
class CTextureUnitSetup;
class CVulkanShaderStage;
class COpenGLProgramStage;
class COpenGLShaderStage;


class RAPTOR_API CShader : public CPersistence, public CObjectReference
{
public:
	//! Base constructor
	CShader(const std::string& name = "SHADER");

	//!	Clone current shader, assigning the new name newShaderName
	virtual CShader* glClone(const std::string& newShaderName) const;

	//!	Returns a an existing shader or the default Null Shader
	//! if shaderName is not found or if shaderName is the name of a non-shader object.
	//! @return the shader named shaderName or the dafault NullShader.
	static const CShader& getShader(const std::string& shaderName);

    //! Applyies the shader, the shader must be compiled.
	virtual void glRender(void);

	//! Make the fixed GL pipeline the default shader.
    virtual void glStop(void);


    //!
    //! Material configuration
    //!
	//!	Basic shading parameters : at least a color !
	void setColor(float r, float g, float b, float a) { m_color.r = r; m_color.g = g; m_color.b = b; m_color.a = a; };
	CColor::RGBA getColor(void) const { return m_color; };

    //! With lighting and no lights, at least ambient color control
    static void setAmbient(float r, float g, float b, float a);
	static CColor::RGBA getAmbient(void);

	//!	Returns the material.
	//!	Allocate a new one if necessary
	CMaterial * const getMaterial(void);

    //! Returns true if shader has a material
	bool hasMaterial(void) const { return NULL != m_pMaterial; };

    //! Removes the material if it has been created.
    //! Returns true if a material is deleted.
    bool glRemoveMaterial(void);

	//!	Renders the internal material if lighting is enabled, uses shader's
	//!	constant color otherwise.
	void glRenderMaterial(void);


    //!
    //! Texturing configuration
    //!
	//!	Returns the texture units configuration.
	//!	Allocate a new one if necessary
	CTextureUnitSetup * const glGetTextureUnitsSetup(const std::string& name = "");

	//!	Returns true if shader has a TMU Setup already
	bool hasTextureUnitSetup(void) const { return NULL != m_pTMUSetup; };

    //! Removes the TMUSetup if it has been created.
    //! @return true if a TMUSetup is deleted.
    bool glRemoveTextureUnitSetup(void);

	//!	Renders the internal texture unit setup if texturing is enabled,
	void glRenderTexture(void);


	//!
	//! OpenGL programs configuration
	//!
	//!	Returns the OpenGL Program.
	//!	Allocate a new one if necessary
	COpenGLProgramStage * const glGetOpenGLProgram(const std::string& name = "");

	//!	Returns true if Program has a OpenGL Program already
	bool hasOpenGLProgram(void) const { return NULL != m_pOpenGLProgram; };

	//! Removes the OpenGL program.
	//! @return true if the OpenGL program has been deleted
	bool glRemoveOpenGLProgram(void);


	//!
	//! OpenGL shaders configuration
	//!
	//!	Returns the OpenGL Shader.
	//!	Allocate a new one if necessary
	COpenGLShaderStage * const glGetOpenGLShader(const std::string& name = "");

	//!	Returns true if Program has a OpenGL Program already
	bool hasOpenGLShader(void) const { return NULL != m_pOpenGLShader; };

	//! Removes the OpenGL program.
	//! @return true if the OpenGL program has been deleted
	bool glRemoveOpenGLShader(void);


	//!
	//! Vulkan shaders configuration
	//!
	//!	Returns the Vulkan Shader.
	//!	Allocate a new one if necessary
	CVulkanShaderStage * const vkGetVulkanShader(const std::string& name = "");

	//!	Returns true if Program has a Vulkan Shader already
	bool hasVulkanShader(void) const { return NULL != m_pVulkanShader; };

	//! Removes the Vulkan Shader.
	//! @return true if the Vulkan program has been deleted
	bool vkRemoveVulkanShader(void);


	//!	Streams : implement CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CShaderClassID,"Shader",CPersistence)


protected:
	//! Copy constructor.
	CShader(const CShader&);

	//! Destructor.
	virtual ~CShader();


private:
    //! Denied operators
	CShader& operator=(const CShader&);

    //! Implements CPersistence
    virtual void unLink(const CPersistence* p);

	CColor::RGBA		m_color;
    static CColor::RGBA	m_ambient;

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
	RAPTOR_HANDLE		m_textureUnitSetup;
	RAPTOR_HANDLE		m_textureUnitUnSetup;
#endif

	CMaterial			*m_pMaterial;
	CTextureUnitSetup	*m_pTMUSetup;

	CVulkanShaderStage	*m_pVulkanShader;
	COpenGLShaderStage	*m_pOpenGLShader;
	COpenGLProgramStage	*m_pOpenGLProgram;

	bool				m_bDeleteVulkanShader;
	bool				m_bDeleteOpenGLProgram;
	bool				m_bDeleteOpenGLShader;
	bool				m_bDeleteTMUSetup;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)

