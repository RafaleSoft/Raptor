// ProgramParameters.h: interface for the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
#define AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif

RAPTOR_NAMESPACE_BEGIN



class RAPTOR_API CProgramParameters
{
public:
	//!  Fixed pipeline attributes binding shortcut that can help switching
	//! easily between the asm shaders and conventional pipelines which are exclusive.
	//! Nevertheless, GLSL bindings can coexist with conventionnal mapping.
	typedef enum GL_VERTEX_ATTRIB_t
	{
		POSITION,
		WEIGHTS,
		NORMAL,
		PRIMARY_COLOR,
		SECONDARY_COLOR,
		FOG_COORDINATE,
		ADDITIONAL_PARAM1,
		ADDITIONAL_PARAM2,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7
	} GL_VERTEX_ATTRIB;

	//! This type defines the kind of parameter value defined here under
	typedef enum PARAMETER_KIND_t
	{
		VECTOR,
		MATRIX,
		ATTRIBUTE,
		SAMPLER
	} PARAMETER_KIND;

	//! This structure defines a parameter value
	typedef struct PROGRAM_PARAMETER_VALUE_t
	{
		//! The name of the value: it is matched to variables of the program ( @see GLSL )
		string				name;
		//! The kind of parameter : depending on the kind, only one of the four values here under is valid
		PARAMETER_KIND		kind;

		//! Parameter locations can be retrieven once per link and reused.
		int				locationIndex;
		unsigned int	locationType;

		//! User values : a vector, a matrix, an attribute of a texture image unit.
		GL_COORD_VERTEX		vector;
		GL_MATRIX			matrix;
		GL_VERTEX_ATTRIB	attribute;
		CTextureUnitSetup::TEXTURE_IMAGE_UNIT	sampler;
	} PROGRAM_PARAMETER_VALUE;

public:
	//! Default constructor. The parameter set is uninitialised, user
	//! must call the proper init method from the relevant shader.
	CProgramParameters() :mValues() {};
	virtual ~CProgramParameters() {};

	//!	Remove all parameters
	void clear(void) { mValues.clear(); }

	//! Number of paremeters
	size_t getNbParameters(void) const { return mValues.size(); };

	//! Add a vertex as a parameter
	bool addParameter(const std::string& name, const GL_COORD_VERTEX& vertex);

	//! Add a matrix as a parameter
	bool addParameter(const std::string& name, const GL_MATRIX& matrix);

	//! Add an attribute as a parameter
	bool addParameter(const std::string& name, GL_VERTEX_ATTRIB attrib);

	//! Add a sampler as a parameter
	bool addParameter(const std::string& name, CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler);

	//! Access to vector parameters.
	PROGRAM_PARAMETER_VALUE& operator[](unsigned int v);
	const PROGRAM_PARAMETER_VALUE& operator[](unsigned int v) const;

	//! Parameters assignment.
	CProgramParameters& operator=(const CProgramParameters& params);

private:
	CProgramParameters(const CProgramParameters&) {};

	//! There should not be a max value, it is inherited from  NV_vertex_program first drafts.
	std::vector<PROGRAM_PARAMETER_VALUE> mValues;
};

RAPTOR_NAMESPACE_END

#endif	//	!defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)