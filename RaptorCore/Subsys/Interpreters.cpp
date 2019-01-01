//////////////////////////////////////////////////////////////////////
// Analyser implementation
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#pragma warning (disable : 4711)	// automatic inline expansion warning
#pragma warning (disable : 4663)	// C++/C# changes
#pragma warning (disable : 4018)	// signed/unsigned mismatch
#pragma warning (disable : 4244)	// possible data loss conversion


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#if !defined(__INTERPRETER_H__)
    #include "Interpreters.h"
#endif

#include "ObjectModels/RaptorScript_tree.h"
#include <iostream>
#include <strstream>

#pragma warning (disable : 4711)	// automatic inline expansion warning
#pragma warning (disable : 4663)	// C++/C# changes
#pragma warning (disable : 4018)	// signed/unsigned mismatch
#pragma warning (disable : 4244)	// possible data loss conversion


//////////////////////////////////////////////////////////////////////
// Local data

RAPTOR_NAMESPACE_BEGIN

// Error messages
static string errorMsg[CMicroYacc::NB_MICROYACC_ERROR] =
{
	"No error",
	"Unknown identifier",
	"Too many local vectors, vertex shaders 1.0 only support 12",
	"Too many local matrix, vertex shaders 1.0 only support 24",
	"Operands differ in type",
	"Parameter out of range",
	"Invalid variable field",
	"Wrong number of operands for function call"
};


// Vertex shaders specific
//	operands
// temporary registers mapping
static unsigned int	targetType;
static bool	targetSwizzled = false;
static map<std::string,void*> objects;
static unsigned int	regsUsed = 0;
static unsigned int	dataUsed = 0;


// Generic time functions + helpers
float RAPTOR_FASTCALL p_FnTimeFunction_Constant(float /*t*/,float p1,float /*p2*/,float /*p3*/,float /*p4*/)
{
	return p1;
}

float RAPTOR_FASTCALL p_FnTimeFunction_Linear(float t,float p1,float p2,float /*p3*/,float /*p4*/)
{
	return p1 * t + p2;
}

float RAPTOR_FASTCALL p_FnTimeFunction_Quadric(float t,float p1,float p2,float p3,float /*p4*/)
{

	return (p1 * t + p2) * t + p3;
}

float RAPTOR_FASTCALL p_FnTimeFunction_Cubic(float t,float p1,float p2,float p3,float p4)
{
	return ((p1 * t + p2) * t + p3) * t + p4;
}

float RAPTOR_FASTCALL p_FnTimeFunction_Cosine(float t,float p1,float /*p2*/,float /*p3*/,float /*p4*/)
{
	return (float)(cos(p1 * t));
}	

float RAPTOR_FASTCALL p_FnTimeFunction_Sine(float t,float p1,float /*p2*/,float /*p3*/,float /*p4*/)
{
	return (float)(sin(p1 * t));
}

RAPTOR_NAMESPACE_END

// Local data
//////////////////////////////////////////////////////////////////////




RAPTOR_NAMESPACE


