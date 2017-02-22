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

	class ParameterBase
	{
	public:
		virtual ~ParameterBase() {};
		virtual uint64_t size(void) const { return 0; };
		virtual const void* addr(void) const { return NULL; };
		virtual ParameterBase* clone(void) const { return NULL; };

		template <class T> bool isA(const T &t) const
		{
			return getTypeId() == Parameter<T>::TypeId();
		};

	protected:
		ParameterBase() {};
		ParameterBase(const ParameterBase&) {};
		virtual const type_info& getTypeId(void) const { return typeid(void*); };
	};

	template <class P>
	class Parameter : public CProgramParameters::ParameterBase
	{
	public:
		Parameter(const P &param) :p(param) {};
		virtual ~Parameter() {};

		static const type_info& TypeId(void) { static P _p; return typeid(_p); };
		virtual const type_info& getTypeId(void) const { return typeid(p); };

		virtual uint64_t size(void) const { return sizeof(p); };
		virtual const void* addr(void) const { return &p; };
		virtual ParameterBase* clone(void) const
		{
			return new Parameter<P>(p);
		};

		Parameter<P>& operator=(const P &_p)
		{
			p = _p; return *this;
		};

	public:
		P	p;
	};

	//! This structure defines a parameter value
	class CParameterValue : public CProgramParameters::ParameterBase
	{
	public:
		//!	Constructor
		CParameterValue();
		//!	Copy constructor
		CParameterValue(const CParameterValue&);
		//!	Assignment operator
		CParameterValue& operator=(const CParameterValue&);
		//!	Destructor
		virtual ~CParameterValue();

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
	};

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
	CParameterValue& operator[](unsigned int v);
	const CParameterValue& operator[](unsigned int v) const;

	//! Parameters assignment.
	CProgramParameters& operator=(const CProgramParameters& params);

private:
	CProgramParameters(const CProgramParameters&) {};

	//! There should not be a max value, it is inherited from  NV_vertex_program first drafts.
	std::vector<CParameterValue> mValues;
};

RAPTOR_NAMESPACE_END

#endif	//	!defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)