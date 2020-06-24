/***************************************************************************/
/*                                                                         */
/*  TestDoc.h                                                              */
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


#if !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
#define AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_

#include "System/RenderEntryPoint.h"
#include "Raptordll.h"

RAPTOR_NAMESPACE

class CHDRFilter;
class CMagnifierFilter;
class CBlurFilter;
class CMBFilter;
class CDOFFilter;
class CColorControlFilter;

class CTestDoc : public  CRenderEntryPoint
{
public:
    CTestDoc(const RAPTOR_HANDLE&,CRaptorDisplay*);
    virtual ~CTestDoc();

	void GLInitContext(void);

    virtual void glRender();


protected:
	CShadedGeometry	*m_pSG;
	CFragmentProgram	*m_pFS;
	CVertexProgram	*m_pVS;
	CVertexProgram	*m_pVS2;

    CRaptorDisplay	*m_pDisplay;
    RAPTOR_HANDLE	m_wnd;

    CHDRFilter			*hdr;
	CMagnifierFilter	*pMag;
	CBlurFilter			*pBlur;
	CMBFilter			*mbf;
	CDOFFilter			*dof;
	CColorControlFilter *bwf;
};

#endif // !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
