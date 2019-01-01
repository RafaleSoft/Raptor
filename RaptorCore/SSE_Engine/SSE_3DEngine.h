// SSE_3DEngine.h: interface for the CSSE_3DEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)
#define AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
    #include "Engine/3DEngine.h"
#endif


RAPTOR_NAMESPACE_BEGIN

#ifdef RAPTOR_SSE_CODE_GENERATION

    class RAPTOR_API CSSE_3DEngine : public C3DEngine  
    {
    public:
	    CSSE_3DEngine(const std::string& name = "SSE_3D_ENGINE");
	    virtual ~CSSE_3DEngine();

	    //!	Return true if the given bbox is visible on the viewport according OpenGL status.
        //! The second parameter controls the use of OpenGL modelview for the current call :
        //! if false, the current engine's modelview is used. If true, OGL modelview is rescanned.
	    virtual CLIP_RESULT glClip(const CBoundingBox *const bbox,bool scanModelview = true,const CGenericMatrix<float> *transform = NULL) const;
    };

#endif

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)

