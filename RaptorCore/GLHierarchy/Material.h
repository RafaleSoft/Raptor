// Material.h: interface for the CMaterial class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
#define AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CMaterial : public CPersistence
{
public:
	//!
	//!	Predefined Base Material colors
	//!
	typedef enum
	{
		CGL_BLACK_MATERIAL,
		CGL_WHITE_MATERIAL,
		CGL_RED_MATERIAL,
		CGL_GREEN_MATERIAL,
		CGL_BLUE_MATERIAL,
		CGL_YELLOW_MATERIAL,
		CGL_CYAN_MATERIAL,
		CGL_MAGENTA_MATERIAL,
		CGL_DARKGRAY_MATERIAL,
		CGL_GRAY_MATERIAL,
		CGL_LIGHTGRAY_MATERIAL,
		CGL_NO_MATERIAL,
		CGL_NB_BASE_MATERIALS
	} BASE_MATERIAL;

	//!
	//!	Predefined Materials simulating common objets
	//!
	typedef enum
	{
		CGL_BRASS_MATERIAL,
		CGL_BRONZE_MATERIAL,
		CGL_POLISHED_BRONZE_MATERIAL,
		CGL_CHROME_MATERIAL,
		CGL_COPPER_MATERIAL,
		CGL_POLISHED_COPPER_MATERIAL,
		CGL_GOLD_MATERIAL,
		CGL_POLISHED_GOLD_MATERIAL,
		CGL_TIN_MATERIAL,
		CGL_SILVER_MATERIAL,
		CGL_POLISHED_SILVER_MATERIAL,
		CGL_EMERALD_MATERIAL,
		CGL_JADE_MATERIAL,
		CGL_OBSIDIAN_MATERIAL,
		CGL_PERL_MATERIAL,
		CGL_RUBY_MATERIAL,
		CGL_TURQUOISE_MATERIAL,
		CGL_BLACK_PLATIC_MATERIAL,
		CGL_CYAN_PLATIC_MATERIAL,
		CGL_GREEN_PLATIC_MATERIAL,
		CGL_RED_PLATIC_MATERIAL,
		CGL_WHITE_PLATIC_MATERIAL,
		CGL_YELLOW_PLATIC_MATERIAL,
		CGL_BLACK_RUBBER_MATERIAL,
		CGL_CYAN_RUBBER_MATERIAL,
		CGL_GREEN_RUBBER_MATERIAL,
		CGL_RED_RUBBER_MATERIAL,
		CGL_WHITE_RUBBER_MATERIAL,
		CGL_YELLOW_RUBBER_MATERIAL,
		CGL_NO_REAL_MATERIAL,
		CGL_NB_REAL_MATERIALS
	} REAL_MATERIAL;

	//!
	//!	Shader parameter type.
	//!
	typedef struct Material_t
	{
		CColor::RGBA ambient;
		CColor::RGBA diffuse;
		CColor::RGBA specular;
		CColor::RGBA emission;
		float		 shininess;
	} R_Material;


public:
	CMaterial(	BASE_MATERIAL amb = CGL_NO_MATERIAL,
			    BASE_MATERIAL diff = CGL_NO_MATERIAL,
				BASE_MATERIAL spec = CGL_NO_MATERIAL,
				float shi	= 4.0,
				BASE_MATERIAL em = CGL_NO_MATERIAL,
				const std::string& name = "GL_MATERIAL");

	CMaterial(	REAL_MATERIAL material,
				const std::string& name = "GL_MATERIAL");

	virtual ~CMaterial();

    //! Operator to convert this object into a RAPTOR_HANDLE
	virtual operator RAPTOR_HANDLE() const;

	bool doRebuild(void) const { return m_bRebuild; };

    //! Setters
	void setAmbient(float r,float g,float b,float a);
	void setAmbient(const CColor::RGBA& ambient)
	{ setAmbient(ambient.r,ambient.g,ambient.b,ambient.a); };

	void setDiffuse(float r,float g,float b,float a);
	void setDiffuse(const CColor::RGBA& diffuse)
	{ setDiffuse(diffuse.r,diffuse.g,diffuse.b,diffuse.a); };

	void setSpecular(float r,float g,float b,float a);
	void setSpecular(const CColor::RGBA& specular)
	{ setSpecular(specular.r,specular.g,specular.b,specular.a); };

	void setEmission(float r,float g,float b,float a);
	void setEmission(const CColor::RGBA& emission)
	{ setEmission(emission.r,emission.g,emission.b,emission.a); };
	void setShininess(float exp);

    //! Getters
	CColor::RGBA getAmbient(void) const { return M.ambient; };
	CColor::RGBA getDiffuse(void) const { return M.diffuse; };
	CColor::RGBA getSpecular(void) const { return M.specular; };
	CColor::RGBA getEmission(void) const { return M.emission; };
	float getShininess(void) const { return M.shininess; };
	const Material_t& getMaterial(void) const { return M; };

    //! This method makes the material darker, multiplying the values by 'd'
	void darken(float d);

    //! This method makes the material brighter, multiplying the values by 'd'
	void lighten(float l);

    //! Common Raptor rendering method
	void glRender();

    //! Material objects can be copied
	const CMaterial& operator=(const CMaterial& rsh);

    //! CPersistence implementation
	DECLARE_IO
	DECLARE_CLASS_ID(CMaterialClassID,"Material",CPersistence)


protected:
	CMaterial(	BASE_MATERIAL amb,
				BASE_MATERIAL diff,
				BASE_MATERIAL spec,
				float shi,
				BASE_MATERIAL em,
				const CPersistence::CPersistenceClassID &ID,
				const std::string& name = "GL_MATERIAL");

	Material_t	M;

private:
	bool	ambient_enabled;
	bool	diffuse_enabled;
	bool	specular_enabled;
	bool	emission_enabled;

    bool	m_bRebuild;

	RAPTOR_HANDLE	handle;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)

