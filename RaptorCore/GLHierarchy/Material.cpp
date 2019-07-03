// Material.cpp: implementation of the CMaterial class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Material.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif


RAPTOR_NAMESPACE

CMaterial	*CMaterial::pCurrentMaterial = NULL;

static float COLOR_MATERIAL[11][4] =
	{
		{ 0.0, 0.0, 0.0, 1.0 },
		{ 1.0, 1.0, 1.0, 1.0 },
		{ 1.0, 0.0, 0.0, 1.0 },
		{ 0.0, 1.0, 0.0, 1.0 },
		{ 0.0, 0.0, 1.0, 1.0 },
		{ 1.0, 1.0, 0.0, 1.0 },
		{ 0.0, 1.0, 1.0, 1.0 },
		{ 1.0, 0.0, 1.0, 1.0 },
		{ 0.25, 0.25, 0.25, 1.0 },
		{ 0.5, 0.5, 0.5, 1.0 },
		{ 0.75, 0.75, 0.75, 1.0 }
	};

//Brass
static CColor::RGBA BRASS_AMBIENT(0.329412f, 0.223529f, 0.027451f,1.0f);
static CColor::RGBA BRASS_DIFFUSE(0.780392f, 0.568627f, 0.113725f, 1.0f);
static CColor::RGBA BRASS_SPECULAR(0.992157f, 0.941176f, 0.807843f, 1.0f);
static float BRASS_SHINE = 27.8974f;
// Bronze
static CColor::RGBA BRONZE_AMBIENT(0.2125f, 0.1275f, 0.054f, 1.0f);
static CColor::RGBA BRONZE_DIFFUSE(0.714f, 0.4284f, 0.18144f, 1.0f);
static CColor::RGBA BRONZE_SPECULAR(0.393548f, 0.271906f, 0.166721f, 1.0f);
static float BRONZE_SHINE = 25.6f;
//Polished bronze
static CColor::RGBA PBRONZE_AMBIENT(0.25f, 0.148f, 0.06475f, 1.0f);
static CColor::RGBA PBRONZE_DIFFUSE(0.4f, 0.2368f, 0.1036f, 1.0f);
static CColor::RGBA PBRONZE_SPECULAR(0.774597f, 0.458561f, 0.200621f, 1.0f);
static float PBRONZE_SHINE = 76.8f ;
//Chrome
static CColor::RGBA CHROME_AMBIENT(0.25f, 0.25f, 0.25f, 1.0f);
static CColor::RGBA CHROME_DIFFUSE(0.4f, 0.4f, 0.4f, 1.0f);
static CColor::RGBA CHROME_SPECULAR(0.774597f, 0.774597f, 0.774597f, 1.0f);
static float CHROME_SHINE = 76.8f;
//Copper
static CColor::RGBA COPPER_AMBIENT(0.19125f, 0.0735f, 0.0225f, 1.0f);
static CColor::RGBA COPPER_DIFFUSE(0.7038f, 0.27048f, 0.0828f, 1.0f);
static CColor::RGBA COPPER_SPECULAR(0.256777f, 0.137622f, 0.086014f, 1.0f);
static float COPPER_SHINE = 12.8f;
//Polished copper
static CColor::RGBA PCOPPER_AMBIENT(0.2295f, 0.08825f, 0.0275f, 1.0f);
static CColor::RGBA PCOPPER_DIFFUSE(0.5508f, 0.2118f, 0.066f, 1.0f);
static CColor::RGBA PCOPPER_SPECULAR(0.580594f, 0.223257f, 0.0695701f, 1.0f);
static float PCOPPER_SHINE = 51.2f ;
//Gold
static CColor::RGBA GOLD_AMBIENT(0.24725f, 0.1995f, 0.0745f, 1.0f);
static CColor::RGBA GOLD_DIFFUSE(0.75164f, 0.60648f, 0.22648f, 1.0f);
static CColor::RGBA GOLD_SPECULAR(0.628281f, 0.555802f, 0.366065f, 1.0f);
static float GOLD_SHINE = 51.2f ;
//Polished gold
static CColor::RGBA PGOLD_AMBIENT(0.24725f, 0.2245f, 0.0645f, 1.0f);
static CColor::RGBA PGOLD_DIFFUSE(0.34615f, 0.3143f, 0.0903f, 1.0f);
static CColor::RGBA PGOLD_SPECULAR(0.797357f, 0.723991f, 0.208006f, 1.0f);
static float PGOLD_SHINE = 83.2f ;
//Tin
static CColor::RGBA TIN_AMBIENT(0.105882f, 0.058824f, 0.113725f, 1.0f);
static CColor::RGBA TIN_DIFFUSE(0.427451f, 0.470588f, 0.541176f, 1.0f);
static CColor::RGBA TIN_SPECULAR(0.333333f, 0.333333f, 0.521569f, 1.0f);
static float TIN_SHINE = 9.84615f;
//Silver
static CColor::RGBA SILVER_AMBIENT(0.19225f, 0.19225f, 0.19225f, 1.0f);
static CColor::RGBA SILVER_DIFFUSE(0.50754f, 0.50754f, 0.50754f, 1.0f);
static CColor::RGBA SILVER_SPECULAR(0.508273f, 0.508273f, 0.508273f, 1.0f);
static float SILVER_SHINE = 51.2f;
//Polished silver
static CColor::RGBA PSILVER_AMBIENT(0.23125f, 0.23125f, 0.23125f, 1.0f);
static CColor::RGBA PSILVER_DIFFUSE(0.2775f, 0.2775f, 0.2775f, 1.0f);
static CColor::RGBA PSILVER_SPECULAR(0.773911f, 0.773911f, 0.773911f, 1.0f);
static float PSILVER_SHINE = 89.6f ;
//Emerald
static CColor::RGBA EMERALD_AMBIENT(0.0215f, 0.1745f, 0.0215f, 0.55f);
static CColor::RGBA EMERALD_DIFFUSE(0.07568f, 0.61424f, 0.07568f, 0.55f);
static CColor::RGBA EMERALD_SPECULAR(0.633f, 0.727811f, 0.633f, 0.55f);
static float EMERALD_SHINE = 76.8f;
//Jade
static CColor::RGBA JADE_AMBIENT(0.135f, 0.2225f, 0.1575f, 0.95f);
static CColor::RGBA JADE_DIFFUSE(0.54f, 0.89f, 0.63f, 0.95f);
static CColor::RGBA JADE_SPECULAR(0.316228f, 0.316228f, 0.316228f, 0.95f);
static float JADE_SHINE = 12.8f;
//Obsidian
static CColor::RGBA OBSIDIAN_AMBIENT(0.05375f, 0.05f, 0.06625f, 0.82f);
static CColor::RGBA OBSIDIAN_DIFFUSE(0.18275f, 0.17f, 0.22525f, 0.82f);
static CColor::RGBA OBSIDIAN_SPECULAR(0.332741f, 0.328634f, 0.346435f, 0.82f);
static float OBSIDIAN_SHINE = 38.4f ;
//Perl
static CColor::RGBA PERL_AMBIENT(0.25f, 0.20725f, 0.20725f, 0.922f);
static CColor::RGBA PERL_DIFFUSE(1.0f, 0.829f, 0.829f, 0.922f);
static CColor::RGBA PERL_SPECULAR(0.296648f, 0.296648f, 0.296648f, 0.922f);
static float PERL_SHINE = 11.264f;
//Ruby
static CColor::RGBA RUBY_AMBIENT(0.1745f, 0.01175f, 0.01175f, 0.55f);
static CColor::RGBA RUBY_DIFFUSE(0.61424f, 0.04136f, 0.04136f, 0.55f);
static CColor::RGBA RUBY_SPECULAR(0.727811f, 0.626959f, 0.626959f, 0.55f);
static float RUBY_SHINE =76.8f ;
//Turquoise
static CColor::RGBA TURQUOISE_AMBIENT(0.1f, 0.18725f, 0.1745f, 0.8f);
static CColor::RGBA TURQUOISE_DIFFUSE(0.396f, 0.74151f, 0.69102f, 0.8f);
static CColor::RGBA TURQUOISE_SPECULAR(0.297254f, 0.30829f, 0.306678f, 0.8f);
static float TURQUOISE_SHINE = 12.8f;
//Black plastic
static CColor::RGBA BPLASTIC_AMBIENT(0.0f, 0.0f, 0.0f, 1.0f);
static CColor::RGBA BPLASTIC_DIFFUSE(0.01f, 0.01f, 0.01f, 1.0f);
static CColor::RGBA BPLASTIC_SPECULAR(0.50f, 0.50f, 0.50f, 1.0f);
static float BPLASTIC_SHINE = 32.0f ;
//Cyan plastic
static CColor::RGBA CPLASTIC_AMBIENT(0.0f,0.1f,0.06f ,1.0f);
static CColor::RGBA CPLASTIC_DIFFUSE(0.0f,0.50980392f,0.50980392f,1.0f);
static CColor::RGBA CPLASTIC_SPECULAR(0.50196078f,0.50196078f,0.50196078f,1.0f);
static float CPLASTIC_SHINE = 32.0f ;
//Green plastic
static CColor::RGBA GPLASTIC_AMBIENT(0.0f,0.0f,0.0f,1.0f);
static CColor::RGBA GPLASTIC_DIFFUSE(0.1f,0.35f,0.1f,1.0f);
static CColor::RGBA GPLASTIC_SPECULAR(0.45f,0.55f,0.45f,1.0f);
static float GPLASTIC_SHINE = 32.0f ;
//Red plastic
static CColor::RGBA RPLASTIC_AMBIENT(0.0f,0.0f,0.0f,1.0f);
static CColor::RGBA RPLASTIC_DIFFUSE(0.5f,0.0f,0.0f,1.0f);
static CColor::RGBA RPLASTIC_SPECULAR(0.7f,0.6f,0.6f,1.0f);
static float RPLASTIC_SHINE = 32.0f ;
//White plastic
static CColor::RGBA WPLASTIC_AMBIENT(0.0f,0.0f,0.0f,1.0f);
static CColor::RGBA WPLASTIC_DIFFUSE(0.55f,0.55f,0.55f,1.0f);
static CColor::RGBA WPLASTIC_SPECULAR(0.70f,0.70f,0.70f,1.0f);
static float WPLASTIC_SHINE = 32.0f ;
//Yellow plastic
static CColor::RGBA YPLASTIC_AMBIENT(0.0f,0.0f,0.0f,1.0f);
static CColor::RGBA YPLASTIC_DIFFUSE(0.5f,0.5f,0.0f,1.0f);
static CColor::RGBA YPLASTIC_SPECULAR(0.60f,0.60f,0.50f,1.0f);
static float YPLASTIC_SHINE = 32.0f ;
//Black rubber
static CColor::RGBA BRUBBER_AMBIENT(0.02f, 0.02f, 0.02f, 1.0f);
static CColor::RGBA BRUBBER_DIFFUSE(0.01f, 0.01f, 0.01f, 1.0f);
static CColor::RGBA BRUBBER_SPECULAR(0.4f, 0.4f, 0.4f, 1.0f);
static float BRUBBER_SHINE = 10.0f;
//Cyan rubber
static CColor::RGBA CRUBBER_AMBIENT(0.0f,0.05f,0.05f,1.0f);
static CColor::RGBA CRUBBER_DIFFUSE(0.4f,0.5f,0.5f,1.0f);
static CColor::RGBA CRUBBER_SPECULAR(0.04f,0.7f,0.7f,1.0f);
static float CRUBBER_SHINE = 10.0f;
//Green rubber
static CColor::RGBA GRUBBER_AMBIENT(0.0f,0.05f,0.0f,1.0f);
static CColor::RGBA GRUBBER_DIFFUSE(0.4f,0.5f,0.4f,1.0f);
static CColor::RGBA GRUBBER_SPECULAR(0.04f,0.7f,0.04f,1.0f);
static float GRUBBER_SHINE =  10.0f;
//Red rubber
static CColor::RGBA RRUBBER_AMBIENT(0.05f,0.0f,0.0f,1.0f);
static CColor::RGBA RRUBBER_DIFFUSE(0.5f,0.4f,0.4f,1.0f);
static CColor::RGBA RRUBBER_SPECULAR(0.7f,0.04f,0.04f,1.0f);
static float RRUBBER_SHINE = 10.0f;
//White rubber
static CColor::RGBA WRUBBER_AMBIENT(0.05f,0.05f,0.05f,1.0f);
static CColor::RGBA WRUBBER_DIFFUSE(0.5f,0.5f,0.5f,1.0f);
static CColor::RGBA WRUBBER_SPECULAR(0.7f,0.7f,0.7f,1.0f);
static float WRUBBER_SHINE = 10.0f;
//Yellow rubber
static CColor::RGBA YRUBBER_AMBIENT(0.05f,0.05f,0.0f,1.0f);
static CColor::RGBA YRUBBER_DIFFUSE(0.5f,0.5f,0.4f,1.0f);
static CColor::RGBA YRUBBER_SPECULAR(0.7f,0.7f,0.04f,1.0f);
static float YRUBBER_SHINE = 10.0f;

