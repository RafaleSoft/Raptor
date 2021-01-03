// Object3DInstance.cpp: implementation of the CObject3DInstance class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "Object3DInstance.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLRenderingProperties.h"
#endif
#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
    #include "Subsys/Object3DContainerNotifier.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif


#include <math.h>

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CObject3DInstance::CObject3DInstanceClassID instanceId;
static CPersistentObjectType<CObject3DInstance> instanceFactory(instanceId);
const CPersistence::CPersistenceClassID& CObject3DInstance::CObject3DInstanceClassID::GetClassId(void)
{
	return instanceId;
}

CObject3DInstance::CObject3DInstance(CObject3D * const instance,const std::string& name)
	:CObject3D(instanceId,name),
	m_pReference(instance), m_pOverride(NULL), m_pObserver(NULL)
{
	if (instance == this)
		m_pReference = NULL;

    m_pObserver = new CObject3DContainerNotifier<CObject3DInstance,CObject3D*>(*this,&CObject3DInstance::notifyFromReference);

	// instance shouldn'tbe NULL
	if (m_pReference == NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DInstance::CObject3DInstanceClassID::GetClassId(),
		                                                CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
#endif
	}
    else
    {
        CObjectProperties &p = getProperties();
        p = instance->getProperties();

        m_pReference->registerDestruction(this);
        m_pReference->addContainerNotifier(m_pObserver);
    }

	resetTransform();
}


CObject3DInstance::CObject3DInstance(const CPersistence::CPersistenceClassID & classID,const std::string& name)
	:CObject3D(classID, name), m_pOverride(NULL)
{
	IDENT_MATRIX(m_transform);

	m_pReference = NULL;

    GL_COORD_VERTEX nullPosition(0,0,0,1);

	setBoundingBox(nullPosition,nullPosition);

    m_pObserver = new CObject3DContainerNotifier<CObject3DInstance,CObject3D*>(*this,&CObject3DInstance::notifyFromReference);
}


CObject3DInstance::~CObject3DInstance()
{
    if (m_pReference != NULL)
    {
        m_pReference->unregisterDestruction(this);
        m_pReference->removeContainerNotifier(m_pObserver);
    }
    if (m_pOverride != NULL)
        delete m_pOverride;
    delete m_pObserver;
}

void CObject3DInstance::unLink(const CPersistence* obj)
{
    if (obj == static_cast<CPersistence*>(m_pReference))
    {
        m_pReference = NULL;
        GL_COORD_VERTEX nullPosition(0,0,0,1);
	    setBoundingBox(nullPosition,nullPosition);
    }
	else
		CObject3D::unLink(obj);
}

void CObject3DInstance::notifyFromReference(CObject3D* reference)
{
    if ((m_pReference == reference) && 
        (m_pObserver->getEvent() == CContainerNotifier<CObject3D*>::UPDATEBBOX))
    {
        GL_COORD_VERTEX min;
		GL_COORD_VERTEX max;

		getBoundingBox(min,max);
		setBoundingBox(min,max);
    }
}

vector<CObject3DContour*> CObject3DInstance::createContours(void)
{
    if (m_pReference != NULL)
        return m_pReference->createContours();
    else
    {
        vector<CObject3DContour*>  res;
        return res;
    }
}

std::vector<CShader*> CObject3DInstance::getShaders(void)
{
	std::vector<CShader*> list;
	if (m_pReference != NULL)
		list = m_pReference->getShaders();

	return list;
}

void CObject3DInstance::overrideShading(const IRenderingProperties& override)
{
    if (m_pOverride ==  NULL)
		m_pOverride = new COpenGLRenderingProperties(override);
    else
        *m_pOverride = override;

    //  prevent buffer clear
    m_pOverride ->clear(0);
}

