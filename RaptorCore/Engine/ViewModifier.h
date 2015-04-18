// ViewModifier.h: interface for the CViewModifier class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_VIEWMODIFIER_H__115939E5_BC5E_40A5_B2A8_7CEBD6C4009A__INCLUDED_)
#define AFX_VIEWMODIFIER_H__115939E5_BC5E_40A5_B2A8_7CEBD6C4009A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "CGLTypes.h"
#endif

#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3DInstance;



class RAPTOR_API CViewModifier : public CModifier  
{
public:
	typedef enum
	{
		RESET_TRANSFORM,
		TRANSLATEABSOLUTE,
		TRANSLATE_VIEW,
		ROTATE_VIEW,
	} VIEW_MODIFIER_ACTIONS;

public:
	CViewModifier(const std::string& name = "GL_VIEWMODIFIER");
	virtual ~CViewModifier();

    //! Implements base class
    virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_VIEW_MODIFIER; };

	//!	An instance is associated ( if necessary ) to the object
	//!	If object is NULL, OpenGL MODELVIEW actions will be issued
	bool setObject(CObject3D *object);

	//!	Returns the object managed. The object can be:
	//!	- the object given to setObject if it was an instance
	//!	- a CObject3DInstance created to handle a "geometric" object
	//!	( any CObject3D indeed, even another instance )
	CObject3DInstance*	getObject(void) const;

	//!	Remove all previously actions added to modifier
	void clearActions(void);

    //! Add a new action to the end of the action list. 
    //! The action is created using the kind and 3 time functions
	void addAction(	CViewModifier::VIEW_MODIFIER_ACTIONS action,
					CModifier::TIME_FUNCTION tx,
					CModifier::TIME_FUNCTION ty,
					CModifier::TIME_FUNCTION tz);

	//! Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CViewModifierClassID,"ViewModifier",CModifier)


protected:
	//!	Compute physics resultant after dt time increment
	//!	This method should not be called directly.
	virtual void RAPTOR_FASTCALL deltaTime(float dt);


private:
	void importAction(CRaptorIO& io);

	bool				m_isInstance;
	CObject3DInstance	*m_object;

	typedef struct
	{
		VIEW_MODIFIER_ACTIONS	action;
		CModifier::TIME_FUNCTION	tx;
		CModifier::TIME_FUNCTION	ty;
		CModifier::TIME_FUNCTION	tz;
	} VIEW_TIME_ACTIONS;

	vector<VIEW_TIME_ACTIONS>	m_viewActions;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VIEWMODIFIER_H__115939E5_BC5E_40A5_B2A8_7CEBD6C4009A__INCLUDED_)

