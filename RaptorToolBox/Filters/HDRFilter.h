// HDRFilter.h: interface for the CHDRFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_)
#define AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "System/RaptorDisplayConfig.h"
#endif
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif


RAPTOR_NAMESPACE_BEGIN
class CRaptorDisplay;
class CVertexShader;
class CFragmentShader;
RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

class RAPTOR_API CHDRFilter : public CRaptorDisplayFilter  
{
public:
    //! Constructor
    //! A display config is necessary to initialise sub displays compliant with source display
	CHDRFilter(const CRaptorDisplayConfig &rda);

    //! Implements base class filter initialiser
    virtual bool glInitFilter(void);

    //! Implements base class filter releaser
    virtual void glDestroyFilter(void);

	//! Configure numbre of bluring passes for the high frequencies
	//! (4 maximum passes)
	void setBlurNbPass(unsigned int nb=1);

	//! Configure high frequencies thresholds
	void setHFTreshold(float t);


private:
	//! Forbidden destructor
	virtual ~CHDRFilter();

	//! Implements base class
    virtual void glRenderFilter(void);

	//! Implements base class
	virtual void glRenderFilterOutput(void);

	//!	Build all intermediate and final shaders
	bool glBuildShaders(void);

	//! HDR rendering attributes
    unsigned int    nLevels;
	unsigned int	nBlurPass;

	//!	Downsizing of framebuffer to compute overall luminance
	CProgramParameters::CParameter<GL_COORD_VERTEX> luminanceParams;
	CProgramParameters maxLuminanceParams;
    CRaptorDisplay  **m_pDownSizedDisplay;
    CTextureObject  **m_pDownSizedBuffer;
	CTextureSet		**m_pDownSizedAttachments;
	CShader			*m_maxLuminance;
    CShader			*m_lastMaxLuminance;
    
	//! Extract high frequencies
	CProgramParameters::CParameter<GL_COORD_VERTEX> thresholdParam;
	CProgramParameters thresholdParams;
    CShader *m_pTreshholdFreqs;
    CRaptorDisplay  *m_pDownHighFreqs;
    CTextureObject  *m_pDownHFBuffer;

	//! Downsizing blur of high frequencies threshold
	CProgramParameters blurOffsets;
	CRaptorDisplay  *m_pDownBlurXDisplay;
    CRaptorDisplay  *m_pDownBlurYDisplay;
    CTextureObject  *m_pDownBlurXBuffer;
    CTextureObject  *m_pDownBlurYBuffer;
    CVertexShader   *m_pBlurXOffsets;
    CVertexShader   *m_pBlurYOffsets;
    CFragmentShader *m_pBlur;

	//! Final image composition with tone mapping
    CShader *m_pComposite;
    
	//! Initial display configuration used to build internal subdisplays
    CRaptorDisplayConfig rda;
};

#endif // !defined(AFX_HDRFILTER_H__6E9B4FC8_154A_46DD_A218_6BC438E45C6A__INCLUDED_)
