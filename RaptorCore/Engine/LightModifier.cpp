// LightModifier.cpp: implementation of the CLightModifier class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_LIGHTMODIFIER_H__3A911E5A_0ABE_4E28_B446_ADC42463246D__INCLUDED_)
    #include "LightModifier.h"
#endif

#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "GLHierarchy/Projector.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CLightModifier::CLightModifierClassID lightId;
static CPersistentType<CLightModifier> lightModifierFactory(lightId);


CLightModifier::CLightModifier(const std::string& name)
    :CModifier(CGL_TIME_CONSTANT,0,0,0,0,NULL,lightId,name),
    m_pLight(NULL)
{

}

CLightModifier::~CLightModifier()
{
	if (m_pLight != NULL)
		m_pLight->unregisterDestruction(this);
}

void CLightModifier::unLink(const CPersistence* obj)
{
    if (obj == static_cast<CPersistence*>(m_pLight))
    {
        m_pLight = NULL;
    }
}

bool CLightModifier::setLight(CLight *light)
{
	if (m_pLight != NULL)
		m_pLight->unregisterDestruction(this);

	m_pLight = light;
    if (light != NULL)
		light->registerDestruction(this);

	return (light != NULL);
}

void RAPTOR_FASTCALL CLightModifier::deltaTime(float dt)
{
	vector<LIGHT_TIME_ACTIONS>::const_iterator itr = m_lightActions.begin();
	while (itr != m_lightActions.end())
	{
		const LIGHT_TIME_ACTIONS& va = (*itr);

		m_timeFunction = va.tx;
		CModifier::deltaTime(dt);
		float f_tx = m_ft;

		m_timeFunction = va.ty;
		CModifier::deltaTime(dt);
		float f_ty = m_ft;

		m_timeFunction = va.tz;
		CModifier::deltaTime(dt);
		float f_tz = m_ft;

		itr++;

		switch(va.action)
		{
			case SET_POSITION:
			{
                
                if (m_pLight != NULL)
                {
                    GL_COORD_VERTEX lp(f_tx,f_ty,f_tz,1.0f);
                    m_pLight->setLightPosition(lp);
                }
				break;
			}
			case SET_DIRECTION:
			{
                if (m_pLight != NULL)
                {
                    GL_COORD_VERTEX ld(f_tx,f_ty,f_tz,1.0f);
                    m_pLight->setLightDirection(ld);
                }
				break;
			}
            default:
                break;
        }
	}
}


void CLightModifier::clearActions(void)
{
	m_lightActions.clear();
}

void CLightModifier::addAction(	CLightModifier::LIGHT_MODIFIER_ACTIONS action,
								CModifier::TIME_FUNCTION tx,
								CModifier::TIME_FUNCTION ty,
								CModifier::TIME_FUNCTION tz)
{
	LIGHT_TIME_ACTIONS va;
	va.action = action;
	va.tx = tx;
	va.ty = ty;
	va.tz  =tz;

	m_lightActions.push_back(va);
}

bool CLightModifier::exportObject(CRaptorIO& o)
{
	return CModifier::exportObject(o);
}


void CLightModifier::importAction(CRaptorIO& io)
{
    string name;

	CLightModifier::LIGHT_MODIFIER_ACTIONS action = CLightModifier::SET_POSITION;
	CModifier::TIME_FUNCTION tx;
	CModifier::TIME_FUNCTION ty;
	CModifier::TIME_FUNCTION tz;
    CModifier::TIME_FUNCTION *currentFunction = NULL;

    io >> name;
	string data = io.getValueName();

	while (!data.empty())
	{
		if (data == "action")
		{
			io >> name;
			if (name == "Position")
				action = SET_POSITION;
			else if (name == "Direction")
				action = SET_DIRECTION;
		}
		else if (data == "TimeFunction")
		{
            if (currentFunction == NULL)
                currentFunction = &tx;
            else if (currentFunction == &tx)
                currentFunction = &ty;
            else if (currentFunction == &ty)
                currentFunction = &tz;
            else
                currentFunction = NULL;

            importTimeFunction(*currentFunction,io);

			io >> name;
		}
		else
			io >> name;

		data = io.getValueName();
	}
	addAction(action,tx,ty,tz);
	io >> name;
}

bool CLightModifier::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data == "light")
        {
            io >> name;
			CPersistence* object = CPersistence::FindObject(name);
            if ((object != NULL) && 
				(object->getId().isSubClassOf(CLight::CLightClassID::GetClassId())))
                setLight((CLight*)object);
        }
		else if (data == "Action")
			importAction(io);
		else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

	return true;
}
