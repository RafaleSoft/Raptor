// RaysClientDoc.h : interface of the CRaysClientDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAYSCLIENTDOC_H__36EB3A38_268D_4407_8C12_0AA739D51AA3__INCLUDED_)
#define AFX_RAYSCLIENTDOC_H__36EB3A38_268D_4407_8C12_0AA739D51AA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Surface.h"
//#include "Light.h"
//#include "Texture.h"
#include "../Camera.h"

#define MAX_OBJECTS	256

class CRaysClientDoc : public CDocument
{
protected: // create from serialization only
	CRaysClientDoc();
	DECLARE_DYNCREATE(CRaysClientDoc)

	int counter;
	int nbObjects;

// Attributes
public:
	CStringArray	Text;
	bool			Compiled;

	CRenderObject	*Objects[MAX_OBJECTS];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysClientDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRaysClientDoc();

	//	THE function
	bool Compile();

	//	Inlines : Doc status
	int NumLines() const { return Text.GetSize(); };
	bool IsCompiled() const { return Compiled; };
	void SetCompiled(bool value) { Compiled=value; };

	//	 Rendering datas
	CString GenName(int ID);
	void AddElement(int ID);

	//	Text functions
	const	CString GetLine(int numLine) const;
	void	NewLine(int numLine);
	void	DeleteLine(int numLine);
	const	CString MaxLine() const;
	const	CSize GetSize(CDC *pDC) const;
	void	Insert(int numLine,int numChar,char nChar);
	void	Extract(int numLine,int numChar);
	void	Expand(int numLine,CString Tail);
	void	SetLine(int numLine,CString newLine);
	void	AppendLine(CString newLine);	
	int		FindLine(CString subString,int fromLine=0);	

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void PurgeObjects();
	CCamera *ParseCamera(int &line);
// Generated message map functions
protected:
	//{{AFX_MSG(CRaysClientDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSCLIENTDOC_H__36EB3A38_268D_4407_8C12_0AA739D51AA3__INCLUDED_)
