// GLObjectViewerDoc.cpp : implementation of the CGLObjectViewerDoc class
//

#include "stdafx.h"
#include "GLObjectViewerDoc.h"
#include "ToolBox/RaptorToolBox.h"
#include "MFCExtension/CWnd/GLView.h"
#include "System/RaptorDisplay.h"
#include "Engine/3DScene.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "Engine/IViewPoint.h"

#include "Resource.h"
#include "MainFrm.h"
/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerDoc

IMPLEMENT_DYNCREATE(CGLObjectViewerDoc, CGLDocument)

BEGIN_MESSAGE_MAP(CGLObjectViewerDoc, CGLDocument)
	//{{AFX_MSG_MAP(CGLObjectViewerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerDoc construction/destruction

CGLObjectViewerDoc::CGLObjectViewerDoc()
{
	m_pRoot = NULL;
    updateView = false;
	m_pRootInstance = NULL;
	m_pDefaultMaterial = NULL;
}

CGLObjectViewerDoc::~CGLObjectViewerDoc()
{
}

BOOL CGLObjectViewerDoc::OnNewDocument()
{
	if (!CGLDocument::OnNewDocument())
		return FALSE;

	
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerDoc serialization

void CGLObjectViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGLObjectViewerDoc commands

BOOL CGLObjectViewerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CGLDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	bool res = false;
    
    string fname = CT2A(lpszPathName);
    CRaptorToolBox::LP_SCENE_LOADER_OPTIONS options = NULL;

    POSITION vPos = GetFirstViewPosition();
    if (vPos != NULL)
    {
        CGLView *view = (CGLView*)GetNextView(vPos);
        view->glMakeCurrent();

		if ((string::npos != fname.rfind(".3DS")) || (string::npos != fname.rfind(".3ds")))
			res = CRaptorToolBox::load3DStudioScene(fname,m_pRoot,options);
		else if ((string::npos != fname.rfind(".OBJ")) || (string::npos != fname.rfind(".obj")))
			res = CRaptorToolBox::loadWavefrontScene(fname,m_pRoot,options);

        if (!res)
        {
            CString msg;
			LPWSTR f = CA2T(fname.c_str());
            msg.Format(TEXT("Failed to load file %s"),f);
            AfxMessageBox(msg);
        }
        else
        {
            updateView = true;
			m_pRoot->translateAbsolute(0.0f,0.0f,0.0f);
        }

		CMainFrame *pFrame = (CMainFrame*)(AfxGetMainWnd());
		pFrame->updateTools(m_pRoot);

        view->glMakeCurrent(false);
    }
	
	return (res);
}

BOOL CGLObjectViewerDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	bool res = false;

	POSITION vPos = GetFirstViewPosition();
    if (vPos != NULL)
    {
        CGLView *view = (CGLView*)GetNextView(vPos);
        view->glMakeCurrent();

		string fname = CT2A(lpszPathName);
		if ((string::npos != fname.rfind(".OBJ")) || (string::npos != fname.rfind(".obj")))
			res = CRaptorToolBox::saveWavefrontScene("Raptor.obj",m_pRoot);

		view->glMakeCurrent(false);
	}

	return (res ? TRUE : FALSE);
}

void CGLObjectViewerDoc::setBBoxes(bool bboxes)
{
	m_pRootInstance->renderBoxes(bboxes);
}

void CGLObjectViewerDoc::selectObject(CObject3D *pObject)
{
	m_pRootInstance->setSelection(pObject);
}

void CGLObjectViewerDoc::GLInitContext(void)
{
    POSITION vPos = GetFirstViewPosition();
    if (vPos != NULL)
    {
        CGLView *view = (CGLView*)GetNextView(vPos);

		RECT rect;
		view->GetClientRect(&rect);
		float ratio = ((float)(rect.right - rect.left)) / ((float)(rect.bottom - rect.top));

        CRaptorDisplay* const pDisplay = view->getDisplay();
		IRenderingProperties &props = pDisplay->getRenderingProperties();
        props.clear(CGL_RGBA|CGL_DEPTH);
		props.setTexturing(IRenderingProperties::DISABLE);
		props.setLighting(IRenderingProperties::DISABLE);
		props.setWireframe(IRenderingProperties::DISABLE);

        IViewPoint *vp = pDisplay->getViewPoint();
        vp->setViewVolume(-ratio,ratio,-1.0f,1.0f,1.0f,10000.0f,IViewPoint::PERSPECTIVE);
        vp->glvkRenderViewPointModel();
		const CBoundingBox * const bbox = m_pRoot->boundingBox();
		vp->translate(0.0f,0.0f,bbox->zMax());

		CLight *pLight = new CLight();
		pLight->setAmbient(1.0f,1.0f,1.0f,1.0f);
		pLight->setDiffuse(1.0f,1.0f,1.0f,1.0f);
		pLight->setSpecular(1.0f,1.0f,1.0f,1.0f);
		pLight->setShininess(20.0f);

		C3DScene *const pScene = pDisplay->getRootScene();
		pScene->useZSort();
		pScene->addLight(pLight);

		GL_COORD_VERTEX lpos(0.0f,100.0f,100.0f,1.0f);
		pLight->setLightPosition(lpos);
		GL_COORD_VERTEX ldir(0.0f,0.0f,0.0f,1.0f);
		pLight->setLightDirection(ldir);

		m_pRootInstance = new CRootInstance();
		m_pRootInstance->instantiate(m_pRoot);
        
		pScene->addObject(m_pRootInstance);
    }
}

