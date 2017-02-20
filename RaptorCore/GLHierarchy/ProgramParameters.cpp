// ProgramParameters.cpp: implementation of the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static datas
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE

static CProgramParameters::PROGRAM_PARAMETER_VALUE noValue;

CProgramParameters::PROGRAM_PARAMETER_VALUE& CProgramParameters::operator[](unsigned int v)
{
	if (mValues.size() > v)
		return mValues[v];
	else
	{
		noValue.name = "";
		noValue.kind = VECTOR;
		noValue.locationIndex = -1;
		noValue.locationType = GL_FLOAT_VEC4_ARB;
		noValue.vector = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f);
		IDENT_MATRIX(noValue.matrix);
		noValue.attribute = POSITION;
		noValue.sampler = CTextureUnitSetup::IMAGE_UNIT_0;
		return noValue;
	}
}

const CProgramParameters::PROGRAM_PARAMETER_VALUE& CProgramParameters::operator[](unsigned int v) const
{
	if (mValues.size() > v)
		return mValues[v];
	else
	{
		noValue.name = "";
		noValue.kind = VECTOR;
		noValue.locationIndex = -1;
		noValue.locationType = GL_FLOAT_VEC4_ARB;
		noValue.vector = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f);
		IDENT_MATRIX(noValue.matrix);
		noValue.attribute = POSITION;
		noValue.sampler = CTextureUnitSetup::IMAGE_UNIT_0;
		return noValue;
	}
}

bool CProgramParameters::addParameter(const std::string& name, const GL_COORD_VERTEX& vertex)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = VECTOR;
	value.vector = vertex;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, const GL_MATRIX& matrix)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = MATRIX;
	value.matrix = matrix;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, GL_VERTEX_ATTRIB attribute)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = ATTRIBUTE;
	value.attribute = attribute;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler)
{
	PROGRAM_PARAMETER_VALUE value;
	value.name = name;
	value.kind = SAMPLER;
	value.sampler = sampler;
	value.locationIndex = -1;
	value.locationType = GL_FLOAT_VEC4_ARB;

	mValues.push_back(value);

	return true;
}

CProgramParameters& CProgramParameters::operator=(const CProgramParameters& params)
{
	// The case that is not handled is when
	//	params is used to update only a part of currently registered parameters.
	// TODO: try to solve by setting m_bRelinked = true when params has more parameters than mValues.
	if (mValues.size() != params.getNbParameters())
		mValues = params.mValues;

	for (size_t i = 0; i < params.getNbParameters(); i++)
	{
		for (size_t j = 0; j < mValues.size(); j++)
		{
			if (mValues[j].name == params[i].name)
			{
				switch (mValues[j].kind)
				{
					case CProgramParameters::MATRIX:
						mValues[j].matrix = params[i].matrix;
						break;
					case CProgramParameters::SAMPLER:
						mValues[j].sampler = params[i].sampler;
						break;
					case CProgramParameters::VECTOR:
						mValues[j].vector = params[i].vector;
						break;
					case CProgramParameters::ATTRIBUTE:
						mValues[j].attribute = params[i].attribute;
						break;
				}
			}
		}
	}

	return *this;
}
