// TreeDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "GLObjectViewer.h"
#include "resource.h"
#include "TreeDialogBar.h"
#include "MainFrm.h"

#include "GLHierarchy/Persistence.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ShadedGeometry.h"

RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CTreeDialogBar dialog


CTreeDialogBar::CTreeDialogBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CTreeDialogBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTreeDialogBar::setScene(C3DSet *pScene)
{
	CTreeCtrl *tree = (CTreeCtrl*)GetDlgItem(IDC_OBJECTTREE);
	HTREEITEM scene_root = tree->InsertItem( CA2T(pScene->getName().c_str()));
	tree->SetItemData(scene_root,(DWORD_PTR)pScene);
	addSet(pScene,scene_root);
}

void CTreeDialogBar::addSet(C3DSet *pSet,HTREEITEM location)
{
	CTreeCtrl *tree = (CTreeCtrl*)GetDlgItem(IDC_OBJECTTREE);
	CObject3D *pObject = NULL;
	C3DSet::C3DSetIterator  scenePos = pSet->getIterator();

	while (NULL != (pObject = pSet->getChild(scenePos++)))
	{
		if (pObject->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
		{
			CShadedGeometry *sg = (CShadedGeometry*)pObject;
			string title = pObject->getName() + " [ShadedGeometry]";

			HTREEITEM item = tree->InsertItem(CA2T(title.c_str()),location);
			tree->SetItemData(item,(DWORD_PTR)pObject);
		}
		else if (pObject->getId().isSubClassOf(C3DSet::C3DSetClassID::GetClassId()))
		{
			C3DSet *set = (C3DSet*)pObject;
			string title = pObject->getName() + " [ShadedGeometry]";

			HTREEITEM item = tree->InsertItem(CA2T(title.c_str()),location);
			tree->SetItemData(item,(DWORD_PTR)pObject);

			addSet(set,item);
		}
	}
}


BEGIN_MESSAGE_MAP(CTreeDialogBar, CDialogBar)
	//{{AFX_MSG_MAP(CTreeDialogBar)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OBJECTTREE, OnSelchangedObjecttree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeDialogBar message handlers
void CTreeDialogBar::OnSelchangedObjecttree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TVITEM tvItem = pNMTreeView->itemNew;

	HTREEITEM item = tvItem.hItem;
	CTreeCtrl *tree = (CTreeCtrl*)GetDlgItem(IDC_OBJECTTREE);	
	CObject3D *obj = (CObject3D*)(tree->GetItemData(item));

	CMainFrame *parent = (CMainFrame*)(AfxGetMainWnd());

	parent->updateSelectedObject(obj);
	
	*pResult = 0;
}
