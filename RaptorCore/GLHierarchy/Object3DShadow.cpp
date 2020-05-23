// Object3DShadow.cpp: implementation of the CObject3DShadow class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OBJECT3DSHADOW_H__A502A78F_0575_4F30_BCE2_0658B60EB490__INCLUDED_)
	#include "Object3DShadow.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DSet.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "Object3DInstance.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "Light.h"
#endif
#if !defined(AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_)
	#include "Subsys/ContourAttributes.h"
#endif
#if !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)
    #include "Subsys/Object3DShadowAttributes.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_SHADOWVOLUMEJOB_H__3C3EB9C3_88DF_4C32_B66E_5265B65D8648__INCLUDED_)
    #include "Subsys/ShadowVolumeJob.h"
#endif
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif
#if !defined(AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_)
	#include "GLHierarchy/GLLod.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif



RAPTOR_NAMESPACE

static CObject3DShadow::CObject3DShadowClassID shadowId;
const CPersistence::CPersistenceClassID& CObject3DShadow::CObject3DShadowClassID::GetClassId(void)
{
	return shadowId;
}

CEngineJob* CObject3DShadow::createJob(unsigned int batchId)
{
    return new CShadowVolumeJob(this,batchId);
}

//////////////////////////////////////////////////////////////////////
// CObject3DShadow
//////////////////////////////////////////////////////////////////////

CObject3DShadow::CObject3DShadow(CObject3D *source,SHADOW_TYPE shadowType):
	CObject3D(shadowId,"OBJECT3D-SHADOW"),
	m_type(shadowType)
{
	m_pAttributes = new CObject3DShadowAttributes();
	m_pAttributes->m_color_red = 0;
	m_pAttributes->m_color_green = 0;
	m_pAttributes->m_color_blue = 0;
	m_pAttributes->reBuild = false;
    m_pAttributes->m_pObject = source;

	if (source == NULL)
	{
		RAPTOR_WARNING(	CObject3DShadow::CObject3DShadowClassID::GetClassId(),
						CRaptorMessages::ID_NULL_OBJECT);
	}
    else
    {
        initContours();
    }
}


CObject3DShadow::~CObject3DShadow()
{
    if (m_pAttributes->m_shadowColors.size() > 0)
    {
        for (unsigned int pos = 0; pos < m_pAttributes->m_shadowColors.size() ; pos++)
            delete [] m_pAttributes->m_shadowColors[pos];
    }
    if (m_pContours.size() > 0)
    {
        for (unsigned int pos = 0; pos < m_pContours.size() ; pos++)
            delete m_pContours[pos];
    }

	delete m_pAttributes;
}

void CObject3DShadow::initContours()
{
    if ((m_type == SHADOW_VOLUME) || (m_type == SHADOW_BOUNDING_VOLUME))
    {
        bool invalidSource = false;
		CObject3D *shadowSource = m_pAttributes->m_pObject;

		vector<CObject3DContour*> pContours = shadowSource->createContours();

		if (!pContours.empty())
        {
            vector<CObject3DContour*>::const_iterator it = pContours.begin();
            while (it != pContours.end())
                m_pContours.push_back(*it++);
        }
        else
        {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
            Raptor::GetErrorManager()->generateRaptorError(	CObject3DShadow::CObject3DShadowClassID::GetClassId(),
														 	CRaptorErrorManager::RAPTOR_WARNING,
															CRaptorMessages::ID_INIT_FAILED);
#endif
            invalidSource = true;
        }

        if (!invalidSource)
        {
            //! Clipping will update this vector per frame
            m_pVisibleContours = m_pContours;

            vector<CObject3DContour*>::iterator it = m_pContours.begin();
            while (it != m_pContours.end())
            {
                (*it++)->prepareContour();
                if (m_type == SHADOW_BOUNDING_VOLUME)
                    setShadowColor(255,255,255,255);
            }
        }
    }
}


