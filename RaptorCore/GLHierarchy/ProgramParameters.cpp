/***************************************************************************/
/*                                                                         */
/*  ProgramParameters.cpp                                                  */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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

#if (_MSC_VER <= 1500) || defined(LINUX)
size_t CProgramParameters::CParameterBase::hash_value(const std::string& s)
{
	const char *ptr = s.c_str();
	const char *end = ptr + s.size();
	size_t hash = 2166136261U;
	while (ptr != end)
		hash = 16777619U * hash ^ (size_t)*ptr++;
	return (hash);
}
#endif

CProgramParameters::CParameterBase::CParameterBase()
	:m_name(""), locationIndex(-1), locationType(0), locationSize(0)
{
}

CProgramParameters::CParameterBase::CParameterBase(const CProgramParameters::CParameterBase& p)
	:m_name(p.m_name),
	locationIndex(p.locationIndex),
	locationType(p.locationType),
	locationSize(p.locationSize)
{
}

size_t CProgramParameters::CParameterBase::getTypeId(void) const
{
#if (_MSC_VER <= 1500) || defined(LINUX)
	const std::string nm = std::string(typeid(void*).name());
	static size_t ti = hash_value(nm);
#else
	static size_t ti = typeid(void*).hash_code();
#endif		
	return ti;
}

CProgramParameters::CParameterBase& CProgramParameters::CParameterBase::operator=(const CProgramParameters::CParameterBase& p)
{
	m_name = p.m_name;
	locationIndex = p.locationIndex;
	locationType = p.locationType;
	locationSize = p.locationSize;
	return *this;
}

static CProgramParameters::CParameter<void*> noValue(0);

//////////////////////////////////////////////////////////////////////
// ProgramParameters implementation.
//////////////////////////////////////////////////////////////////////

CProgramParameters::CParameterBase& CProgramParameters::operator[](size_t v)
{
	if (m_parameters.size() > v)
		return *(m_parameters[v]);
	else
		return noValue;
}

const CProgramParameters::CParameterBase& CProgramParameters::operator[](size_t v) const
{
	if (m_parameters.size() > v)
		return *(m_parameters[v]);
	else
		return noValue;
}

void CProgramParameters::clear(void)
{
	for (size_t i = 0; i < m_parameters.size(); i++)
	{
		CProgramParameters::CParameterBase* param = m_parameters[i];
		if (NULL != param)
			delete param;
	}

	m_parameters.clear();
}

bool CProgramParameters::addParameter(const CParameterBase& param)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	for (size_t i = 0; i < m_parameters.size(); i++)
	{
		if (m_parameters[i]->name() == param.name())
		{
			RAPTOR_WARNING(	CPersistence::CPersistenceClassID::GetClassId(),
							"Duplicate parameter name");
			return false;
		}
	}
#endif

	m_parameters.push_back(param.clone());
	return true;
}

CProgramParameters& CProgramParameters::operator=(const CProgramParameters& params)
{
	if (m_parameters.size() != params.getNbParameters())
	{
		for (size_t i = 0; i < m_parameters.size(); i++)
		{
			CProgramParameters::CParameterBase* param = m_parameters[i];
			if (NULL != param)
				delete param;
		}
		m_parameters.clear();

		for (size_t i = 0; i < params.m_parameters.size(); i++)
			m_parameters.push_back(params.m_parameters[i]->clone());
	}
	else
		for (size_t i = 0; i < params.m_parameters.size(); i++)
		{
			for (size_t j = 0; j < m_parameters.size(); j++)
			{
				if (m_parameters[j]->name() == params.m_parameters[i]->name())
				{
					m_parameters[j]->copy(*params.m_parameters[i]);
					break;
				}
			}
		}

	return *this;
}

bool CProgramParameters::updateParameters(const CProgramParameters& params)
{
	if (m_parameters.empty() || (0 == params.getNbParameters()))
		return false;

	bool atLeastOneAssignment = false;
	for (size_t i = 0; i < params.m_parameters.size(); i++)
	{
		for (size_t j = 0; j < m_parameters.size(); j++)
		{
			if (m_parameters[j]->name() == params.m_parameters[i]->name())
			{
				m_parameters[j]->copy(*params.m_parameters[i]);
				atLeastOneAssignment = true;
				break;
			}
		}
	}

	return atLeastOneAssignment;
}