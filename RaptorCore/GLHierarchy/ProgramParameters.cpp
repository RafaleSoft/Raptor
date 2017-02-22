// ProgramParameters.cpp: implementation of the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static datas
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE

static CProgramParameters::CParameterValue noValue;

CProgramParameters::CParameterValue::CParameterValue()
	:ParameterBase(),
	name(""),
	kind(VECTOR),
	locationIndex(-1),
	locationType(GL_FLOAT_VEC4_ARB),
	vector(0.0f, 0.0f, 0.0f, 1.0f),
	attribute(POSITION),
	sampler(CTextureUnitSetup::IMAGE_UNIT_0)
{
	IDENT_MATRIX(noValue.matrix);
}

CProgramParameters::CParameterValue::CParameterValue(const CParameterValue& value)
	:ParameterBase(),
	name(value.name),
	kind(value.kind),
	locationIndex(value.locationIndex),
	locationType(value.locationType),
	vector(value.vector),
	attribute(value.attribute),
	sampler(value.sampler),
	matrix(value.matrix)
{
}

CProgramParameters::CParameterValue& CProgramParameters::CParameterValue::operator=(const CParameterValue& value)
{
	if (&value != this)
	{
		name = value.name;
		kind = value.kind;
		locationIndex = value.locationIndex;
		locationType = value.locationType;
		vector = value.vector;
		attribute = value.attribute;
		sampler = value.sampler;
		matrix = value.matrix;
	}

	return *this;
}

CProgramParameters::CParameterValue::~CParameterValue()
{
}

CProgramParameters::CParameterValue& CProgramParameters::operator[](unsigned int v)
{
	if (mValues.size() > v)
		return mValues[v];
	else
	{
		noValue = CProgramParameters::CParameterValue();
		return noValue;
	}
}

const CProgramParameters::CParameterValue& CProgramParameters::operator[](unsigned int v) const
{
	if (mValues.size() > v)
		return mValues[v];
	else
	{
		noValue = CProgramParameters::CParameterValue();
		return noValue;
	}
}

bool CProgramParameters::addParameter(const std::string& name, const GL_COORD_VERTEX& vertex)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < mValues.size(); i++)
	{
		if (mValues[i].name == name)
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Duplicate parameter name");
			return false;
		}
	}
#endif

	CProgramParameters::CParameterValue value;
	value.name = name;
	value.vector = vertex;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, const GL_MATRIX& matrix)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < mValues.size(); i++)
	{
		if (mValues[i].name == name)
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Duplicate parameter name");
			return false;
		}
	}
#endif

	CParameterValue value;
	value.name = name;
	value.kind = MATRIX;
	value.matrix = matrix;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, GL_VERTEX_ATTRIB attribute)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < mValues.size(); i++)
	{
		if (mValues[i].name == name)
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Duplicate parameter name");
			return false;
		}
	}
#endif

	CParameterValue value;
	value.name = name;
	value.kind = ATTRIBUTE;
	value.attribute = attribute;

	mValues.push_back(value);

	return true;
}

bool CProgramParameters::addParameter(const std::string& name, CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < mValues.size(); i++)
	{
		if (mValues[i].name == name)
		{
			Raptor::GetErrorManager()->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   "Duplicate parameter name");
			return false;
		}
	}
#endif

	CParameterValue value;
	value.name = name;
	value.kind = SAMPLER;
	value.sampler = sampler;

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