void CObject3DShadow::setLight(CLight *light)
{
	m_pAttributes->m_pLight = light;
}

void CObject3DShadow::setLightPosition(float lpx,float lpy,float lpz)
{
	m_pAttributes->m_light.Set(lpx,lpy,lpz,1.0f);
}

void CObject3DShadow::setShadowPlane(CGenericVector<float> &o,CGenericVector<float> &n)
{
    m_pAttributes->plane = o;
    m_pAttributes->normal = n;
    m_pAttributes->reBuild = true;
}

void CObject3DShadow::setShadowExtrusion(float extrusion)
{
    m_pAttributes->extrusion = extrusion;
    m_pAttributes->reBuild = true;
}

void CObject3DShadow::glSetShadowPlaneDistance(float distance)
{
    GL_COORD_VERTEX center;
	m_pAttributes->m_pObject->getCenter(center);

	CGenericVector<float> lp;
	m_pAttributes->glQueryLightPosition(lp);
	CGenericVector<float> ap = (float*)center;
	
	ap -= lp;
	ap.Normalize();

	lp.X() = center.x + (ap.X() * distance);
	lp.Y() = center.y + (ap.Y() * distance);
	lp.Z() = center.z + (ap.Z() * distance);

	!ap;

    setShadowPlane(lp,ap);
}

void CObject3DShadow::setShadowColor(unsigned char cr,unsigned char cg,unsigned char cb,unsigned char ca)
{
	m_pAttributes->m_color_red = ((float)cr) / 255.0f;
	m_pAttributes->m_color_green = ((float)cg) / 255.0f;
	m_pAttributes->m_color_blue = ((float)cb) / 255.0f;
	m_pAttributes->m_color_alpha = ((float)ca) / 255.0f;

    if ((m_type == SHADOW_VOLUME) || (m_type == SHADOW_BOUNDING_VOLUME))
    {
        CObject3DContour *pContour = m_pVisibleContours[0];
	    if (pContour->getGeometry() != NULL)
	    {
		    unsigned int color =	((int)(255.0f*m_pAttributes->m_color_red) ) +
							    ((int)(255.0f*m_pAttributes->m_color_green) << 8) +
							    ((int)(255.0f*m_pAttributes->m_color_blue) << 16) +
							    ((int)(255.0f*m_pAttributes->m_color_alpha) << 24);

		    unsigned int color0 =	((int)(255.0f*m_pAttributes->m_color_red) ) +
							    ((int)(255.0f*m_pAttributes->m_color_green) << 8) +
							    ((int)(255.0f*m_pAttributes->m_color_blue) << 16);

            unsigned int max = pContour->getGeometry()->nbVertex();

            if (m_pAttributes->m_shadowColors.size() == 0)
		        m_pAttributes->m_shadowColors.push_back(new unsigned int[2 * max]);

            unsigned int *colors = m_pAttributes->m_shadowColors[0];
		    for (unsigned int j = 0; j<max;j++)
		    {
			    colors[j] = color;
			    colors[j+max] = color0;
		    }
	    }
    }
}


void CObject3DShadow::glRender(void) 
{
    //! CAUTION : This test case can be completely wrong if visibility is a result 
    //! of frustum clipping : depending on light's position, any non viewable
    //! object can cast a shadow into the visible frustum.!!!
    if ((m_pAttributes->m_pObject == NULL) ||
        !m_pAttributes->m_pObject->getProperties().isVisible())
        return;

	CGenericMatrix<float> modelview;
	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,modelview);
	buildShadow(modelview);

	bool transform = m_pAttributes->m_pObject->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId());
    if ( transform )
    {
        glPushMatrix();
        GL_MATRIX T = ((CObject3DInstance*)m_pAttributes->m_pObject)->getTransform();
        glMultMatrixf(T);
    }

    switch(m_type)
    {
        case SHADOW_PLANAR:
            glRender(m_pAttributes->m_pObject);
            break;
        case SHADOW_BOUNDING_VOLUME:
            glRenderShadowContour();
            break;
        case SHADOW_VOLUME:
            glRenderShadowVolume();
            break;
        default:
            if (m_pAttributes->m_pObject != NULL)
	            glRender(m_pAttributes->m_pObject);
    }

    if ( transform )
    {
        glPopMatrix();
    }

    CATCH_GL_ERROR
}

