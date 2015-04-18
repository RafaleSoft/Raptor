// MBFilter.h: interface for the CMBFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MBFILTER_H__53A619DD_DBAB_4709_9EAD_72C5D6C401E9__INCLUDED_)
#define AFX_MBFILTER_H__53A619DD_DBAB_4709_9EAD_72C5D6C401E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif

RAPTOR_NAMESPACE

class RAPTOR_API CMBFilter : public CRaptorDisplayFilter  
{
public:
	CMBFilter();

    //! Defines a the percentage of motion blur per component
    //! The default is 0.75. Rq : the affect appearence also depend on the framerate.
    void setPercentage(float r,float g,float b,float a);

	//! Returns the percentage of motion blur per component
    void getPercentage(float &r,float &g,float &b,float &a);

    //! Implements base class
    virtual bool glInitFilter(void);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

private:
	//! Fordidden destructor
	virtual ~CMBFilter();

	//! Implements base class
    virtual void glRenderFilter();

	//! Implements base class
	virtual void glRenderFilterOutput(void);

    CShader             *m_pMotionBlurShader;
    CTextureGenerator	*m_pAccumulator;
	CTextureSet			*m_pRenderTextures2;

    CColor::RGBA		mbParams;
};

#endif // !defined(AFX_MBFILTER_H__53A619DD_DBAB_4709_9EAD_72C5D6C401E9__INCLUDED_)
