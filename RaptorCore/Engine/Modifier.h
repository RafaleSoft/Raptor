// Modifier.h: interface for the CModifier class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
#define AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CModifier : public CPersistence, public CTimeObject
{
public:
    //! Available kind of modifiers
    typedef enum
    {
        CGL_VIEW_MODIFIER,
        CGL_GEOMETRIC_MODIFIER,
        CGL_IMAGE_MODIFIER,
        CGL_LIGHT_MODIFIER
    } MODIFIER_TYPE;

    //! Default time functions.
    //! Functions use a0 to a3 parameters and compute a result
    //! using a global time parameter passed to deltaTime by Raptor
	typedef enum
	{
		CGL_TIME_CONSTANT,	//	f(t) = a0
		CGL_TIME_LINEAR,	//	f(t) = a1.t + a0
		CGL_TIME_QUADRIC,	//	f(t) = a2.t^2 + a1.t + a0
		CGL_TIME_CUBIC,		//	f(t) = a3.t^3 + a2.t^2 + a1.t + a0
		CGL_TIME_COSINE,	//	f(t) = a3 + a2.cos(a1.t + a0)
		CGL_TIME_SINE,		//	f(t) = a3 + a2.sin(a1.t + a0)
		CGL_TIME_TRIANGLE,	//	f(t) = a1.(MIN(1,t/a2) + MIN(0,(t-a2)/(a2-a3))) + a0
		CGL_TIME_SQUARE,	//
		CGL_TIME_USER
	} TIME_FUNCTION_TYPE;

    //! Type specifier for user functions.
    typedef float (*USER_FUNCTION)(float dt);

    //! Global description of a time function :
    //! - the type of mathematical function
    //! - a period when it is relevent
    //! - up to 4 parameters
    //! - a user compute function for more complex situations.
	typedef struct TIME_FUNCTION_t
	{
		TIME_FUNCTION_TYPE	timeFunction;
		float				period;
		float				a0;
		float				a1;
		float				a2;
		float				a3;
        USER_FUNCTION		pUserFunction;

		TIME_FUNCTION_t(TIME_FUNCTION_TYPE type=CGL_TIME_CONSTANT,float c0=0.0f,
						float c1=0.0f,float c2=0.0f,float c3=0.0f,float p=0.0f,USER_FUNCTION pFct=NULL):
		timeFunction(type),period(p),a0(c0),a1(c1),a2(c2),a3(c3),pUserFunction(pFct)
		{ };
	} TIME_FUNCTION;

public:
	virtual ~CModifier();

	//!	@return the modifier type from subclasses.
	//	Might disappear in future versions.
	virtual MODIFIER_TYPE getType(void) const = 0;

	//!	@return the computed time function
	float getTime() const { return m_ft; };

	//!	Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CModifierClassID,"Modifier",CPersistence)


protected:
	TIME_FUNCTION		m_timeFunction;

	//	The result of the time function
	float m_ft;

	//	Compute physics resultant after dt time increment
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

    //  Reserved constructor for derived classes
	CModifier(	TIME_FUNCTION_TYPE	timeFunction,
				float				timeArg0,
				float				timeArg1,
				float				timeArg2,
				float				timeArg3,
                USER_FUNCTION		pUserFunction,
				const CPersistence::CPersistenceClassID& id,
				const std::string& name = "GL_MODIFIER");

    // Implements I/O for Time functions.
    void importTimeFunction(CModifier::TIME_FUNCTION& tf, CRaptorIO& io);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)