void CObject3DShadow::buildShadow(CGenericMatrix<float> &modelview) 
{
	switch(m_type)
	{
		case SHADOW_PLANAR:
		{
			if ((m_pAttributes->reBuild) || 
				(0 != memcmp(modelview.matrix(),m_pAttributes->m_projection.matrix(),16*sizeof(float))))
			{
				buildPlanarShadow(m_pAttributes->plane,m_pAttributes->normal);
				m_pAttributes->reBuild = false;
			}
			break;
		}
		case SHADOW_BOUNDING_VOLUME:
		case SHADOW_VOLUME:
		{
			if (m_pAttributes->reBuild)
			{
				CObject3DContour *pContour = m_pVisibleContours[0];

				CGenericMatrix<float> T;
				T.Ident();
				if ( m_pAttributes->m_pObject->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId()) )
					T = ((CObject3DInstance*)m_pAttributes->m_pObject)->getTransform();

				C3DEngineMatrix M = modelview * T.Transpose();

				//  update extrusion properly
				const CBoundingBox *const bbox = pContour->getBoundingBox();
				float z_max = C3DEngine::Get3DEngine()->getFarPlane() + M[11] - bbox->getDiameter();
				if (m_pAttributes->m_pLight->getLightDMax() < z_max)
					m_pAttributes->extrusion = m_pAttributes->m_pLight->getLightDMax();
				else
					m_pAttributes->extrusion = z_max;

				CGenericVector<float> lp = M.Inverse() * m_pAttributes->m_pLight->getLightEyePosition();
				GL_COORD_VERTEX L(lp.X(),lp.Y(),lp.Z(),lp.H());
				pContour->buildVolume(L,z_max);
				m_pAttributes->reBuild = false;
				break;
			}
		}
		default:
			break;
	}
}

void RAPTOR_FASTCALL CObject3DShadow::glRender(CObject3D *source)
{
	if ((source != NULL) && (m_type == SHADOW_PLANAR))
	{
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);

		glPolygonOffset(-0.5f,-1.0f);

		glMultMatrixf(m_pAttributes->m_projection.matrix());
	
		GLint blend_src;
		GLint blend_dst;
		glGetIntegerv(GL_BLEND_SRC,&blend_src);
		glGetIntegerv(GL_BLEND_DST,&blend_dst);

#if defined(GL_ARB_imaging)
		if(m_pAttributes->glBlendColorARB != NULL)
		{
			m_pAttributes->glBlendColorARB(m_pAttributes->m_color_red,
										m_pAttributes->m_color_green,
										m_pAttributes->m_color_blue,
										m_pAttributes->m_color_alpha);
			glBlendFunc(GL_ZERO,GL_CONSTANT_COLOR_ARB);
		}
		else
#endif
			glBlendFunc(GL_ZERO,GL_SRC_ALPHA);

		source->glRender();
		glBlendFunc(blend_src,blend_dst);

		glPopAttrib();

		glPopMatrix();
	}

	CATCH_GL_ERROR
}


