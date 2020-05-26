/***************************************************************************/
/*                                                                         */
/*  DemoDoc.h                                                              */
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


#if !defined(AFX_DEMODOC_H__B28A066B_5EFD_412F_83DC_DD8E230F844D__INCLUDED_)
#define AFX_DEMODOC_H__B28A066B_5EFD_412F_83DC_DD8E230F844D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raptordll.h"
#include "System/RenderEntryPoint.h"

RAPTOR_NAMESPACE

class CObjectStore;
class CDOFFilter;
class CHDRFilter;

class CDemoDoc : public  CRenderEntryPoint
{
public:
	CDemoDoc(const RAPTOR_HANDLE& wnd,CRaptorDisplay *pDisplay);
	virtual ~CDemoDoc();

	void glRender(void);

	virtual	void GLInitContext(void);

	unsigned int	nbFrames;
	unsigned int	startTime;
	unsigned int	finishTime;
	unsigned int	lastFrame;
	float			minfps;
	float			maxfps;

private:
	CRaptorDisplay	*m_pDisplay;
    RAPTOR_HANDLE	m_wnd;

	CObjectStore	*m_objects;

	
	CRaptorDisplay	*m_pBuffer;
	IViewPoint		*m_pViewPoint;
	ITextureObject	*m_pTexture;

	C3DPath			*m_viewPathRoom;
	C3DPath			*m_lookPathRoom;

	C3DPath			*m_viewPathStairs;
	C3DPath			*m_lookPathStairs;
	CShader			*m_projector;

    CDOFFilter      *dof;
	CHDRFilter		*hdr;

	void DrawStair(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODOC_H__B28A066B_5EFD_412F_83DC_DD8E230F844D__INCLUDED_)