CObject3D** FindObject(const std::string& identifier)
{
	map<std::string,void*>::const_iterator pos = objects.find(identifier);

	if (pos != objects.end())
		return (CObject3D**)(*pos).second;
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////
// CMicroYacc specific
//////////////////////////////////////////////////////////////////////
void CMicroYacc::InitAnalyse(void)
{
	if (ta != NULL)
		delete ta;
	if (fs != NULL)
		delete fs;
	if (tf != NULL)
		delete tf;

	ta = NULL;
	fs = new FRAMESEQUENCE;
	tf = NULL;
	shader = NULL;

	error = CMicroYacc::MICROYACC_NO_ERROR;

	map<std::string,void*>::const_iterator pos = objects.begin( );
	while (pos != objects.end())
	{
		CObject3D** obj = (CObject3D**)(*pos).second;
		delete obj;
	}
	objects.clear();
}



//////////////////////////////////////////////////////////////////////
// Base class implementation
//////////////////////////////////////////////////////////////////////
string	CMicroYacc::MapTypestring(string toMap) const
{ 
	return (toMap.substr(1,toMap.size()-2));
}


void CMicroYacc::Onload3DS(unsigned int RULE_ID)
{
	switch(RULE_ID)
	{
		case ATOM_string_ID:
		{
			C3DSet *set = NULL;
			string fname = status->m_stload3DS.st_string3;
			
			if (!fname.empty())
			{
				fname += ".3DS";
				CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
				C3DScene *pScene = pDisplay->getRootScene();
				/*
				if (pScene->glLoadScene(fname,NULL))
				{
					CObject3D **obj = FindObject(status->m_stload3DS.st_identifier0);
					if (obj != NULL)
					{
						// Here we should clear the root because
						// object life-cycle is lost !
						*obj = pScene->getRoot();
					}
					else
					{
						error = UNKNOWN_ID;
						OnError(CYacc::YACC_ERR_SYN);
					}
				}
				else
				{
					string error = "Load data " + status->m_stload3DS.st_string3 + " failed";
					Raptor::glGetErrorManager()->glGenerateRaptorError(CPersistence::ID_CLASS_ANIMATOR,
                                                                                                   CRaptorErrorManager::RAPTOR_WARNING,
                                                                                                   error);
				}
				*/
			}
			break;
		}
	}
}



void CMicroYacc::Ondeclaration(unsigned int RULE_ID) 
{
	if (status->m_stdeclaration.m_step == 0)
	{
		/*
		if (status->m_stdeclaration.st_typedef0.st_typedefID == ATOM_object_ID)
		{
			objects[status->m_stdeclaration.st_identifier1] = new CObject3D*;
		}
		else if (status->m_stdeclaration.st_typedef0.st_typedefID == ATOM_matrix_ID)
		{
			MakeLocalVar(status->m_stdeclaration.st_identifier1,ATOM_matrix_ID);
		}
		else if (status->m_stdeclaration.st_typedef0.st_typedefID == ATOM_vector_ID)
		{
			MakeLocalVar(status->m_stdeclaration.st_identifier1,ATOM_vector_ID);
		}
		*/
	}
}

void CMicroYacc::Onprogramparameter_listitem(unsigned int RULE_ID) 
{
	/*
	if (status->m_stprogramparameter_listitem.m_step == 2)
	{
		MakeLocalVar(	status->m_stprogramparameter_listitem.st_identifier1,
						status->m_stprogramparameter_listitem.st_typedef0.st_typedefID,
						true);
	}
	*/
}

void CMicroYacc::Onprogramparameter_lastitem(unsigned int RULE_ID) 
{
	/*
	if (status->m_stprogramparameter_lastitem.m_step == 2)
	{
		MakeLocalVar(	status->m_stprogramparameter_lastitem.st_identifier1,
						status->m_stprogramparameter_lastitem.st_typedef0.st_typedefID,
						true);
	}
	*/
}

void CMicroYacc::Onrotateobject(unsigned int RULE_ID)
{
    if (status->m_strotateobject.m_step == 0)
	{
        CObject3D *o = m_currentObject;
        if (o != NULL)
        {
            o->rotation(status->m_strotateobject.st_float2,
						status->m_strotateobject.st_float4,
						status->m_strotateobject.st_float6,
						status->m_strotateobject.st_float8);
            msg = "Object [" + o->getName() + "] rotated.";
        }
        else
            msg = "No Object Selected.";
    }
}
void CMicroYacc::Onvisibleobject(unsigned int RULE_ID)
{ 
    if (status->m_stvisibleobject.m_step == 0)
    {
        CObject3D *o = m_currentObject;
        if (o != NULL)
        {
            bool visible = (status->m_stvisibleobject.st_boolean2.st_booleanID == ATOM_true_ID);
            o->getProperties().setVisible(visible);
            msg = "Object [" + o->getName() + "] visibility changed.";
        }
        else
            msg = "No Object Selected.";
    }
}


void CMicroYacc::Ontranslateobject(unsigned int RULE_ID)
{
    if (status->m_sttranslateobject.m_step == 0)
	{
        CObject3D *o = m_currentObject;
        if (o != NULL)
        {
            o->translate(	status->m_sttranslateobject.st_float2,
							status->m_sttranslateobject.st_float4,
							status->m_sttranslateobject.st_float6);
            msg = "Object [" + o->getName() + "] translated.";
        }
        else
            msg = "No Object Selected.";
    }
}

void CMicroYacc::Ontranslateobjectabsolute(unsigned int RULE_ID)
{
    if (status->m_sttranslateobjectabsolute.m_step == 0)
	{
        CObject3D *o = m_currentObject;
        if (o != NULL)
        {
            o->translateAbsolute(	status->m_sttranslateobjectabsolute.st_float2,
									status->m_sttranslateobjectabsolute.st_float4,
									status->m_sttranslateobjectabsolute.st_float6);
            msg = "Object [" + o->getName() + "] translated to absolute position.";
        }
        else
            msg = "No Object Selected.";
    }
}

void CMicroYacc::Onscaleobject(unsigned int RULE_ID)
{
    if (status->m_stscaleobject.m_step == 0)
	{
        CObject3D *o = m_currentObject;
        if (o != NULL)
        {
            o->scale(	status->m_stscaleobject.st_float2,
						status->m_stscaleobject.st_float4,
						status->m_stscaleobject.st_float6);
            msg = "Object [" + o->getName() + "] scaled.";
        }
        else
            msg = "No Object Selected.";
    }
}

void CMicroYacc::Onselection(unsigned int RULE_ID)
{
	if (status->m_stselection.m_step == 0)
	{
		string objectName = status->m_stselection.st_string1;
		CPersistence *p = CPersistence::FindObject(objectName.data());
		if (p != NULL)
		{
			if (p->getId().isSubClassOf(CObject3D::CObject3DClassID::GetClassId()))
            {
                m_currentObject = (CObject3D*)p;
                msg = "Object [" + objectName + "] selected.";
            }
            else
                m_currentObject = NULL;
		}
		else
        {
            m_currentObject = NULL;
            msg = "Object [" + objectName + "] not found.";
        }
	}
}

void CMicroYacc::Onhelp(unsigned int RULE_ID)
{
    if (status->m_sthelp.m_step == 0)
	{
        displayHelp = true;
    }
}

void CMicroYacc::Onclear(unsigned int RULE_ID)
{
    if (status->m_stclear.m_step == 0)
	{
        clear = true;
    }
}

//////////////////////////////////////////////////////////////////////
// Vertex shaders specific
//////////////////////////////////////////////////////////////////////
void CMicroYacc::OnError(CYacc::YaccError err)
{
	msg.erase();

	switch(err)
	{
		case CYacc::YACC_ERR_LEX:
			msg = "Error: failed to parse script";
			break;
		case CYacc::YACC_ERR_SYN:
			msg = "Error: syntax error in source script";
			break;
		case CYacc::YACC_NOERR:
			msg = "Ok : source script contains no error";
			break;
	}

	if (error != MICROYACC_NO_ERROR)
		msg += errorMsg[error - MICROYACC_NO_ERROR];

	ostrstream errPos;
	errPos << " [line: " << lex->GetNumLine();
	errPos << " pos: " << lex->GetLinePos();
	errPos << "]" << ends;

	msg += errPos.str();

	if (displayError)
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),msg.data());
}


void CLexical::OnError(CLex::LexError err)
{
	msg.erase();

	switch(err)
	{
	case CLex::LEX_ERR_SRC:
		msg = "Error: failed to source script";
		break;
	case CLex::LEX_ERR_SYN:
	{
		ostrstream str;
		str << "Error: syntax error in source script\nline: ";
		str << GetNumLine() << " col: ";
		str << GetLinePos() << ends;
		msg = str.str();
		break;
	}
	case CLex::LEX_NOERR:
		msg = "Ok : source script contains no error";
		break;
	}

	if (displayError)
		RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(),msg.data());
}