void CObject3DShadow::buildPlanarShadow(CGenericVector<float> &o,CGenericVector<float> &n)
{	
	float D;

	CGenericVector<float> light;
	m_pAttributes->glQueryLightPosition(light);
    m_pAttributes->plane = o;
    m_pAttributes->normal = n;

	//	D = d + a.vx + b.vy + c.vz
	//
	//	Projection matrix:
	//
	//	-D + a.vx	b.vx		c.vx		D.vx - a.vx^2 - b.vx.vy - c.vx.vz
	//	a.vy		-D + b.vy	c.vy		D.vy - a.vx.vy - b.vy^2 - c.vy.vz
	//	a.vz		b.vz		-D + c.vz	D.vz - a.vx.vz - b.vy.vz - c.vz^2
	//	a			b			c			-a.vx - b.vy - c.vz

	//	find d
	D = -(n % o);

	//	a.vx : b.vy : c.vz
	CGenericVector<float> t = light * n;
	float L = t.X() + t.Y() + t.Z();

	m_pAttributes->m_projection[15] = -L;
	D += L;
	m_pAttributes->m_projection[14] = n.Z();
	m_pAttributes->m_projection[13] = n.Y();
	m_pAttributes->m_projection[12] = n.X();

	m_pAttributes->m_projection[0] = t.X() - D;
	m_pAttributes->m_projection[5] = t.Y() - D;
	m_pAttributes->m_projection[10] = t.Z() - D;

	m_pAttributes->m_projection[1] = n.Y() * light.X();
	m_pAttributes->m_projection[2] = n.Z() * light.X();

	m_pAttributes->m_projection[4] = n.X() * light.Y();
	m_pAttributes->m_projection[6] = n.Z() * light.Y();

	m_pAttributes->m_projection[8] = n.X() * light.Z();
	m_pAttributes->m_projection[9] = n.Y() * light.Z();

	D -= L;
	m_pAttributes->m_projection[3] = light.X() * D;
	m_pAttributes->m_projection[7] = light.Y() * D;
	m_pAttributes->m_projection[11] = light.Z() * D;

	if (m_pAttributes->m_projection[15] < 0 )
		!m_pAttributes->m_projection;

	m_pAttributes->m_projection.Transpose();
}


