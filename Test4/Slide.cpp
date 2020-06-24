#include "StdAfx.h"

#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/TextureFactory.h"
#include "System/Raptor.h"
#include "Engine/Animator.h"
#include "ToolBox/RaptorToolBox.h"

#include "Slide.h"
#include "PathObject.h"

CShadedGeometry  *CSlide::m_pSlide = NULL;
CMaterial  *CSlide::m_pMaterial = NULL;
CMaterial  *CSlide::m_pMaterial2 = NULL;
CMaterial  *CSlide::m_pMaterial3 = NULL;
bool          CSlide::m_bHighLight = false;

CSlide::CSlide(const string& name,CPathObject& path)
    :mPath(path),tpos(0.5f),mVideo(NULL),mStream(0),tw(1.0f),th(1.0f)
{
    GL_COORD_VERTEX lowerLeft(-2.0f,-2.0f,0.0f,1.0f);
    GL_COORD_VERTEX upperRight(2.0f,2.0f,0.0f,1.0f);

    extendBoundingBox(lowerLeft,upperRight);
    translateCenter(0.0f,2.0f,-1.0f);
    setName(name);

    if (m_pMaterial == NULL)
    {
        m_pMaterial = new CMaterial(CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,4.0f,CMaterial::CGL_NO_MATERIAL,name+"_mat");
        m_pMaterial->setAmbient(0.1f,0.0f,0.0f,1.0f);
        m_pMaterial->setDiffuse(0.8f,0.0f,0.0f,1.0f);
        m_pMaterial->setSpecular(0.5f,0.5f,0.5f,1.0f);
    }
    
    if (m_pMaterial2 == NULL)
    {
        m_pMaterial2 = new CMaterial(CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,1.0f,CMaterial::CGL_NO_MATERIAL,name+"_matHigh");
        m_pMaterial2->setAmbient(0.1f,0.1f,0.1f,0.5f);
        m_pMaterial2->setDiffuse(0.8f,0.8f,0.0f,0.5f);
        m_pMaterial2->setSpecular(0.9f,0.9f,0.7f,0.5f);
    }

    if (m_pMaterial3 == NULL)
    {
        m_pMaterial3 = new CMaterial(CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,CMaterial::CGL_NO_MATERIAL,1.0f,CMaterial::CGL_NO_MATERIAL,name+"_matLow");
        m_pMaterial3->setAmbient(0.1f,0.1f,0.0f,0.5f);
        m_pMaterial3->setDiffuse(0.6f,0.6f,0.0f,0.5f);
        m_pMaterial3->setSpecular(0.7f,0.7f,0.4f,0.5f);
    }

    if (m_pSlide == NULL)
    {
        C3DSet *set = NULL;
        CRaptorToolBox::SCENE_LOADER_OPTIONS options;
        options.texturePath = "Datas";
        CRaptorToolBox::load3DStudioScene("Datas\\slide.3DS",set,&options);

        C3DSet::C3DSetIterator it = set->getIterator();
	    m_pSlide = (CShadedGeometry*)(set->getChild(it++));
		m_pSlide->setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
		m_pSlide->addModel(CGeometry::CGL_NORMALS);
        m_pSlide->translateAbsolute(0.0f,0.0f,0.0f);
    }
}

CSlide::~CSlide(void)
{
    if (m_pMaterial != NULL)
    {
        delete m_pMaterial;
        m_pMaterial = NULL;
    }
    if (m_pMaterial2 != NULL)
    {
        delete m_pMaterial2;
        m_pMaterial2 = NULL;
    }
    if (m_pMaterial3 != NULL)
    {
        delete m_pMaterial3;
        m_pMaterial3 = NULL;
    }
    if (mVideo != NULL)
        mVideo->releaseReference();
}

void CSlide::setVideo(const string& vName)
{
    CAnimator *pAnimator = CAnimator::GetAnimator();

    ITextureGenerator *pGenerator = pAnimator->glStartPlayBack(vName,true);

    if (mVideo != NULL)
        mVideo->releaseReference();

    CTextureFactory f;
    mVideo = f.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
										ITextureObject::CGL_BILINEAR,
										pGenerator);

	tw = (float)(pGenerator->getGenerateWidth()) / (float)(mVideo->getWidth());
	th = (float)(pGenerator->getGenerateHeight()) / (float)(mVideo->getHeight());
}

void CSlide::translate(float dt)
{
    tpos += dt;

    GL_COORD_VERTEX c;
    getCenter(c);
    GL_COORD_VERTEX p = mPath.getTPos(tpos);

    translateCenter(p.x - c.x,p.y - c.y,p.z - c.z);
}

void CSlide::glRender(void)
{
    GL_COORD_VERTEX center;
    getCenter(center);

    glPushMatrix();
    glTranslatef(center.x,center.y,center.z);
    glRotatef(-20.0f,1.0f,0.0f,0.0f);

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);

    m_pMaterial->glRender();
    glNormal3f(0.0f,0.0f,1.0f);

    if (mVideo == NULL)
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glVertex3f(-2.0f,-2.0f,0.1f);
            glVertex3f(2.0f,-2.0f,0.1f);
            glVertex3f(2.0f,2.0f,0.1f);
            glVertex3f(-2.0f,2.0f,0.1f);
        glEnd();
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
		mVideo->glvkRender();
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);   glVertex3f(-2.0f,-2.0f,0.1f);
            glTexCoord2f(tw,0.0f);   glVertex3f(2.0f,-2.0f,0.1f);
            glTexCoord2f(tw,th);   glVertex3f(2.0f,2.0f,0.1f);
            glTexCoord2f(0.0f,th);   glVertex3f(-2.0f,2.0f,0.1f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    if (m_bHighLight)
        m_pMaterial2->glRender();
    else
        m_pMaterial3->glRender();

    m_pSlide->glRender();

    glPopAttrib();

    glPopMatrix();
}