typedef struct real_mat_t
{
	CColor::RGBA& ambient;
	CColor::RGBA& diffuse;
	CColor::RGBA& specular;
	float shininess;
} real_mat;
static real_mat_t REAL_MATERIALS[29] = 
{
	{ BRASS_AMBIENT, BRASS_DIFFUSE, BRASS_SPECULAR, BRASS_SHINE },
	{ BRONZE_AMBIENT, BRONZE_DIFFUSE, BRONZE_SPECULAR, BRONZE_SHINE },
	{ PBRONZE_AMBIENT, PBRONZE_DIFFUSE, PBRONZE_SPECULAR, PBRONZE_SHINE },
	{ CHROME_AMBIENT, CHROME_DIFFUSE, CHROME_SPECULAR, CHROME_SHINE },
	{ COPPER_AMBIENT, COPPER_DIFFUSE, COPPER_SPECULAR, COPPER_SHINE },
	{ PCOPPER_AMBIENT, PCOPPER_DIFFUSE, PCOPPER_SPECULAR, PCOPPER_SHINE },
	{ GOLD_AMBIENT, GOLD_DIFFUSE, GOLD_SPECULAR, GOLD_SHINE },
	{ PGOLD_AMBIENT, PGOLD_DIFFUSE, PGOLD_SPECULAR, PGOLD_SHINE },
	{ TIN_AMBIENT, TIN_DIFFUSE, TIN_SPECULAR, TIN_SHINE },
	{ SILVER_AMBIENT, SILVER_DIFFUSE, SILVER_SPECULAR, SILVER_SHINE },
	{ PSILVER_AMBIENT, PSILVER_DIFFUSE, PSILVER_SPECULAR, PSILVER_SHINE },
	{ EMERALD_AMBIENT, EMERALD_DIFFUSE, EMERALD_SPECULAR, EMERALD_SHINE },
	{ JADE_AMBIENT, JADE_DIFFUSE, JADE_SPECULAR, JADE_SHINE },
	{ OBSIDIAN_AMBIENT, OBSIDIAN_DIFFUSE, OBSIDIAN_SPECULAR, OBSIDIAN_SHINE },
	{ PERL_AMBIENT, PERL_DIFFUSE, PERL_SPECULAR, PERL_SHINE },
	{ RUBY_AMBIENT, RUBY_DIFFUSE, RUBY_SPECULAR, RUBY_SHINE },
	{ TURQUOISE_AMBIENT, TURQUOISE_DIFFUSE, TURQUOISE_SPECULAR, TURQUOISE_SHINE },
	{ BPLASTIC_AMBIENT, BPLASTIC_DIFFUSE, BPLASTIC_SPECULAR, BPLASTIC_SHINE },
	{ CPLASTIC_AMBIENT, CPLASTIC_DIFFUSE, CPLASTIC_SPECULAR, CPLASTIC_SHINE },
	{ GPLASTIC_AMBIENT, GPLASTIC_DIFFUSE, GPLASTIC_SPECULAR, GPLASTIC_SHINE },
	{ RPLASTIC_AMBIENT, RPLASTIC_DIFFUSE, RPLASTIC_SPECULAR, RPLASTIC_SHINE },
	{ WPLASTIC_AMBIENT, WPLASTIC_DIFFUSE, WPLASTIC_SPECULAR, WPLASTIC_SHINE },
	{ YPLASTIC_AMBIENT, YPLASTIC_DIFFUSE, YPLASTIC_SPECULAR, YPLASTIC_SHINE },
	{ BRUBBER_AMBIENT, BRUBBER_DIFFUSE, BRUBBER_SPECULAR, BRUBBER_SHINE },
	{ CRUBBER_AMBIENT, CRUBBER_DIFFUSE, CRUBBER_SPECULAR, CRUBBER_SHINE },
	{ GRUBBER_AMBIENT, GRUBBER_DIFFUSE, GRUBBER_SPECULAR, GRUBBER_SHINE },
	{ RRUBBER_AMBIENT, RRUBBER_DIFFUSE, RRUBBER_SPECULAR, RRUBBER_SHINE },
	{ WRUBBER_AMBIENT, WRUBBER_DIFFUSE, WRUBBER_SPECULAR, WRUBBER_SHINE },
	{ YRUBBER_AMBIENT, YRUBBER_DIFFUSE, YRUBBER_SPECULAR, YRUBBER_SHINE }
};

