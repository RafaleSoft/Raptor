#if !defined(AFX_SHADERDIALOG_H__CC503995_E3B9_4BC2_9835_19F547740025__INCLUDED_)
#define AFX_SHADERDIALOG_H__CC503995_E3B9_4BC2_9835_19F547740025__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShaderDialog.h : header file
//

#include "GLHierarchy/Shader.h"

RAPTOR_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CShaderDialog dialog

class CShaderDialog : public CDialog
{
// Construction
public:
	CShaderDialog();

	void refreshShaders(void);

// Dialog Data
	//{{AFX_DATA(CShaderDialog)
	enum { IDD = IDD_DIALOGSHADER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShaderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShaderDialog)
	afx_msg void OnEditchangeShaderlist();
	afx_msg void OnSelchangeShaderlist();
	afx_msg void OnModifyShader();
	afx_msg void OnColorambient();
	afx_msg void OnColordiffuse();
	afx_msg void OnColorspecular();
	afx_msg void OnDiffusemap();
	afx_msg void OnLightmap();
	afx_msg void OnEnvironmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void setValue(int nID,float value);
	void getValue(int nID,float &value);

	CShader *m_pShader;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADERDIALOG_H__CC503995_E3B9_4BC2_9835_19F547740025__INCLUDED_)
