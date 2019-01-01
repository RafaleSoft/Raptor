// ViewModifier.cpp: implementation of the CViewModifier class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif

#if !defined(AFX_VIEWMODIFIER_H__115939E5_BC5E_40A5_B2A8_7CEBD6C4009A__INCLUDED_)
	#include "ViewModifier.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DEngine.h"
#endif
#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "System/RaptorDisplay.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CViewModifier::CViewModifierClassID viewId;
static CPersistentType<CViewModifier,CObject3DInstance> viewModifierFactory(viewId,&CViewModifier::getObject);

CViewModifier::CViewModifier(const std::string& name)
	:CModifier(CGL_TIME_CONSTANT,0,0,0,0,NULL,viewId,name),
	m_isInstance(false),m_object(NULL)
{
}

CViewModifier::~CViewModifier()
{

}


void RAPTOR_FASTCALL CViewModifier::deltaTime(float dt)
{
	CGenericMatrix<float>	transform;
	transform.Ident();

	vector<VIEW_TIME_ACTIONS>::const_iterator itr = m_viewActions.begin();
	while (itr != m_viewActions.end())
	{
		const VIEW_TIME_ACTIONS& va = (*itr);

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
			case TRANSLATEABSOLUTE:
			{
				transform[3] = f_tx;
				transform[7] = f_ty;
				transform[11] = f_tz;
				break;
			}
			case TRANSLATE_VIEW:
			{
				transform[3] += f_tx;
				transform[7] += f_ty;
				transform[11] += f_tz;
				break;
			}
			case ROTATE_VIEW:
			{
				if (f_tx != 0)
				{
					float u = (float)cos(TO_RADIAN(f_tx));
					float v = (float)sin(TO_RADIAN(f_tx));
					//					*	m_transform
					//	1	0		0	
					//	0	cos		-sin
					//	0	sin		cos

					float aux = u*transform[4] - v*transform[8];
					transform[8] = v*transform[4] + u*transform[8];
					transform[4] = aux;

					aux = u*transform[5] - v*transform[9];
					transform[9] = v*transform[5] + u*transform[9];
					transform[5] = aux;

					aux = u*transform[6] - v*transform[10];
					transform[10] = v*transform[6] + u*transform[10];
					transform[6] = aux;
				}

				if (f_ty != 0)
				{
					float u = (float)cos(TO_RADIAN(f_ty));
					float v = (float)sin(TO_RADIAN(f_ty));
					//					*	m_transform
					//	cos		0	sin	
					//	0		1	0
					//	-sin	0	cos

					float aux = u*transform[0] + v*transform[8];
					transform[8] = -v*transform[0] + u*transform[8];
					transform[0] = aux;

					aux = u*transform[1] + v*transform[9];
					transform[9] = -v*transform[1] + u*transform[9];
					transform[1] = aux;

					aux = u*transform[2] + v*transform[10];
					transform[10] = -v*transform[2] + u*transform[10];
					transform[2] = aux;
				}

				if (f_tz != 0)
				{
					float u = (float)cos(TO_RADIAN(f_tz));
					float v = (float)sin(TO_RADIAN(f_tz));
					//					*	m_transform
					// cos	-sin	0	
					// sin	cos		0
					//	0	0		1

					float aux = u*transform[0] - v*transform[4];
					transform[4] = v*transform[0] + u*transform[4];
					transform[0] = aux;

					aux = u*transform[1] - v*transform[5];
					transform[5] = v*transform[1] + u*transform[5];
					transform[1] = aux;

					aux = u*transform[2] - v*transform[6];
					transform[6] = v*transform[2] + u*transform[6];
					transform[2] = aux;
				}
				break;
			}
			case RESET_TRANSFORM:
			{
				m_object->resetTransform();
				break;
			}
            default:
                break;
		}
	}

    if (m_object != NULL)
    {   
    	GL_MATRIX T;
    	C3DEngine::Generic_to_MATRIX(T,transform);
    	m_object->transform(T);
    }
}


void CViewModifier::clearActions(void)
{
	m_viewActions.clear();
}

void CViewModifier::addAction(	CViewModifier::VIEW_MODIFIER_ACTIONS action,
								CModifier::TIME_FUNCTION tx,
								CModifier::TIME_FUNCTION ty,
								CModifier::TIME_FUNCTION tz)
{
	VIEW_TIME_ACTIONS va;
	va.action = action;
	va.tx = tx;
	va.ty = ty;
	va.tz  =tz;

	m_viewActions.push_back(va);
}

CObject3DInstance* CViewModifier::getObject(void) const 
{
	return m_object;
}

bool CViewModifier::setObject(CObject3D *object)
{
	if (object == NULL)
		//	OpenGL Modelview assumed
		return false;

	if ((!m_isInstance) && (m_object != NULL))
	{
		m_object->instantiate(NULL);
		delete m_object;
	}

	m_isInstance = (object->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId()));
		 
	if (!m_isInstance)
		m_object = new CObject3DInstance(object,object->getName()+"_VIEWMODIFIER");
	else
		m_object = (CObject3DInstance*)object;

	return true;
}

bool CViewModifier::exportObject(CRaptorIO& o)
{
	CModifier::exportObject(o);

	if (m_isInstance)
		o<<(char)1;
	else
		o<<(char)0;

	return true;
}


void CViewModifier::importAction(CRaptorIO& io)
{
    string name;

	CViewModifier::VIEW_MODIFIER_ACTIONS action = CViewModifier::RESET_TRANSFORM;
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
			if (name == "Rotation")
				action = ROTATE_VIEW;
			else if (name == "Translation")
				action = TRANSLATE_VIEW;
			else if (name == "AbsoluteTranslation")
				action = TRANSLATEABSOLUTE;
			else if (name == "Reset")
				action = RESET_TRANSFORM;
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

bool CViewModifier::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
		else if (data == "object")
        {
            io >> name;
			CPersistence* object = CPersistence::FindObject(name);
            if (object != NULL)
            {
				const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
				setObject(op);
            }
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