#define SET_MATERIAL(var,mat) \
	{\
	    var[0]=COLOR_MATERIAL[mat][0]; \
	    var[1]=COLOR_MATERIAL[mat][1]; \
	    var[2]=COLOR_MATERIAL[mat][2]; \
	    var[3]=COLOR_MATERIAL[mat][3]; \
	}
#define DEL_LIST \
	if (handle.handle() > 0) \
		glDeleteLists(handle.handle(),1);
#define SET_COLOR(var) \
	{\
		var[0]=r;\
		var[1]=g;\
		var[2]=b;\
		var[3]=a;\
	}
#define DARKEN(var) \
	{\
		var[0]=MAX(var[0]*dd,0.0f);\
		var[1]=MAX(var[1]*dd,0.0f);\
		var[2]=MAX(var[2]*dd,0.0f);\
	}
#define LIGHTEN(var) \
	{\
		var[0]=MIN(var[0]*dd,1.0f);\
		var[1]=MIN(var[1]*dd,1.0f);\
		var[2]=MIN(var[2]*dd,1.0f);\
	}

IMPLEMENT_CLASS_ID(CMaterial, materialID)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMaterial::CMaterial(REAL_MATERIAL material,
					 const std::string& name):
	CPersistence(materialID,name)
{
	m_bRebuild = true;

	handle.handle(0);
	handle.hClass(CMaterial::CMaterialClassID::GetClassId().ID());

	emission_enabled = false;
	SET_MATERIAL(emission,CGL_BLACK_MATERIAL)

	if (material >= CGL_NO_REAL_MATERIAL)
	{
		ambient_enabled = false;
		diffuse_enabled = false;
		specular_enabled = false;
		SET_MATERIAL(ambient,CGL_BLACK_MATERIAL)
		SET_MATERIAL(diffuse,CGL_BLACK_MATERIAL)
		SET_MATERIAL(specular,CGL_BLACK_MATERIAL)
		shininess[0] = 0;
	}
	else
	{
		ambient_enabled = true;
		diffuse_enabled = true;
		specular_enabled = true;
		ambient = REAL_MATERIALS[material].ambient;
		diffuse = REAL_MATERIALS[material].diffuse;
		specular = REAL_MATERIALS[material].specular;
		shininess[0] = REAL_MATERIALS[material].shininess;
	}
}

