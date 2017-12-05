#if !defined(AFX_OBJECTDIALOG_H__5561BA29_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
#define AFX_OBJECTDIALOG_H__5561BA29_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_GLWND_H__9DD83791_854B_11D2_9142_830BB0D20AF2__INCLUDED_)
	#include "CWnd/GLWnd.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CPersistence;
class CGLView;



/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog

class CObjectDialog : public CDialog
{
// Construction
public:
	CObjectDialog(CWnd* pParent = NULL);   // standard constructor

	CGLView *GLContext;
// Dialog Data
	//{{AFX_DATA(CObjectDialog)
	enum { IDD = IDD_DIALOG2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGLWnd	Context;
	// Generated message map functions
	//{{AFX_MSG(CObjectDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

RAPTOR_NAMESPACE_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTDIALOG_H__5561BA29_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)