void CObject3DInstance::instantiate(CObject3D * const instance)
{
    if (m_pReference != NULL)
    {
            m_pReference->unregisterDestruction(this);
            m_pReference->removeContainerNotifier(m_pObserver);
    }

	if (instance == NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	CObject3DInstance::CObject3DInstanceClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_NULL_OBJECT);
#endif
		m_pReference = NULL;
		GL_COORD_VERTEX nullPosition(0,0,0,1);
	    setBoundingBox(nullPosition,nullPosition);
	}
	else
	{
		m_pReference = instance;

        CObjectProperties &p = getProperties();
        p = instance->getProperties();

		GL_COORD_VERTEX min;
		GL_COORD_VERTEX max;

		getBoundingBox(min,max);
		setBoundingBox(min,max);

        m_pReference->registerDestruction(this);
        m_pReference->addContainerNotifier(m_pObserver);
	}
}


void CObject3DInstance::resetTransform(void)
{
	IDENT_MATRIX(m_transform);

	// instance shouldn'tbe NULL
	if (m_pReference == NULL)
	{
		GL_COORD_VERTEX nullPosition(0,0,0,1);
	    setBoundingBox(nullPosition,nullPosition);
	}
	else
	{
        GL_COORD_VERTEX Min;
		GL_COORD_VERTEX Max;
        m_pReference->getBoundingBox(Min,Max);
		setBoundingBox(Min,Max);
	}
}


void CObject3DInstance::getTransform(CGenericMatrix<float> &T) const
{
	C3DEngineMatrix transformTranspose(m_transform);
	T = transformTranspose.Transpose();
}


void CObject3DInstance::getBoundingBox(GL_COORD_VERTEX &min,GL_COORD_VERTEX &max) const
{
	if ( m_pReference != NULL)
		m_pReference->boundingBox()->get(min, max, m_transform);
	else
	{
		min.x = min.y = min.z = 0.0f;
		max.x = max.y = max.z = 0.0f;
		min.h = max.h = 0.0f;
	}
}

bool RAPTOR_FASTCALL CObject3DInstance::fullClip(const CGenericMatrix<float> &transform)
{
    if (m_pReference == NULL)
        return CObject3D::fullClip(transform);
    else
    {
        CGenericMatrix<float> boxTransform = transform;
        earlyClip = C3DEngine::Get3DEngine()->glClip(boundingBox(),false,&boxTransform);
	    C3DEngineMatrix transformTranspose(m_transform);
		boxTransform *= transformTranspose.Transpose();
        m_pReference->fullClip(boxTransform);
    }
    return (earlyClip != C3DEngine::CLIP_UNKNOWN);
}

void CObject3DInstance::glRenderBBox(RENDER_BOX_MODEL filled)
{
	if (!properties.isVisible())
		return;
    
    GLenum mode = GL_LINE;
	if (filled)
		mode = GL_FILL;

    if (m_pReference != NULL)
	{
		glPushMatrix();

		glMultMatrixf(m_transform);

		m_pReference->glRenderBBox(filled);

		glPopMatrix();
	}

	CATCH_GL_ERROR
}


void CObject3DInstance::glRender()
{
	if (!properties.isVisible())
		return;

    if (m_pOverride != NULL)
        m_pOverride->glPushProperties();

	if (m_pReference != NULL)
	{
		glPushMatrix();
		glMultMatrixf(m_transform);
		m_pReference->glRender();
		glPopMatrix();
	}

    if (m_pOverride != NULL)
        m_pOverride->glPopProperties();

	CATCH_GL_ERROR
}


void CObject3DInstance::glClipRender()
{
	if (!properties.isVisible())
		return;

    if (isEarlyClipEnabled())
    {
        if (earlyClip == C3DEngine::CLIP_FULL)
            return;
    }

    if (m_pOverride != NULL)
        m_pOverride->glPushProperties();

	if (m_pReference != NULL)
	{
		glPushMatrix();
		glMultMatrixf(m_transform);
		m_pReference->glClipRender();
		glPopMatrix();
	}

    if (m_pOverride != NULL)
        m_pOverride->glPopProperties();

	CATCH_GL_ERROR
}


void CObject3DInstance::translate(float alpha,float beta,float gamma)
{
	m_transform.rowh.x+=alpha;
	m_transform.rowh.y+=beta;
	m_transform.rowh.z+=gamma;

	translateCenter(alpha,beta,gamma);
}