void CObject3DShadow::glRenderShadowContour()
{
    CObject3DContour *pContour = m_pVisibleContours[0];
    const CObject3DContour::CONTOUR_VOLUME& scv = pContour->getContourVolume();

    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        CGeometryAllocator::GetInstance()->glvkLockMemory(false);

	glColor4f(1.0f,1.0f,1.0f,0.5f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(4,GL_FLOAT,0,scv.volume);
	glColorPointer(4,GL_UNSIGNED_BYTE,0,m_pAttributes->m_shadowColors[0]);
	glDrawElements(GL_QUADS,scv.volumeSize,GL_UNSIGNED_INT,scv.volumeIndexes);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        CGeometryAllocator::GetInstance()->glvkLockMemory(true);

    m_pAttributes->reBuild = true;

    CATCH_GL_ERROR
}

void CObject3DShadow::glRenderShadowVolume()
{
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        CGeometryAllocator::GetInstance()->glvkLockMemory(false);

    CObject3DContour *pContour = m_pVisibleContours[0];
	const CObject3DContour::CONTOUR_VOLUME& scv = pContour->getContourVolume();
/*
	if (extrusion < 0)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
	
		double a,b,c,d,x,y;
		x = (2.0 * 1.0) / (1.33 - -1.33);
		y = (2.0 * 1.0) / (1.0 - -1.0);
		a = (1.33 + -1.33) / (1.33 - -1.33);
		b = (1.0 + -1.0) / (1.0 - -1.0);
	
		c = -1.0;
		d = -2.0 * 1.0;

		double m[] = { x, 0, 0, 0,
									0, y, 0, 0,
									a, b, c, -1,
									0, 0, d, 0 };
		glLoadMatrixd(m);
	}
*/
#if (defined(GL_EXT_stencil_two_side) && defined(GL_EXT_stencil_wrap))
	if (m_pAttributes->glActiveStencilFaceEXT == NULL)
	//if (m_pAttributes->glActiveStencilFaceEXT != NULL)
#endif
	{
		glStencilMask(~0);
		glStencilFunc(GL_ALWAYS,1,1);
		glEnable(GL_CULL_FACE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(4,GL_FLOAT,0,scv.volume);

		// draw front faces
        //  function lifted up to 128 to be able to render in OGL 1.0 ( see C3DScene )
        /*
#if defined(GL_EXT_stencil_wrap)
		glStencilOp(GL_KEEP,GL_INCR_WRAP_EXT,GL_KEEP);
#endif
        */
        glStencilOp(GL_KEEP,GL_INCR,GL_KEEP);
		glCullFace(GL_BACK);
		glDrawElements(GL_QUADS,scv.volumeSize,GL_UNSIGNED_INT,scv.volumeIndexes);
		if (!scv.darkCapClipped)
			glDrawElements(GL_TRIANGLES,scv.darkCapSize,GL_UNSIGNED_INT,scv.darkCapIndexes);
		if (!scv.lightCapClipped)
			glDrawElements(GL_TRIANGLES,scv.lightCapSize,GL_UNSIGNED_INT,scv.lightCapIndexes);

		// draw back faces
        //  function lifted up to 128 to be able to render in OGL 1.0 ( see C3DScene )
        /*
#if defined(GL_EXT_stencil_wrap)
		glStencilOp(GL_KEEP,GL_DECR_WRAP_EXT,GL_KEEP);
#endif
        */
        glStencilOp(GL_KEEP,GL_DECR,GL_KEEP);
		glCullFace(GL_FRONT);
		glDrawElements(GL_QUADS,scv.volumeSize,GL_UNSIGNED_INT,scv.volumeIndexes);
        if (!scv.darkCapClipped)
		    glDrawElements(GL_TRIANGLES,scv.darkCapSize,GL_UNSIGNED_INT,scv.darkCapIndexes);
        if (!scv.lightCapClipped)
		    glDrawElements(GL_TRIANGLES,scv.lightCapSize,GL_UNSIGNED_INT,scv.lightCapIndexes);

		glCullFace(GL_BACK);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
#if (defined(GL_EXT_stencil_two_side) && defined(GL_EXT_stencil_wrap))
	else
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

		m_pAttributes->glActiveStencilFaceEXT(GL_BACK);
		glStencilOp(GL_KEEP,GL_INCR_WRAP_EXT,GL_KEEP);
		glStencilMask(~0);
		glStencilFunc(GL_ALWAYS,0,~0);

		m_pAttributes->glActiveStencilFaceEXT(GL_FRONT);
		glStencilOp(GL_KEEP,GL_DECR_WRAP_EXT,GL_KEEP);
		glStencilMask(~0);
		glStencilFunc(GL_ALWAYS,0,~0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(4,GL_FLOAT,0,scv.volume);

		glDrawElements(GL_QUADS,scv.volumeSize,GL_UNSIGNED_INT,scv.volumeIndexes);
        if (!scv.darkCapClipped)
		    glDrawElements(GL_TRIANGLES,scv.darkCapSize,GL_UNSIGNED_INT,scv.darkCapIndexes);
        if (!scv.lightCapClipped)
		    glDrawElements(GL_TRIANGLES,scv.lightCapSize,GL_UNSIGNED_INT,scv.lightCapIndexes);

		glEnable(GL_CULL_FACE);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
	}
#endif
/*
	if (extrusion < 0)
	{
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
*/
    if (CGeometryAllocator::GetInstance()->isMemoryRelocated())
        CGeometryAllocator::GetInstance()->glvkLockMemory(true);

    m_pAttributes->reBuild = true;

	CATCH_GL_ERROR
}

void CObject3DShadow::glRenderBBox(bool filled)
{
    if (m_type == SHADOW_PLANAR)
        return;

	bool transform = m_pAttributes->m_pObject->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId());
    if ( transform )
    {
        glPushMatrix();
        GL_MATRIX T = ((CObject3DInstance*)m_pAttributes->m_pObject)->getTransform();
        glMultMatrixf(T);
    }

	GLenum mode = GL_LINE;
	if (filled)
		mode = GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK,mode);

    CObject3DContour *pContour = m_pVisibleContours[0];
    const CBoundingBox    *const bbox = pContour->getBoundingBox();
    CGenericVector<float> lp;
	m_pAttributes->glQueryLightPosition(lp);
    float ext = m_pAttributes->extrusion;

    //  step1 : render origin bbox
    float xmin,xmax,ymin,ymax,zmin,zmax;
	bbox->get(xmin,ymin,zmin,xmax,ymax,zmax);
    glBegin(GL_QUADS);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
	glEnd();

    GL_COORD_VERTEX baseBBox[8] = 
    {   GL_COORD_VERTEX(xmin, ymin, zmin, 1.0f), 
        GL_COORD_VERTEX(xmax, ymin, zmin, 1.0f),
        GL_COORD_VERTEX(xmin, ymax, zmin, 1.0f), 
        GL_COORD_VERTEX(xmax, ymax, zmin, 1.0f),
        GL_COORD_VERTEX(xmin, ymin, zmax, 1.0f), 
        GL_COORD_VERTEX(xmax, ymin, zmax, 1.0f),
        GL_COORD_VERTEX(xmin, ymax, zmax, 1.0f), 
        GL_COORD_VERTEX(xmax, ymax, zmax, 1.0f)
    };
    GL_COORD_VERTEX extBBox[8];
    CGenericVector<float> lightVect;

    //  Step 2 : render extruded origin bbox
    for (unsigned int i=0;i<8;i++)
    {
        const GL_COORD_VERTEX& box = baseBBox[i];

        lightVect.Set(	box.x - lp.X(), box.y - lp.Y(), box.z - lp.Z(), 	1.0f);
        float norm = ext / sqrt(lightVect.X()*lightVect.X() + lightVect.Y()*lightVect.Y() + lightVect.Z()*lightVect.Z());
	    extBBox[i].x = lightVect.X()*norm + box.x;
	    extBBox[i].y = lightVect.Y()*norm + box.y;
	    extBBox[i].z = lightVect.Z()*norm + box.z;
	    extBBox[i].h = 1.0f;
    }

    CBoundingBox ebbox;
    ebbox.set(extBBox[0].x,extBBox[0].y,extBBox[0].z,extBBox[1].x,extBBox[1].y,extBBox[1].z);
    ebbox.extendTo(extBBox[2].x,extBBox[2].y,extBBox[2].z);
    ebbox.extendTo(extBBox[3].x,extBBox[3].y,extBBox[3].z);
    ebbox.extendTo(extBBox[4].x,extBBox[4].y,extBBox[4].z);
    ebbox.extendTo(extBBox[5].x,extBBox[5].y,extBBox[5].z);
    ebbox.extendTo(extBBox[6].x,extBBox[6].y,extBBox[6].z);
    ebbox.extendTo(extBBox[7].x,extBBox[7].y,extBBox[7].z);
    ebbox.get(xmin,ymin,zmin,xmax,ymax,zmax);
	
	glBegin(GL_QUADS);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
	glEnd();

    //  Step 3 : render sides

/*
	float xmin,xmax,ymin,ymax,zmin,zmax;
	GetBoundingBox()->Get(xmin,ymin,zmin,xmax,ymax,zmax);
	
	glBegin(GL_QUADS);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
	glEnd();
*/
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    if ( transform )
    {
        glPopMatrix();
    }

	CATCH_GL_ERROR
}


void CObject3DShadow::glClipRender(void)
{
#if defined(GL_ARB_occlusion_query)
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
    unsigned int available = 0;

    if (m_pAttributes->queryIssued )
    {
        pExtensions->glGetQueryObjectuivARB(m_pAttributes->visibilityQuery,GL_QUERY_RESULT_AVAILABLE_ARB,&available);
 	    pExtensions->glGetQueryObjectuivARB(m_pAttributes->visibilityQuery,GL_QUERY_RESULT_ARB,&m_pAttributes->visibilityCount);
    }

    pExtensions->glBeginQueryARB(GL_SAMPLES_PASSED_ARB,m_pAttributes->visibilityQuery);
    m_pAttributes->queryIssued = true;

    if ((m_pAttributes->visibilityCount > 0) || (!available))
    {
    	glRender();
/*
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glColor4f(0.0f,1.0f,0.0f,0.4f);
    glRenderBBox(true);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
#endif
*/
    }
    else
    {
        //  actual values should be 'get' and then restored after bbox is rendered
        GLboolean cMask[4];
        glGetBooleanv(GL_COLOR_WRITEMASK ,cMask);
        GLboolean dMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK ,&dMask);
        GLboolean sMask;
        glGetBooleanv(GL_STENCIL_TEST ,&sMask);
        GLint dFunc;
        glGetIntegerv(GL_DEPTH_FUNC,&dFunc);
        GLboolean cFace;
        glGetBooleanv(GL_CULL_FACE ,&cFace);

        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        glDepthMask(GL_FALSE);
        glDisable(GL_STENCIL_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_CULL_FACE);

        glRenderBBox(true);

        glDepthMask(dMask);
        glColorMask(cMask[0],cMask[1],cMask[2],cMask[3]);
        if (sMask)
            glEnable(GL_STENCIL_TEST);
        glDepthFunc(dFunc);
        if (cFace)
            glEnable(GL_CULL_FACE);
/*
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f,0.0f,0.0f,0.4f);
    glRenderBBox(true);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
#endif
*/
    }

    pExtensions->glEndQueryARB(GL_SAMPLES_PASSED_ARB);
#else
    if (glClip() != C3DEngine::CLIP_FULL)
        glRender();
#endif
}

