/***************************************************************************/
/*                                                                         */
/*  DOFFilter.h                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_DOFFILTER_H__A4FA0FE9_04AA_4887_9B4A_3CFAF930D840__INCLUDED_)
#define AFX_DOFFILTER_H__A4FA0FE9_04AA_4887_9B4A_3CFAF930D840__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif

RAPTOR_NAMESPACE_BEGIN
class CRaptorDisplay;
RAPTOR_NAMESPACE_END

RAPTOR_NAMESPACE


class RAPTOR_API CDOFFilter : public CRaptorDisplayFilter  
{
public:
	CDOFFilter();

    //! Defines the Depth Of Field effect parameters:
    //! - percentageOfDepthFiltered : represents the percentage of the view volume depth that is filtered.
    //! A value of 0.95 means that 95% of depth clear, and the 5% remaining is in the circle of confusion ( DOF ).
    //! - filterAmplitude : defines the speed at which the bluriness grows as depth increase.
    //! This value is multiplied by percentageOfDepthFiltered, which is 2.5 times faster than the value of 20,
    //! where bluriness is 100% at maximum depth.
    void setDOFParams(float percentageOfDepthFiltered=0.95f, float filterAmplitude=50.0f);

    //! Implements base class
    virtual bool glInitFilter(void);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

	//! Configure numbre of bluring passes for the high frequencies
	//! (4 maximum passes)
	void setBlurNbPass(unsigned int nb=1);


private:
	//! Fordibben destructor
	virtual ~CDOFFilter();

	//! Implements base class
    virtual void glRenderFilter();

	//! Implements base class
	virtual void glRenderFilterOutput(void);

	//!	Initializes the shaders
	void glInitShaders(void);

	unsigned int		m_nbBlur;
	CProgramParameters::CParameter<GL_COORD_VERTEX> dofParams;

    CRaptorDisplay		*tmpDisplay;
    ITextureObject      *tmpTexture;
	CRaptorDisplay		*tmpDisplay2;
    ITextureObject      *tmpTexture2;
	CTextureSet			*m_pRenderTextures2;
    CShader             *DOFShader;

	CProgramParameters	paramsX;
	CProgramParameters	paramsY;
};

#endif // !defined(AFX_DOFFILTER_H__A4FA0FE9_04AA_4887_9B4A_3CFAF930D840__INCLUDED_)
