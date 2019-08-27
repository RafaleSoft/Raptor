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
class CVertexProgram_old;
class CFragmentProgram_old;
class CVertexShader;
class CFragmentShader;
class CGeometryShader;
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

    //! Generate the shader as a compiled object ( program objects / display lists ).
    //! The compile state must be correct to render the shader.
	//! @return true if successfully compiled, false otherwise.
    bool glCompileShader(void); 



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
	bool hasMaterial(void) const { return m_pMaterial != NULL; };

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
	bool hasTextureUnitSetup(void) const { return m_pTMUSetup != NULL; };

    //! Removes the TMUSetup if it has been created.
    //! @return true if a TMUSetup is deleted.
    bool glRemoveTextureUnitSetup(void);

	//!	Renders the internal texture unit setup if texturing is enabled,
	void glRenderTexture(void);


    //!
    //! Shaders configuration
    //!
	//!	Returns the vertex shader
	//!	Allocate a new one if necessary
	CVertexProgram_old * const glGetVertexProgram_old(const std::string& name = "");

	//!	Returns true if shader has a Vertex Shader already
	bool hasVertexProgram_old(void) const { return m_pVProgram_old != NULL; };

	//! Removes the vertex shader.
	//! @return true if the vertex shader has been deleted
	bool glRemoveVertexProgram_old(void);

	//!	Returns the fragment shader
	//!	Allocate a new one if necessary
	CFragmentProgram_old * const glGetFragmentProgram_old(const std::string& name = "");

	//!	Returns true if shader has a Fragment Shader already
	bool hasFragmentProgram_old(void) const { return m_pFProgram_old != NULL; };

	//! Removes the fragment shader.
	//! @return true if the fragment shader has been deleted
	bool glRemoveFragmentProgram_old(void);

    //!	Returns the vertex Program, allocate a new one if necessary.
	//!	EMPTY_PROGRAM is a special name for a void program (i.e. doing nothing)
	CVertexShader * const glGetVertexShader(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasVertexShader(void) const { return m_pVShader != NULL; };

	//! Removes the vertex program.
	//! @return true if the vertex program has been deleted
	bool glRemoveVertexShader(void);

	//!	Returns the fragment Program
	//!	Allocate a new one if necessary
	CFragmentShader * const glGetFragmentShader(const std::string& name = "");

	//!	Returns true if Program has a Fragment Program already
	bool hasFragmentShader(void) const { return m_pFShader != NULL; };

	//! Removes the fragment program.
	//! @return true if the fragment program has been deleted
	bool glRemoveFragmentShader(void);

	//!	Returns the geometry Program
	//!	Allocate a new one if necessary
	CGeometryShader * const glGetGeometryShader(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasGeometryShader(void) const { return m_pGShader != NULL; };

	//! Removes the geometry program.
	//! @return true if the geometry program has been deleted
	bool glRemoveGeometryShader(void);


	//!	Returns the OpenGL Program.
	//!	Allocate a new one if necessary
	COpenGLProgramStage * const glGetOpenGLProgram(const std::string& name = "");

	//!	Returns true if Program has a OpenGL Program already
	bool hasOpenGLProgram(void) const { return m_pOpenGLProgram != NULL; };

	//! Removes the OpenGL program.
	//! @return true if the OpenGL program has been deleted
	bool glRemoveOpenGLProgram(void);


	//!	Returns the Vulkan Shader.
	//!	Allocate a new one if necessary
	CVulkanShaderStage * const vkGetVulkanShader(const std::string& name = "");

	//!	Returns true if Program has a Vulkan Shader already
	bool hasVulkanShader(void) const { return m_pVulkanShader != NULL; };

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

    RAPTOR_HANDLE       m_shaderProgram;
	RAPTOR_HANDLE		m_textureUnitSetup;
	RAPTOR_HANDLE		m_textureUnitUnSetup;

	CMaterial			*m_pMaterial;
	CTextureUnitSetup	*m_pTMUSetup;

	CVertexProgram_old	*m_pVProgram_old;
	CFragmentProgram_old *m_pFProgram_old;
    CVertexShader		*m_pVShader;
	CFragmentShader		*m_pFShader;
	CGeometryShader		*m_pGShader;

	CVulkanShaderStage	*m_pVulkanShader;
	COpenGLShaderStage	*m_pOpenGLShader;
	COpenGLProgramStage	*m_pOpenGLProgram;

	bool				m_bDeleteVProgram_old;
	bool				m_bDeleteFProgram_old;
    bool				m_bDeleteVShader;
	bool				m_bDeleteFShader;
	bool				m_bDeleteGShader;
	bool				m_bDeleteVulkanShader;
	bool				m_bDeleteOpenGLProgram;
	bool				m_bDeleteOpenGLShader;
	bool				m_bDeleteTMUSetup;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)