CMaterial::CMaterial(BASE_MATERIAL amb,
					 BASE_MATERIAL diff,
					 BASE_MATERIAL spec,
					 float shi,
					 BASE_MATERIAL em,
					 const CPersistence::CPersistenceClassID &ID,
					 const std::string& name):
	CPersistence(ID,name)
{
    m_bRebuild = true;

	handle.handle(0);
	handle.hClass(CMaterial::CMaterialClassID::GetClassId().ID());

	if (amb>=CGL_NO_MATERIAL)
	{
		ambient_enabled = false;
		SET_MATERIAL(ambient,CGL_BLACK_MATERIAL)
	}
	else
	{
		ambient_enabled = true;
		SET_MATERIAL(ambient,amb)
	}
	if (diff>=CGL_NO_MATERIAL)
	{
		diffuse_enabled = false;
		SET_MATERIAL(diffuse,CGL_BLACK_MATERIAL)
	}
	else
	{
		diffuse_enabled = true;
		SET_MATERIAL(diffuse,diff)
	}
	if (spec>=CGL_NO_MATERIAL)
	{
		specular_enabled = false;
		SET_MATERIAL(specular,CGL_BLACK_MATERIAL)
	}
	else
	{
		specular_enabled = true;
		SET_MATERIAL(specular,spec)
	}
	if (em>=CGL_NO_MATERIAL)
	{
		emission_enabled = false;
		SET_MATERIAL(emission,CGL_BLACK_MATERIAL)
	}
	else
	{
		emission_enabled = true;
		SET_MATERIAL(emission,em)
	}
	shininess[0] = shi;
}

