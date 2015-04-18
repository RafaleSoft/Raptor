// TestDoc.h : interface of the CTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
#define AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_

#pragma once

#include "Raptordll.h"
#include "System/RenderEntryPoint.h"

RAPTOR_NAMESPACE

class CSlide;


class CTestDoc  : public  CRenderEntryPoint
{
public:
	static const float SPEED_TRESHOLD;
	static const float SPEED_FADE;

    CTestDoc(HWND , CRaptorDisplay*);
    virtual ~CTestDoc();

	virtual	void GLInitContext(HDC hdc);

    virtual void glRender(void);

    virtual void glDisplay(HDC hdc);

    void translateMenu(float dx);

    void highLight(bool HL);

protected:
    CRaptorDisplay *m_pDisplay;
    HWND m_wnd;    

    vector<CSlide*>   m_slides;

	CRaptorDisplayFilter *m_hdr;
	CRaptorDisplayFilter *m_mb;

	float		m_currentSpeed;
	float		m_currentFade;
};

#endif // !defined(AFX_TESTDOC_H__864FB149_93D7_4EE3_BCFE_7CE132610911__INCLUDED_)
