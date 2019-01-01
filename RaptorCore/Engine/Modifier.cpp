// Modifier.cpp: implementation of the CModifier class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


RAPTOR_NAMESPACE

static CModifier::CModifierClassID modifierID;
const CPersistence::CPersistenceClassID& CModifier::CModifierClassID::GetClassId(void)
{
	return modifierID;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CModifier::CModifier(TIME_FUNCTION_TYPE	timeFunction,
					             float timeArg0,	float timeArg1,
					             float timeArg2,    float timeArg3,
                                 USER_FUNCTION  pUserFunction ,
								 const CPersistence::CPersistenceClassID& id,
					             const std::string&	name)
	:CPersistence(id,name),m_ft(timeArg0)
{
	m_timeFunction.timeFunction = timeFunction;
	m_timeFunction.period = 0;
	m_timeFunction.a0 = timeArg0;
	m_timeFunction.a1 = timeArg1;
	m_timeFunction.a2 = timeArg2;
	m_timeFunction.a3 = timeArg3;
    m_timeFunction.pUserFunction = pUserFunction;
}

CModifier::~CModifier()
{
}

void RAPTOR_FASTCALL CModifier::deltaTime(float dt)
{
	float gtime = this->GetTime();

	switch(m_timeFunction.timeFunction)
	{
		case CGL_TIME_CONSTANT:
			m_ft = m_timeFunction.a0;
			break;
		case CGL_TIME_LINEAR:
			m_ft = m_timeFunction.a1 * gtime + m_timeFunction.a0;
			break;
		case CGL_TIME_QUADRIC:
			m_ft = (m_timeFunction.a2 * gtime + m_timeFunction.a1) * gtime + m_timeFunction.a0;
			break;
		case CGL_TIME_CUBIC:
			m_ft = ((m_timeFunction.a3 * gtime + m_timeFunction.a2) * gtime + m_timeFunction.a1 ) * gtime + m_timeFunction.a0;
			break;
		case CGL_TIME_COSINE:
			m_ft = (float)(m_timeFunction.a3  + m_timeFunction.a2 * cos(m_timeFunction.a1 * gtime + m_timeFunction.a0));
			break;
		case CGL_TIME_SINE:
			m_ft = (float)(m_timeFunction.a3  + m_timeFunction.a2 * sin(m_timeFunction.a1 * gtime + m_timeFunction.a0));
			break;
		case CGL_TIME_TRIANGLE:
			m_timeFunction.period += dt;
			if (m_timeFunction.period < m_timeFunction.a2)
				m_ft = m_timeFunction.a1 * m_timeFunction.period / m_timeFunction.a2 - m_timeFunction.a0;
			else
				m_ft = m_timeFunction.a1 * (m_timeFunction.period - m_timeFunction.a3)/(m_timeFunction.a2 - m_timeFunction.a3) - m_timeFunction.a0;
			if (m_timeFunction.period > m_timeFunction.a3)
				m_timeFunction.period = 0;
			break;
		case CGL_TIME_SQUARE:
			m_timeFunction.period += dt;
			if (m_timeFunction.period < m_timeFunction.a2)
				m_ft = m_timeFunction.a0;
			else
				m_ft = m_timeFunction.a0 + m_timeFunction.a1;
			if (m_timeFunction.period > m_timeFunction.a3)
				m_timeFunction.period = 0;
			break;
        case CGL_TIME_USER:
            m_ft = m_timeFunction.pUserFunction(gtime);
            break;
		default:
			break;
	}
}

void CModifier::importTimeFunction(CModifier::TIME_FUNCTION& tf, CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
	while (!data.empty())
	{
        if (data == "type")
		{
			io >> name;
			if (name == "time_constant") tf.timeFunction = CGL_TIME_CONSTANT;
			else if (name == "time_linear") tf.timeFunction = CGL_TIME_LINEAR;
			else if (name == "time_quadric") tf.timeFunction = CGL_TIME_QUADRIC;
			else if (name == "time_cubic") tf.timeFunction = CGL_TIME_CUBIC;
			else if (name == "time_cosine") tf.timeFunction = CGL_TIME_COSINE;
			else if (name == "time_sine") tf.timeFunction = CGL_TIME_SINE;
			else if (name == "time_triangle") tf.timeFunction = CGL_TIME_TRIANGLE;
			else if (name == "time_square") tf.timeFunction = CGL_TIME_SQUARE;
		}
		else if (data == "a0") io >> tf.a0;
		else if (data == "a1") io >> tf.a1;
		else if (data == "a2") io >> tf.a2;
		else if (data == "a3") io >> tf.a3;
        else
			io >> name;

        data = io.getValueName();
	}

	io >> name;
}

bool CModifier::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);

	o << m_timeFunction.timeFunction;
	o << m_ft;
	o << m_timeFunction.period;
	o << m_timeFunction.a0;
	o << m_timeFunction.a1;
	o << m_timeFunction.a2;
	o << m_timeFunction.a3;

	return true;
}

bool CModifier::importObject(CRaptorIO& i)
{
	CPersistence::importObject(i);

    importTimeFunction(m_timeFunction,i);

	return true;
}