void CObject3DInstance::translateAbsolute(float alpha,float beta,float gamma)
{
	translateCenter(alpha - m_transform.rowh.x,
					beta - m_transform.rowh.y,
					gamma - m_transform.rowh.z);

	m_transform.rowh.x=alpha;
	m_transform.rowh.y=beta;
	m_transform.rowh.z=gamma;
}

void CObject3DInstance::scale(float alpha,float beta,float gamma)
{
	m_transform.rowx.x *= alpha;
	m_transform.rowy.y *= beta;
	m_transform.rowz.z *= gamma;

    scaleBoundingBox(alpha,beta,gamma);
}

void CObject3DInstance::transform(GL_MATRIX &m)
{
	CGenericMatrix<float> T;
	CGenericMatrix<float> TT;

	T = m_transform;
	TT = m;
	T *= TT.Transpose();
	C3DEngine::Generic_to_MATRIX(m_transform,T);

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotationX(float rx)
{
	float u = (float)cos(TO_RADIAN(rx));
	float v = (float)sin(TO_RADIAN(rx));
	//					*	m_transform
	//	1	0		0	
	//	0	cos		-sin
	//	0	sin		cos

	float aux = u*m_transform.rowy.x - v*m_transform.rowz.x;
	m_transform.rowz.x = v*m_transform.rowy.x + u*m_transform.rowz.x;
	m_transform.rowy.x = aux;

	aux = u*m_transform.rowy.y - v*m_transform.rowz.y;
	m_transform.rowz.y = v*m_transform.rowy.y + u*m_transform.rowz.y;
	m_transform.rowy.y = aux;

	aux = u*m_transform.rowy.z - v*m_transform.rowz.z;
	m_transform.rowz.z = v*m_transform.rowy.z + u*m_transform.rowz.z;
	m_transform.rowy.z = aux;

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotationY(float ry) 
{
	float u = (float)cos(TO_RADIAN(ry));
	float v = (float)sin(TO_RADIAN(ry));
	//					*	m_transform
	//	cos		0	sin	
	//	0		1	0
	//	-sin		0	cos

	float aux = u*m_transform.rowx.x + v*m_transform.rowz.x;
	m_transform.rowz.x = -v*m_transform.rowx.x + u*m_transform.rowz.x;
	m_transform.rowx.x = aux;

	aux = u*m_transform.rowx.y + v*m_transform.rowz.y;
	m_transform.rowz.y = -v*m_transform.rowx.y + u*m_transform.rowz.y;
	m_transform.rowx.y = aux;

	aux = u*m_transform.rowx.z + v*m_transform.rowz.z;
	m_transform.rowz.z = -v*m_transform.rowx.z + u*m_transform.rowz.z;
	m_transform.rowx.z = aux;

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotationZ(float rz) 
{
	float u = (float)cos(TO_RADIAN(rz));
	float v = (float)sin(TO_RADIAN(rz));
	//					*	m_transform
	// cos	-sin	0	
	// sin	cos		0
	//	0	0		1

	float aux = u*m_transform.rowx.x - v*m_transform.rowy.x;
	m_transform.rowy.x = v*m_transform.rowx.x + u*m_transform.rowy.x;
	m_transform.rowx.x = aux;

	aux = u*m_transform.rowx.y - v*m_transform.rowy.y;
	m_transform.rowy.y = v*m_transform.rowx.y + u*m_transform.rowy.y;
	m_transform.rowx.y = aux;

	aux = u*m_transform.rowx.z - v*m_transform.rowy.z;
	m_transform.rowy.z = v*m_transform.rowx.z + u*m_transform.rowy.z;
	m_transform.rowx.z = aux;

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotation(double angle,float ax,float ay,float az)
{
	CGenericMatrix<float> temp;
	float tx,ty,tz;

	CGenericMatrix<float> T;
	T = m_transform;
	T.Transpose();

	CGenericVector<float> axis(ax,ay,az,1.0f);
	C3DEngine::Get3DEngine()->generateRotation(angle,axis,temp);

	// save translation before it is cleared
	//	to compute rotation
	tx = T[3];	T[3] = 0;
	ty = T[7];	T[7] = 0;
	tz = T[11];	T[11] = 0;

	temp *= T;

	temp[3] = tx;
	temp[7] = ty;
	temp[11] = tz;

	temp.Transpose();

	C3DEngine::Generic_to_MATRIX(m_transform,temp);

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}


void CObject3DInstance::rotationX(float rx,float cy,float cz) 
{
	CGenericMatrix<float> T;
	CGenericMatrix<float> M;


	// Compute new position
	CGenericVector<float> t(m_transform.rowh.x,m_transform.rowh.y - cy,m_transform.rowh.z - cz,1);

	float u = (float)cos(TO_RADIAN(rx));
	float v = (float)sin(TO_RADIAN(rx));
	float aux = t.Y();

	t.Y() = aux * u - t.Z() * v + cy;
	t.Z() = aux * v + t.Z() * u + cz;

	T = m_transform;
	T.Transpose();
	T[3] = 0;
	T[7] = 0;
	T[11] = 0;

	M.Ident();
	M[5] = u;
	M[6] = -v;
	M[9] = v;
	M[10] = u;

	T *= M;

	T[3] = t.X();
	T[7] = t.Y();
	T[11] = t.Z();

	T.Transpose();
	C3DEngine::Generic_to_MATRIX(m_transform,T);

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotationY(float ry,float cx,float cz) 
{
	CGenericMatrix<float> T;
	CGenericMatrix<float> M;


	// Compute new position
	CGenericVector<float> t(m_transform.rowh.x - cx,m_transform.rowh.y,m_transform.rowh.z - cz,1);

	float u = (float)cos(TO_RADIAN(ry));
	float v = (float)sin(TO_RADIAN(ry));
	float aux = t.X();

	t.X() = aux * u - t.Z() * v + cx;
	t.Z() = aux * v + t.Z() * u + cz;

	T = m_transform;
	T.Transpose();
	T[3] = 0;
	T[7] = 0;
	T[11] = 0;

	M.Ident();
	M[0] = u;
	M[2] = -v;
	M[8] = v;
	M[10] = u;

	T *= M;

	T[3] = t.X();
	T[7] = t.Y();
	T[11] = t.Z();

	T.Transpose();
	C3DEngine::Generic_to_MATRIX(m_transform,T);

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}

void CObject3DInstance::rotationZ(float rz,float cx,float cy) 
{
	CGenericMatrix<float> T;
	CGenericMatrix<float> M;


	// Compute new position
	CGenericVector<float> t(m_transform.rowh.x - cx,m_transform.rowh.y - cy,m_transform.rowh.z,1);

	float u = (float)cos(TO_RADIAN(rz));
	float v = (float)sin(TO_RADIAN(rz));
	float aux = t.X();

	t.X() = aux * u - t.Y() * v + cx;
	t.Y() = aux * v + t.Y() * u + cy;

	T = m_transform;
	T.Transpose();
	T[3] = 0;
	T[7] = 0;
	T[11] = 0;

	M.Ident();
	M[0] = u;
	M[1] = -v;
	M[4] = v;
	M[5] = u;

	T *= M;

	T[3] = t.X();
	T[7] = t.Y();
	T[11] = t.Z();

	T.Transpose();
	C3DEngine::Generic_to_MATRIX(m_transform,T);

	GL_COORD_VERTEX min;
	GL_COORD_VERTEX max;

	getBoundingBox(min,max);
	setBoundingBox(min,max);
}


bool CObject3DInstance::exportObject(CRaptorIO& o)
{
    CPersistence::exportObject(o);

	o << m_transform;

	return true;
}

bool CObject3DInstance::importObject(CRaptorIO& io)
{
   string name;
	io >> name;

	string data = io.getValueName();
	while (io.hasMoreValues())
	{
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Object3D")
            CObject3D::importObject(io);
        else if (data == "reference")
        {
            io >> name;
			CPersistence* object = CPersistence::FindObject(name);
            if (object != NULL)
            {
				const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
				instantiate(op);
            }
        }
        else if(data == "RenderingProperties")
        {
			COpenGLRenderingProperties props;
            props.importObject(io);
            overrideShading(props);
        }
		else if (data == "Transform")
		{
			GL_MATRIX m;
			io >> m;
			transform(m);
		}
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	return true;
}



