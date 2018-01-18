// ObjectDialog.cpp : implementation file
//
#include "StdAfx.h"

#include "Resource.h"

#if !defined(AFX_OBJECTDIALOG_H__5561BA29_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "ObjectDialog.h"
#endif
#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif
#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
    #include "GLHierarchy/ShadedGeometry.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_)
	#include "GLHierarchy/GLLod.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_GLVIEW_H__6E79FB08_9C61_11D3_9855_383E0D000000__INCLUDED_)
	#include "CWnd/GLView.h"
#endif

#include <afxcmn.h>


RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog
	void AddObject(CTreeCtrl *tree,HTREEITEM parentItem,CPersistence *obj);

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CPersistence* obj)
	{
		CString str;
		str.Format(TEXT("Address: 0x%p"), obj);
		tree->InsertItem(str,parentItem);
        string name = string("Name: ") + obj->getName();
		tree->InsertItem(CA2T(name.data()),parentItem);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,const CBoundingBox* const obj)
	{
		HTREEITEM item = tree->InsertItem(TEXT("BBox"), parentItem);
		CString str;
		str.Format(TEXT("Min: %f %f %f"), obj->xMin(), obj->yMin(), obj->zMin());
		tree->InsertItem(str,item);
		str.Format(TEXT("Max: %f %f %f"), obj->xMax(), obj->yMax(), obj->zMax());
		tree->InsertItem(str,item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,C3DEngine* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CMaterial* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);

		CString str;
		CColor::RGBA color = obj->getAmbient();
		str.Format(TEXT("Ambient: r (%f) g (%f) b (%f) a (%f)"), color.r, color.g, color.b, color.a);
		tree->InsertItem(str,item);

		color = obj->getDiffuse();
		str.Format(TEXT("Diffuse: r (%f) g (%f) b (%f) a (%f)"), color.r, color.g, color.b, color.a);
		tree->InsertItem(str,item);

		color = obj->getSpecular();
		str.Format(TEXT("Specular: r (%f) g (%f) b (%f) a (%f)"), color.r, color.g, color.b, color.a);
		tree->InsertItem(str,item);

		color = obj->getShininess();
		str.Format(TEXT("Shininess: s (%f)"), color.r);
		tree->InsertItem(str,item);

		color = obj->getEmission();
		str.Format(TEXT("Emission: r (%f) g (%f) b (%f) a (%f)"), color.r, color.g, color.b, color.a);
		tree->InsertItem(str,item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CLight* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CMaterial*)obj);

		CString str;
		GL_COORD_VERTEX v = obj->getLightPosition();
		str.Format(TEXT("Position: x (%f) y (%f) z (%f) h (%f)"), v.x, v.y, v.z, v.h);
		tree->InsertItem(str,item);

		v = obj->getLightDirection();
		str.Format(TEXT("Direction: x (%f) y (%f) z (%f) h (%f)"), v.x, v.y, v.z, v.h);
		tree->InsertItem(str,item);

		v = obj->getSpotParams();
		str.Format(TEXT("Spot: quadratic (%f) linear (%f) constant (%f) cutoff (%f)"),v.x,v.y,v.z,v.h);
		tree->InsertItem(str,item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CTextureObject* obj)
	{
		HTREEITEM item = tree->InsertItem(TEXT("<Image>"), parentItem);
		CString str;
	
		str.Format(TEXT("Object: 0x%p"), obj);
		tree->InsertItem(str,item);

		if (NULL == obj)
			return;
		
		tree->InsertItem(CString("Source: ")+obj->getName().data(),item);

		str.Format(TEXT("Size: w (%u) h (%u)"), obj->getWidth(), obj->getHeight());
		tree->InsertItem(str,item);
		str.Format(TEXT("Alpha: %u"), obj->getTransparency());
		tree->InsertItem(str,item);
		switch(obj->getFilter())
		{
            case CTextureObject::CGL_UNFILTERED: str = "Filtering : none"; break;
			case CTextureObject::CGL_BILINEAR: str = "Filtering : bilinear"; break;
			case CTextureObject::CGL_BILINEAR_MIPMAPPED: str = "Filtering : bilinear mipmapped"; break;
			case CTextureObject::CGL_TRILINEAR: str = "Filtering : trilinear"; break;
            case CTextureObject::CGL_ANISOTROPIC: str = "Filtering : anisotropic"; break;
			default : str = "Filtering : unknown";break;
		}
		tree->InsertItem(str,item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CTextureSet* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);
		for (unsigned int i=0;i<obj->getNbTexture();i++)
		{
			Add(tree,item,obj->getTexture(i));
		}
	}

	void Add(CTreeCtrl *tree,HTREEITEM item,CObject3D *obj)
	{
		const CPersistence::CPersistenceClassID &ID = obj->getId();

		if (ID.isSubClassOf(C3DSet::C3DSetClassID().GetClassId()))
			Add(tree,item,(C3DSet*)obj);
		else if (ID.isSubClassOf(CGeometry::CGeometryClassID().GetClassId()))
			Add(tree,item,(CGeometry*)obj);
		else if (ID.isSubClassOf(CObject3DInstance::CObject3DInstanceClassID().GetClassId()))
			Add(tree,item,(CObject3DInstance*)obj);
		else if (ID.isSubClassOf(CGLLod::CGLLodClassID().GetClassId()))
			Add(tree,item,(CGLLod*)obj);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CGeometry* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);
		Add(tree,item,obj->boundingBox());

		CShadedGeometry	*shade = NULL;
		if (obj->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
			shade = (CShadedGeometry*)obj;

		CString str;
		if (shade != NULL)
		{
			if ((shade->getShader() != NULL) && shade->getShader()->hasMaterial())
				Add(tree,item,shade->getShader()->getMaterial());
			else
				tree->InsertItem(TEXT("<No Material>"), item);
			if ((shade->getShader() != NULL) && shade->getShader()->hasTextureUnitSetup())
			{
				if (shade->getShader()->glGetTextureUnitsSetup()->getDiffuseMap() != NULL)
					Add(tree,item,shade->getShader()->glGetTextureUnitsSetup()->getDiffuseMap());
				if (shade->getShader()->glGetTextureUnitsSetup()->getNormalMap() != NULL)
					Add(tree,item,shade->getShader()->glGetTextureUnitsSetup()->getNormalMap());
				if (shade->getShader()->glGetTextureUnitsSetup()->getLightMap() != NULL)
					Add(tree,item,shade->getShader()->glGetTextureUnitsSetup()->getLightMap());
				if (shade->getShader()->glGetTextureUnitsSetup()->getEnvironmentMap() != NULL)
					Add(tree,item,shade->getShader()->glGetTextureUnitsSetup()->getEnvironmentMap());
			}
			else
				tree->InsertItem(TEXT("<No Texture>"), item);

			CColor::RGBA color;
			if (shade->getShader() != NULL)
				color = shade->getShader()->getColor();
			str.Format(TEXT("Color: r (%f) g (%f) b (%f) a (%f)"), color.r, color.g, color.b, color.a);
			tree->InsertItem(str,item);
		}

		str.Format(TEXT("Rendering model: %x"), obj->getRenderingModel());
		tree->InsertItem(str,item);

		str.Format(TEXT("nbVertex: %u"), obj->nbVertex());
		tree->InsertItem(str,item);
		str.Format(TEXT("nbFace: %u"), obj->nbFace());
		tree->InsertItem(str,item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CObject3DInstance* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);
		
		CGenericMatrix<float> T;
		obj->getTransform(T);

		CString str;
		str.Format(TEXT("Transform rowx: %f %f %f %f"), T[0], T[1], T[2], T[3]);
		tree->InsertItem(str,item);
		str.Format(TEXT("Transform rowy: %f %f %f %f"), T[4], T[5], T[6], T[7]);
		tree->InsertItem(str,item);
		str.Format(TEXT("Transform rowz: %f %f %f %f"), T[8], T[9], T[10], T[11]);
		tree->InsertItem(str,item);
		str.Format(TEXT("Transform rowh: %f %f %f %f"), T[12], T[13], T[14], T[15]);
		tree->InsertItem(str,item);

		if (obj->getObject() != NULL)
			Add(tree,item,obj->getObject());
		else
			tree->InsertItem(TEXT("<No instance>"), item);
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,CGLLod* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CObject3DInstance*)obj);

		CString str;
		str.Format(TEXT("Number of LODs: %u"), obj->getNbLod());
		tree->InsertItem(str,item);

		for (unsigned int i=0;i<obj->getNbLod();i++)
		{
			Add(tree,item,obj->getLod(i));
		}
	}

	void Add(CTreeCtrl *tree,HTREEITEM parentItem,C3DSet* obj)
	{
		HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
		Add(tree,item,(CPersistence*)obj);
		Add(tree,item,obj->boundingBox());

        C3DSet::C3DSetIterator it = obj->getIterator();
		CObject3D *child = obj->getChild(it++);
		while(child != NULL)
		{
			Add(tree,item,child);

			child = obj->getChild(it++);
		}
	}


	void AddObject(CTreeCtrl *tree,HTREEITEM parentItem,CPersistence *obj)
	{
		const CPersistence::CPersistenceClassID &ID = obj->getId();

		if (ID.isSubClassOf(C3DEngine::C3DEngineClassID().GetClassId()))
			Add(tree,parentItem,(C3DEngine*)obj);
		else if (ID.isSubClassOf(C3DSet::C3DSetClassID().GetClassId()))
			Add(tree,parentItem,(C3DSet*)obj);
		else if (ID.isSubClassOf(CGeometry::CGeometryClassID().GetClassId()))
			Add(tree,parentItem,(CGeometry*)obj);
		else if (ID.isSubClassOf(CLight::CLightClassID().GetClassId()))
			Add(tree,parentItem,(CLight*)obj);
		else if (ID.isSubClassOf(CMaterial::CMaterialClassID().GetClassId()))
			Add(tree,parentItem,(CMaterial*)obj);
		else if (ID.isSubClassOf(CTextureFactory::CTextureFactoryClassID().GetClassId()))
			Add(tree,parentItem,(CTextureFactory*)obj);
		else if (ID.isSubClassOf(CObject3DInstance::CObject3DInstanceClassID().GetClassId()))
			Add(tree,parentItem,(CObject3DInstance*)obj);
		else if (ID.isSubClassOf(CGLLod::CGLLodClassID().GetClassId()))
			Add(tree,parentItem,(CGLLod*)obj);
		else
		{
			HTREEITEM item = tree->InsertItem(CA2T(obj->getId().ClassName()),parentItem);
			Add(tree,item,obj);
		}
	};

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog


CObjectDialog::CObjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectDialog::IDD, pParent),GLContext(NULL)
{
	//{{AFX_DATA_INIT(CObjectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectDialog, CDialog)
	//{{AFX_MSG_MAP(CObjectDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog message handlers

BOOL CObjectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    if (GLContext!=NULL)
		GLContext->glMakeCurrent();
	else
	{
		CRaptorDisplayConfig glCS;
		glCS.width = 0;
		glCS.height = 0;
		glCS.x = 0;
		glCS.y = 0;
		glCS.caption = "Raptor Release test";
		glCS.double_buffer = true;
		glCS.depth_buffer = true;
		glCS.display_mode = CGL_RGBA | CGL_DEPTH;
		Context.GLCreateWindow("OpenGL Context",this,glCS);
		Context.glMakeCurrent();
	}


	void *pos = NULL;
	CPersistence	*obj = CPersistence::Object((void*&)pos);

	if (obj != NULL)
	{
		CTreeCtrl	*t = (CTreeCtrl*)this->GetDlgItem(IDC_OBJECTTREE);
		HTREEITEM	root = t->GetRootItem( );

		do
		{
			if (obj != NULL)
			{
				AddObject(t,root,obj);
			}
			obj = CPersistence::Object((void*&)pos);
		}
		while (pos != NULL);
	}

    if (GLContext!=NULL)
		GLContext->glMakeCurrent(false);
	else
	{
        Context.glMakeCurrent(false);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CObjectDialog::OnOK() 
{
	Context.DestroyWindow();
	
	CDialog::OnOK();
}

void CObjectDialog::OnCancel() 
{
	Context.DestroyWindow();
	
	CDialog::OnCancel();
}