CMaterial::CMaterial(BASE_MATERIAL amb,
					 BASE_MATERIAL diff,
					 BASE_MATERIAL spec,
					 float shi,
					 BASE_MATERIAL em,
					 const std::string& name):
	CPersistence(materialID,name)
{
    m_bRebuild = true;

	handle.handle(0);
	handle.hClass(CMaterial::CMaterialClassID::GetClassId().ID());

	if (amb>=CGL_NO_MATERIAL)
	{
		ambient_enabled = false;
		SET_MATERIAL(ambient,CGL_BLACK_MATERIAL)
	}
	else
	{
		ambient_enabled = true;
		SET_MATERIAL(ambient,amb)
	}
	if (diff>=CGL_NO_MATERIAL)
	{
		diffuse_enabled = false;
		SET_MATERIAL(diffuse,CGL_BLACK_MATERIAL)
	}
	else
	{
		diffuse_enabled = true;
		SET_MATERIAL(diffuse,diff)
	}
	if (spec>=CGL_NO_MATERIAL)
	{
		specular_enabled = false;
		SET_MATERIAL(specular,CGL_BLACK_MATERIAL)
	}
	else
	{
		specular_enabled = true;
		SET_MATERIAL(specular,spec)
	}
	if (em>=CGL_NO_MATERIAL)
	{
		emission_enabled = false;
		SET_MATERIAL(emission,CGL_BLACK_MATERIAL)
	}
	else
	{
		emission_enabled = true;
		SET_MATERIAL(emission,em)
	}
	shininess[0] = shi;
}

