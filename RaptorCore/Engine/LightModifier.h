// LightModifier.h: interface for the CLightModifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTMODIFIER_H__3A911E5A_0ABE_4E28_B446_ADC42463246D__INCLUDED_)
#define AFX_LIGHTMODIFIER_H__3A911E5A_0ABE_4E28_B446_ADC42463246D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MODIFIER_H__BEAB48CC_47E5_4807_B18E_D2005795363F__INCLUDED_)
	#include "Modifier.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLight;
class CObject3DInstance;


class RAPTOR_API CLightModifier : public CModifier  
{
public:
    typedef enum
    {
        SET_POSITION,
        SET_DIRECTION
    } LIGHT_MODIFIER_ACTIONS;

public:
	CLightModifier(const std::string& name = "GL_LIGHTMODIFIER");
	virtual ~CLightModifier();

    //! CModifier interface
    virtual CModifier::MODIFIER_TYPE getType(void) const { return CModifier::CGL_LIGHT_MODIFIER; };

    //! Sets the light upon which this modifier applies transforms
	bool setLight(CLight *light);

    //	Remove all previously actions added to modifier
	void clearActions(void);

	void addAction(	CLightModifier::LIGHT_MODIFIER_ACTIONS action,
					CModifier::TIME_FUNCTION tx,
					CModifier::TIME_FUNCTION ty,
					CModifier::TIME_FUNCTION tz);

	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CLightModifierClassID,"LightModifier",CModifier)



protected:
	//	Compute physics resultant after dt time increment
	//	This method should not be called directly.
	virtual void RAPTOR_FASTCALL deltaTime(float dt);

	//!	Implements CPersistence
	void unLink(const CPersistence* obj);



private:
    void importAction(CRaptorIO& io);

    CLight* m_pLight;

    typedef struct
	{
		LIGHT_MODIFIER_ACTIONS	action;
		CModifier::TIME_FUNCTION	tx;
		CModifier::TIME_FUNCTION	ty;
		CModifier::TIME_FUNCTION	tz;
	} LIGHT_TIME_ACTIONS;

	vector<LIGHT_TIME_ACTIONS>	m_lightActions;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTMODIFIER_H__3A911E5A_0ABE_4E28_B446_ADC42463246D__INCLUDED_)