void CObject3DShadow::glSelectContours(void)
{
	if (m_pAttributes->m_pObject->getId().isSubClassOf(CGLLod::CGLLodClassID::GetClassId()))
    {
        m_pVisibleContours.clear();
        
        CGLLod *pLod = ((CGLLod*)m_pAttributes->m_pObject);
    
        size_t size = pLod->getNbLod();
        C3DEngine::CLIP_RESULT  res = pLod->glClip();

        if (res == C3DEngine::CLIP_FULL)
            m_pVisibleContours.push_back(m_pContours[size-1]);
        else
        {
            CObject3D *lod = pLod->glGetLod();
            for (size_t i=0;i<size;i++)
            {
                if (pLod->getLod(i) == lod)
                    m_pVisibleContours.push_back(m_pContours[i]);
            }
        }
    }
}

C3DEngine::CLIP_RESULT RAPTOR_FASTCALL CObject3DShadow::glClip(void) const
{
	bool transform = m_pAttributes->m_pObject->getId().isSubClassOf(CObject3DInstance::CObject3DInstanceClassID::GetClassId());
    if ( transform )
    {
        glPushMatrix();
        GL_MATRIX T = ((CObject3DInstance*)m_pAttributes->m_pObject)->getTransform();
        glMultMatrixf(T);
    }

    C3DEngine::CLIP_RESULT  res = C3DEngine::CLIP_NONE;

    switch(m_type)
    {
        case SHADOW_PLANAR:
            res = C3DEngine::CLIP_NONE;
            break;
        case SHADOW_BOUNDING_VOLUME:
        case SHADOW_VOLUME:
        {
            CGenericVector<float> lp;
	        m_pAttributes->glQueryLightPosition(lp);
            CObject3DContour *pContour = m_pVisibleContours[0];
            const CBoundingBox    *const bbox = pContour->getBoundingBox();

            //  Step 1 : intersect volume and frustum
            //  - clip light cap ( i.e. base bbox )
            C3DEngine::CLIP_RESULT clip_light_cap = C3DEngine::Get3DEngine()->glClip(bbox,true,NULL);
            CObject3DContour::CONTOUR_VOLUME &cv = (CObject3DContour::CONTOUR_VOLUME &)pContour->getContourVolume();
            cv.lightCapClipped = (clip_light_cap == C3DEngine::CLIP_FULL);

            float xmin,xmax,ymin,ymax,zmin,zmax;
	        bbox->get(xmin,ymin,zmin,xmax,ymax,zmax);

            GL_COORD_VERTEX baseBBox[8] = 
            {   GL_COORD_VERTEX(xmin, ymin, zmin, 1.0f), 
                GL_COORD_VERTEX(xmax, ymin, zmin, 1.0f),
                GL_COORD_VERTEX(xmin, ymax, zmin, 1.0f), 
                GL_COORD_VERTEX(xmax, ymax, zmin, 1.0f),
                GL_COORD_VERTEX(xmin, ymin, zmax, 1.0f), 
                GL_COORD_VERTEX(xmax, ymin, zmax, 1.0f),
                GL_COORD_VERTEX(xmin, ymax, zmax, 1.0f), 
                GL_COORD_VERTEX(xmax, ymax, zmax, 1.0f)
            };
            GL_COORD_VERTEX extBBox[8];
            CGenericVector<float> lightVect;
            float ext = m_pAttributes->extrusion;

            //  Extrude BBox
            for (unsigned int i=0;i<8;i++)
            {
                const GL_COORD_VERTEX& box = baseBBox[i];
                lightVect.Set(	box.x - lp.X(), box.y - lp.Y(), box.z - lp.Z(), 1.0f);

                float norm = ext / sqrt(lightVect.X()*lightVect.X() + lightVect.Y()*lightVect.Y() + lightVect.Z()*lightVect.Z());
	            extBBox[i].x = lightVect.X()*norm + box.x;
	            extBBox[i].y = lightVect.Y()*norm + box.y;
	            extBBox[i].z = lightVect.Z()*norm + box.z;
	            extBBox[i].h = 1.0f;
            }

            //  Step 2 : intersect volume and frustum
            //  - clip dark cap ( i.e. extruded bbox )
            CBoundingBox ebbox;
            ebbox.set(extBBox[0].x,extBBox[0].y,extBBox[0].z,extBBox[1].x,extBBox[1].y,extBBox[1].z);
            ebbox.extendTo(extBBox[2].x,extBBox[2].y,extBBox[2].z);
            ebbox.extendTo(extBBox[3].x,extBBox[3].y,extBBox[3].z);
            ebbox.extendTo(extBBox[4].x,extBBox[4].y,extBBox[4].z);
            ebbox.extendTo(extBBox[5].x,extBBox[5].y,extBBox[5].z);
            ebbox.extendTo(extBBox[6].x,extBBox[6].y,extBBox[6].z);
            ebbox.extendTo(extBBox[7].x,extBBox[7].y,extBBox[7].z);
            C3DEngine::CLIP_RESULT clip_dark_cap = C3DEngine::Get3DEngine()->glClip(&ebbox,true,NULL);
            cv.darkCapClipped = (clip_dark_cap == C3DEngine::CLIP_FULL);
/*
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	ebbox.Get(xmin,ymin,zmin,xmax,ymax,zmax);
	
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glColor4f(0.5f,0.8f,0.8f,1.0f);
    if (m_pAttributes->shadowVolume.darkCapClipped)
        glColor4f(1.0f,0.5f,0.5f,1.0f);
	glBegin(GL_QUADS);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
	glEnd();
    glPopAttrib();
#endif
*/
            res = C3DEngine::CLIP_NONE;
            break;
        }
        default:
            res = C3DEngine::CLIP_NONE;
            break;
    }

    if ( transform )
    {
        glPopMatrix();
    }

    CATCH_GL_ERROR

    return res;
}
