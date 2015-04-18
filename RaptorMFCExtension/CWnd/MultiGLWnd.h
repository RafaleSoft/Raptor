#if !defined(AFX_MULTIGLWND_H__8025342E_91E0_11D3_9800_8282AC000000__INCLUDED_)
#define AFX_MULTIGLWND_H__8025342E_91E0_11D3_9800_8282AC000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiGLWnd.h : header file
//

#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "GLWnd.h"
#endif

RAPTOR_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CMultiGLWnd window

class RAPTOR_API CMultiGLWnd : public CWnd
{
// Construction
public:
	CMultiGLWnd();
	virtual ~CMultiGLWnd();

	bool GLCreateFrame(CString name,int x,int y,int width,int height);

	bool AddDisplay(CGLWnd *d);

	virtual void glRender();
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CMultiGLWnd)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	virtual void unLink(const CPersistence*);

	// Attributes
private:
	typedef struct display_t
	{
		CGLWnd	*display;
		float	percentx;
		float	percenty;
		float	percentw;
		float	percenth;
	};

	CString		WndClass;

	int			nb_displays;
	display_t	*Displays;

	int			m_width;
	int			m_height;
};

RAPTOR_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIGLWND_H__8025342E_91E0_11D3_9800_8282AC000000__INCLUDED_)
