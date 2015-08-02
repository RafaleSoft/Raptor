// ObjectView.cpp : implementation file
//

#include "stdafx.h"
#include "RaysClient.h"
#include "MainFrm.h"
#include "RaysClientDoc.h"
#include "ObjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectView

IMPLEMENT_DYNCREATE(CObjectView, CTreeView)

CObjectView::CObjectView():ObjectsData(NULL)
{
	//{{AFX_DATA_INIT(CObjectView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CObjectView::~CObjectView()
{
}


void CObjectView::DoDataExchange(CDataExchange* pDX)
{
	CTreeView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectView, CTreeView)
	//{{AFX_MSG_MAP(CObjectView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectView diagnostics

#ifdef _DEBUG
void CObjectView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CObjectView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectView message handlers

void CObjectView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	ObjectsData = ((CRaysClientApp*)AfxGetApp())->ObjectsData;
	this->GetTreeCtrl().ModifyStyle(0,TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_EDITLABELS,0);
	this->GetTreeCtrl().SetImageList(ObjectsData,TVSIL_NORMAL);
	root = this->GetTreeCtrl().InsertItem("Scene objects",0,0);
}


void CObjectView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonDblClk(nFlags, point);
}

void CObjectView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRaysClientApp *pApp = (CRaysClientApp*)AfxGetApp();
	CMainFrame *main = (CMainFrame *)pApp->m_pMainWnd;
	CImageList *l = main->dragImage;
	if (l!=NULL)
	{
		CPoint point;
		GetCursorPos(&point);
		l->DragMove(point);
	}	
	
	CTreeView::OnMouseMove(nFlags, point);
}


void CObjectView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRaysClientApp *pApp = (CRaysClientApp*)AfxGetApp();
	CMainFrame *main = (CMainFrame *)pApp->m_pMainWnd;
	CImageList *l = main->dragImage;
	if (l!=NULL)
	{
		l->DragLeave(NULL);
		l->EndDrag();
		delete main->dragImage;
		main->dragImage = NULL;

		// update tree
		if (main->ObjID>IDI_OBJDATA)
			UpdateTree(main->ObjID);
	}

	CTreeView::OnLButtonUp(nFlags, point);
}

BOOL CObjectView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CTreeView::OnCommand(wParam, lParam);
}

void CObjectView::UpdateTree(DWORD ID)
{
	CRaysClientDoc *pDoc = GetDocument();

	switch(ID)
	{
		case IDI_CAMAXES:
			{
				HTREEITEM cam = this->GetTreeCtrl().InsertItem("AxisCamera " + pDoc->GenName(IDI_CAMAXES),6,6,root);
				HTREEITEM vertex = this->GetTreeCtrl().InsertItem("Origin",0,0,cam);
				this->GetTreeCtrl().InsertItem("X 0",5,5,vertex);
				this->GetTreeCtrl().InsertItem("Y 0",5,5,vertex);
				this->GetTreeCtrl().InsertItem("Z 0",5,5,vertex);
				this->GetTreeCtrl().InsertItem("Focale 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("Aperture 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("ObjectPlane 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("Width 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("Height 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("Depth 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("Variance 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("ReflectionDepth 0",5,5,cam);
				this->GetTreeCtrl().InsertItem("RefractionDepth 0",5,5,cam);
			}
		break;
	}
	pDoc->UpdateAllViews(NULL,ID);
}

void CObjectView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	int pos;

	int img,simg;
	this->GetTreeCtrl().GetItemImage(pTVDispInfo->item.hItem,img,simg) ;

	// cancel editing for non data items
	if (img == 5)
	{
		buffer = pTVDispInfo->item.pszText;
		pos = buffer.ReverseFind(' ');

		// seek to owner object in this view
		CRaysClientDoc *pDoc = GetDocument();
		HTREEITEM owner = this->GetTreeCtrl().GetParentItem(pTVDispInfo->item.hItem);
		this->GetTreeCtrl().GetItemImage(owner,img,simg) ;

		while ((img==5)||(img==0))
		{
			owner = this->GetTreeCtrl().GetParentItem(owner);
			this->GetTreeCtrl().GetItemImage(owner,img,simg) ;
		}

		// seek to text in text view
		line = -1;
		CString name = this->GetTreeCtrl().GetItemText(owner);
		if ((line=pDoc->FindLine(name))>0)
			line=pDoc->FindLine(buffer,line);


		// store variable to avoid modifications
		buffer = buffer.Left(pos);
	
		*pResult = 0;
	}
	else
		*pResult = TRUE;
}

void CObjectView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	CString val;

	val = pTVDispInfo->item.pszText;
	if (val.ReverseFind(' ')<0)
	{
		buffer = buffer+" "+val;
		this->GetTreeCtrl().SetItemText(pTVDispInfo->item.hItem,buffer);
	
		*pResult = 0;
	}
	else
	{
		buffer = this->GetTreeCtrl().GetItemText(pTVDispInfo->item.hItem);
		*pResult = TRUE;
	}

	// updates edit text
	if (line>0)
	{
		CRaysClientDoc *pDoc = GetDocument();
		pDoc->DeleteLine(line);
		pDoc->NewLine(line);
		pDoc->SetLine(line,buffer);
		pDoc->UpdateAllViews(this);
	}
}