CMaterial::~CMaterial()
{
    // handle should be released ... but needs a valid GL context!
    DEL_LIST
}

const CMaterial& CMaterial::operator=(const CMaterial& rsh)
{
    m_bRebuild = true;

	ambient = rsh.ambient;
	diffuse = rsh.diffuse;
	specular = rsh.specular;
	shininess = rsh.shininess;
	emission = rsh.emission;

	ambient_enabled = rsh.ambient_enabled;
	diffuse_enabled = rsh.diffuse_enabled;
	specular_enabled = rsh.specular_enabled;
	emission_enabled = rsh.emission_enabled;

	CATCH_GL_ERROR

	return *this;
}


void CMaterial::setAmbient(float r,float g,float b,float a)
{
    m_bRebuild = true;

	if ((r==0)&&(g==0)&&(b==0)&&(a==0))
		ambient_enabled = false;
	else
	{
		SET_COLOR(ambient)
		ambient_enabled = true;
	}

	CATCH_GL_ERROR
}

void CMaterial::setDiffuse(float r,float g,float b,float a)
{
    m_bRebuild = true;

	if ((r==0)&&(g==0)&&(b==0)&&(a==0))
		diffuse_enabled = false;
	else
	{
		diffuse_enabled = true;
		SET_COLOR(diffuse)
	}

	CATCH_GL_ERROR
}

