// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

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
class CVertexShader;
class CFragmentShader;
class CVertexProgram;
class CFragmentProgram;
class CGeometryProgram;
class CVulkanShaderStage;

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
	CVertexShader * const glGetVertexShader(const std::string& name = "");

	//!	Returns true if shader has a Vertex Shader already
	bool hasVertexShader(void) const { return m_pVShader != NULL; };

	//! Removes the vertex shader.
	//! @return true if the vertex shader has been deleted
	bool glRemoveVertexShader(void);

	//!	Returns the fragment shader
	//!	Allocate a new one if necessary
	CFragmentShader * const glGetFragmentShader(const std::string& name = "");

	//!	Returns true if shader has a Fragment Shader already
	bool hasFragmentShader(void) const { return m_pFShader != NULL; };

	//! Removes the fragment shader.
	//! @return true if the fragment shader has been deleted
	bool glRemoveFragmentShader(void);

    //!	Returns the vertex Program
	//!	Allocate a new one if necessary
	CVertexProgram * const glGetVertexProgram(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasVertexProgram(void) const { return m_pVProgram != NULL; };

	//! Removes the vertex program.
	//! @return true if the vertex program has been deleted
	bool glRemoveVertexProgram(void);

	//!	Returns the fragment Program
	//!	Allocate a new one if necessary
	CFragmentProgram * const glGetFragmentProgram(const std::string& name = "");

	//!	Returns true if Program has a Fragment Program already
	bool hasFragmentProgram(void) const { return m_pFProgram != NULL; };

	//! Removes the fragment program.
	//! @return true if the fragment program has been deleted
	bool glRemoveFragmentProgram(void);

	//!	Returns the geometry Program
	//!	Allocate a new one if necessary
	CGeometryProgram * const glGetGeometryProgram(const std::string& name = "");

	//!	Returns true if Program has a Vertex Program already
	bool hasGeometryProgram(void) const { return m_pGProgram != NULL; };

	//! Removes the geometry program.
	//! @return true if the geometry program has been deleted
	bool glRemoveGeometryProgram(void);


	//!	Returns the geometry Program
	//!	Allocate a new one if necessary
	CVulkanShaderStage * const vkGetVulkanProgram(const std::string& name = "");

	//!	Returns true if Program has a Vulkan Shader already
	bool hasVulkanProgram(void) const { return m_pVulkanProgram != NULL; };

	//! Removes the Vulkan program.
	//! @return true if the Vulkan program has been deleted
	bool vkRemoveVulkanProgram(void);


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
    CShader& operator=(const CShader&) { return *this; }

    //! Implements CPersistence
    virtual void unLink(const CPersistence* p);

	CColor::RGBA		m_color;
    static CColor::RGBA	m_ambient;

    RAPTOR_HANDLE       m_shaderProgram;
	RAPTOR_HANDLE		m_textureUnitSetup;
	RAPTOR_HANDLE		m_textureUnitUnSetup;

	CMaterial			*m_pMaterial;
	CTextureUnitSetup	*m_pTMUSetup;
	CVertexShader		*m_pVShader;
	CFragmentShader		*m_pFShader;
    CVertexProgram      *m_pVProgram;
    CFragmentProgram    *m_pFProgram;
	CGeometryProgram	*m_pGProgram;
	CVulkanShaderStage	*m_pVulkanProgram;

	bool				m_bDeleteVShader;
	bool				m_bDeleteFShader;
    bool				m_bDeleteVProgram;
	bool				m_bDeleteFProgram;
	bool				m_bDeleteGProgram;
	bool				m_bDeleteVulkanProgram;
	bool				m_bDeleteTMUSetup;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)