void CMaterial::setSpecular(float r,float g,float b,float a)
{
    m_bRebuild = true;

	if ((r==0)&&(g==0)&&(b==0)&&(a==0))
		specular_enabled = false;
	else
	{
		specular_enabled = true;
		SET_COLOR(specular)
	}

	CATCH_GL_ERROR
}

void CMaterial::setEmission(float r,float g,float b,float a)
{
    m_bRebuild = true;

	if ((r==0)&&(g==0)&&(b==0)&&(a==0))
		emission_enabled = false;
	else
	{
		emission_enabled = true;
		SET_COLOR(emission)
	}

	CATCH_GL_ERROR
}

void CMaterial::setShininess(float exp)
{
    m_bRebuild = true;

	shininess.r = exp;

	CATCH_GL_ERROR
}

void CMaterial::darken(float d)
{
	float dd = MIN(MAX(d,0.0f),1.0f);

    m_bRebuild = true;

	DARKEN(ambient)
	DARKEN(diffuse)
	DARKEN(specular)
	DARKEN(emission)

	CATCH_GL_ERROR
}

void CMaterial::lighten(float l)
{
	float dd = MAX(l,1.0f);

    m_bRebuild = true;

	LIGHTEN(ambient)
	LIGHTEN(diffuse)
	LIGHTEN(specular)
	LIGHTEN(emission)

	CATCH_GL_ERROR
}

void CMaterial::glRender()
{
    if(m_bRebuild)
    {
        DEL_LIST
        handle.handle(0);
        m_bRebuild = false;
    }
	else if (pCurrentMaterial == this)
		return;

	pCurrentMaterial = this;

	//if (glIsList(handle.handle))
    //  This is much faster ...
    if (handle.handle() > 0)
		glCallList(handle.handle());
	else
	{
		handle.handle(glGenLists(1));
		glNewList(handle.handle(),GL_COMPILE_AND_EXECUTE);
			if (ambient_enabled)
				glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
			if (diffuse_enabled)
				glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
			if (specular_enabled)
			{
				glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
				glMaterialfv(GL_FRONT,GL_SHININESS,shininess);
			}
			if (emission_enabled)
				glMaterialfv(GL_FRONT,GL_EMISSION,emission);
		glEndList();
	}

	CATCH_GL_ERROR
}


CMaterial::operator RAPTOR_HANDLE() const
{
	RAPTOR_HANDLE handle(getId().ID(),(void*)this);
	return handle;
}


bool CMaterial::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);

	o << ambient;
	o << diffuse;
	o << specular;
	o << emission;
	o << shininess;

	if (ambient_enabled)
		o<<'y';
	else
		o<<'n';

	if (diffuse_enabled)
		o<<'y';
	else
		o<<'n';

	if (specular_enabled)
		o<<'y';
	else
		o<<'n';

	if (emission_enabled)
		o<<'y';
	else
		o<<'n';

	return true;
}

bool CMaterial::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "Ambient")
			io >> ambient;
		else if (data == "Diffuse")
			io >> diffuse;
		else if (data == "Specular")
			io >> specular;
		else if (data == "Emission")
			io >> emission;
		else if (data == "Shininess")
			io >> shininess;
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}

