/***************************************************************************/
/*                                                                         */
/*  3DEngine.cpp                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"


#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DEngine.h"
#endif
#if !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Physics.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif
#ifdef RAPTOR_SSE_CODE_GENERATION
	#if !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)
		#include "SSE_Engine/SSE_3DEngine.h"
	#endif
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "IViewPoint.h"
#endif

//	Collision core
#include <math.h>
#include "Subsys/Collisions.h"
#include "Subsys/3DEngineAttributes.h"


RAPTOR_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////
// Const and statics
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4711)	// automatic inline expansion warning

CCollisionStatus		C3DEngineAttributes::currentStatus;
int						C3DEngineAttributes::m_iNbCollisions = 0;


//	bbox clipping indexes
static int indexes[24] = 
{
	0 , 1,
	1 , 2,
	2 , 3,
	3 , 0,
	4 , 5,
	5 , 6,
	6 , 7,
	7 , 4,
	0 , 4,
	1 , 5,
	2 , 6,
	3 , 7
};

static C3DEngine::C3DEngineClassID engineId;
const CPersistence::CPersistenceClassID& C3DEngine::C3DEngineClassID::GetClassId(void)
{
	return engineId;
}


RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DEngine::C3DEngine(const std::string& name)
	:CPersistence(engineId,name)
{
	m_pAttributes = new C3DEngineAttributes();

	m_pAttributes->farPlane = 100.0f;
	m_pAttributes->viewport[0] = 0;
	m_pAttributes->viewport[1] = 0;
	m_pAttributes->viewport[2] = 0;
	m_pAttributes->viewport[3] = 0;

	IDENT_MATRIX(m_pAttributes->currentModelview);

	m_camera = new CBoundingBox();
	
	m_camera->set(-1.0,-1.0,-1.0,1.0,1.0,1.0);
	m_collision = new CCubeBTree();
	m_collision->m_engine = this;

	IDENT_MATRIX(m_collision->transform);
}


C3DEngine::~C3DEngine()
{
	delete m_camera;
	delete m_collision;

	if (C3DEngine::Get3DEngine() == this)
		C3DEngine::Set3DEngine(NULL);

	delete m_pAttributes;
}


void C3DEngine::Set3DEngine(C3DEngine *Engine)
{
	CRaptorInstance::GetInstance().p3DEngine = Engine;
}


C3DEngine *C3DEngine::Get3DEngine(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (instance.p3DEngine == NULL)
	{
#ifdef RAPTOR_SSE_CODE_GENERATION
		const CPU_INFO &info = getCPUINFO();
		if (info.SSE)
			instance.p3DEngine = new CSSE_3DEngine;
		else
			instance.p3DEngine = new C3DEngine;
#else
		status.p3DEngine = new C3DEngine;
#endif
	}
	return instance.p3DEngine;
}


void C3DEngine::setCameraBBox(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax)
{
	m_camera->set(xmin,ymin,zmin,xmax,ymax,zmax);
}

int C3DEngine::GetNbCollisions() 
{ 
	return C3DEngineAttributes::m_iNbCollisions; 
}

//////////////////////////////////////////////////////////////////////
// Geometry engine processing
//////////////////////////////////////////////////////////////////////
void C3DEngine::TriangleNormal(GL_COORD_VERTEX& v1,GL_COORD_VERTEX& v2,GL_COORD_VERTEX& v3,GL_COORD_VERTEX& dest)
{
	float vect1[3],vect2[3],vect3[3];

	vect1[0] = v2.x-v1.x;
	vect1[1] = v2.y-v1.y;
	vect1[2] = v2.z-v1.z;

	vect2[0] = v3.x-v2.x;
	vect2[1] = v3.y-v2.y;
	vect2[2] = v3.z-v2.z;

	//	cross product
	vect3[0] = vect1[1] * vect2[2] - vect2[1] * vect1[2];
	vect3[1] = vect1[2] * vect2[0] - vect2[2] * vect1[0];
	vect3[2] = vect1[0] * vect2[1] - vect2[0] * vect1[1];

	float n = (float)sqrt(vect3[0]*vect3[0]+vect3[1]*vect3[1]+vect3[2]*vect3[2]);
	float sq = 1.0f;
	if (n != 0)
		sq /= n;
	
	
	dest.x = vect3[0] * sq;
	dest.y = vect3[1] * sq;
	dest.z = vect3[2] * sq;
}


void C3DEngine::generateRotation(double angle,const CGenericVector<float> &axis,CGenericMatrix<float> &matrix) const
{
	float u = (float)cos(TO_RADIAN(angle));
	float v = (float)sin(TO_RADIAN(angle));

	CGenericMatrix<float> uu;

	uu.Ident();
	matrix.Ident();

	uu[0] = axis.X() * axis.X();
	uu[1] = uu[4] = axis.X() * axis.Y();
	uu[2] = uu[8] = axis.X() * axis.Z();
	uu[5] = axis.Y() * axis.Y();
	uu[6] = uu[9] = axis.Y() * axis.Z();
	uu[10] = axis.Z() * axis.Z();

	matrix -=  uu;
	matrix *= u;

	matrix += uu;
	
	uu[0] = uu[5] = uu[10] = 0;
	uu[4] = axis.Z();
	uu[1] = -axis.Z();
	uu[8] = -axis.Y();
	uu[2] = axis.Y();
	uu[9] = axis.X();
	uu[6] = -axis.X();

	uu *= v;
	uu[15] = 0.0f;

	//	matrix = axis.t(axis) + (I - axis.t(axis)).Cos(angle) + S.Sin(angle)
	//	where : 
	//	- t() denotes the transpose
	//	- S = 0  -az  ay
	//        az  0  -ax
	//        -ay ax  0
	matrix += uu;
}

void C3DEngine::solve(const CGenericVector<float> &b,const CGenericMatrix<float> &A,CGenericVector<float> &x) const
{
	float *M = A.matrix();
/*
	//works faster, but can't handle singularities

	float K1 = M[4] * M[1] - M[0] * M[5];
	float K2 = M[4] * M[2] - M[0] * M[6];
	float K3 = M[4] * b.X() - M[0] * b.Y();

	float K4 = M[8] * M[5] - M[4] * M[9];
	float K5 = M[8] * M[6] - M[4] * M[10];
	float K6 = M[8] * b.Y() - M[4] * b.Z();

	x[2] = (K3*K4 - K1*K6) / (K2*K4 - K1*K5);
	x[1] = (K3 - K2*x[2]) / K1;
	x[0] = b.X() - M[2]*x[2] - M[1]*x[1];
*/
	float det = M[0]*M[5]*M[10] + M[1]*M[6]*M[8] + M[2]*M[4]*M[9]
				-M[2]*M[5]*M[8] - M[1]*M[4]*M[10] - M[0]*M[6]*M[9];

	if (det != 0)
	{
		det = -1.0f / det;

		float K0 = M[5]*M[10] - M[6]*M[9];
		float K1 = -(M[4]*M[10] - M[6]*M[8]);
		float K2 = M[4]*M[9] - M[5]*M[8];

		float K3 = -(M[1]*M[10] - M[2]*M[9]);
		float K4 = M[0]*M[10] - M[2]*M[8];
		float K5 = -(M[0]*M[9] - M[1]*M[8]);

		float K6 = M[1]*M[6] - M[2]*M[5];
		float K7 = -(M[0]*M[6] - M[2]*M[4]);
		float K8 = M[0]*M[5] - M[1]*M[4];

		x[0] = (b.X()*K0 + b.Y()*K3 + b.Z()*K6) * det;

		x[1] = (b.X()*K1 + b.Y()*K4 + b.Z()*K7) * det;

		x[2] = (b.X()*K2 + b.Y()*K5 + b.Z()*K8) * det;
	}
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(	C3DEngine::C3DEngineClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Raptor 3DEngine cannot solve: no solutions");
#endif
		x.Set(0.0f,0.0f,0.0f,1.0f);
	}
}



//////////////////////////////////////////////////////////////////////
// Engine visibility processing
//////////////////////////////////////////////////////////////////////

void C3DEngine::pushProjectionMatrix(void) const
{
	m_pAttributes->projection_stack.push_back(m_pAttributes->projection);
}

void C3DEngine::popProjectionMatrix(void) const
{
	if (m_pAttributes->projection_stack.size() > 0)
	{
		m_pAttributes->projection = m_pAttributes->projection_stack.back();
		m_pAttributes->projection_stack.pop_back();
	}
}

void C3DEngine::pushModelviewMatrix(void) const
{
	m_pAttributes->modelview_stack.push_back(m_pAttributes->modelview);
}

void C3DEngine::popModelviewMatrix(void) const
{
	if (m_pAttributes->modelview_stack.size() > 0)
	{
		m_pAttributes->modelview = m_pAttributes->modelview_stack.back();
		m_pAttributes->modelview_stack.pop_back();
	}
}

void C3DEngine::setClip(int xmin, int ymin, int xmax, int ymax)
{
	m_pAttributes->viewport[0] = xmin;
	m_pAttributes->viewport[1] = ymin;
	m_pAttributes->viewport[2] = xmax;
	m_pAttributes->viewport[3] = ymax;
}

void C3DEngine::setProjection(const CGenericMatrix<float> &projection)
{
    m_pAttributes->projection = projection;
}

void C3DEngine::setClipPlanes(vector<GL_COORD_VERTEX> &planes) const
{
	m_pAttributes->clippingPlanes.clear();
	for (size_t i=0;i<planes.size();i++)
	{
		CGenericVector<float> V(planes[i].x,planes[i].y,planes[i].z,1.0f);
		V.Normalize();
		V.H(planes[i].h);
		m_pAttributes->clippingPlanes.push_back(V);
	}
}

void C3DEngine::setFarPlane(float farPlane) const
{
	m_pAttributes->farPlane = farPlane;
}

float C3DEngine::getFarPlane(void) const
{
    return m_pAttributes->farPlane;
}

CGenericMatrix<float> &C3DEngine::getProjection(void) const
{
    return m_pAttributes->projection;
}

void C3DEngine::setModelview(const CGenericMatrix<float> &modelview)
{
    m_pAttributes->modelview = modelview;
}

CGenericMatrix<float> &C3DEngine::getModelview(void) const
{
    return m_pAttributes->modelview;
}

void C3DEngine::glConfigureEngine(IViewPoint* pViewPoint)
{
	CGenericMatrix<float> M;
	CGenericMatrix<float> P;

	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
	glGetTransposeFloatv(GL_PROJECTION_MATRIX,P);

    m_pAttributes->modelview = M;
	m_pAttributes->projection = P;

	// Far plane with a perspective frustum: this cannot handle 
	//	any situation, so do not compute it, let the view volume 
	//	set the far plane.
	//float C = projection[10];
    //float D = projection[11];
    //float n = 0.5f*D * ((C+1)/(C-1) - 1);
    //m_pAttributes->farPlane = n*D/(n+n+D);
	if (pViewPoint != NULL)
	{
		float n,f,l,r,t,b;
		IViewPoint::VIEW_POINT_MODEL m;
		pViewPoint->getViewVolume(l,r,b,t,n,f,m);

		m_pAttributes->farPlane = f;

		vector<GL_COORD_VERTEX>	clipPlanes;
		GL_COORD_VERTEX farPlane(0.0f,0.0f,-1.0f,f);
		GL_COORD_VERTEX nearPlane(0.0f,0.0f,1.0f,-n);

		GL_COORD_VERTEX leftPlane(n*(b - t), 0.0f, l*(b - t), 0.0f);
		GL_COORD_VERTEX rightPlane(n*(t - b), 0.0f, r*(t - b), 0.0f);

		GL_COORD_VERTEX topPlane(0.0f, n*(r - l), t*(r - l), 0.0f);
		GL_COORD_VERTEX bottomPlane(0.0f, n*(l - r), b*(l - r), 0.0f);

		clipPlanes.push_back(farPlane);
		clipPlanes.push_back(nearPlane);
		clipPlanes.push_back(leftPlane);
		clipPlanes.push_back(rightPlane);
		clipPlanes.push_back(topPlane);
		clipPlanes.push_back(bottomPlane);
		setClipPlanes(clipPlanes);
	}


	CATCH_GL_ERROR
}

void C3DEngine::glProject(GL_COORD_VERTEX &v) const
{
	CGenericMatrix<float> P;
	CGenericMatrix<float> M;

	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
	glGetTransposeFloatv(GL_PROJECTION_MATRIX,P);
	GLint *viewport = getAttributes()->viewport;
	
	P *= M;
	CGenericVector<float> V(v.x,v.y,v.z,v.h);
	V *= P;
	v.x = ((V.X() / V.H()) + 1) / 2 * viewport[2];
	v.y = ((V.Y() / V.H()) + 1) / 2 * viewport[3];
	v.z = (V.Z() / V.H());
	v.h = 1.0f;

    CATCH_GL_ERROR
}


void C3DEngine::initVisibilitySet(CGenericMatrix<float> *projection)
{
	if (projection == NULL)
	{
		CGenericMatrix<float> P;
		CGenericMatrix<float> M;

		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
		glGetTransposeFloatv(GL_PROJECTION_MATRIX,m_pAttributes->visibilityProjection);
	
		m_pAttributes->visibilityProjection *= M;
		Generic_to_MATRIX(m_pAttributes->currentModelview,M);
	}
	else
        m_pAttributes->visibilityProjection = *projection;
}


//	Calcul de l'intersection par la methode de l'englobant :
//	- on parcourt les points de B1 pour former la courbe englobante
//	- a chaque nouveau point de construction, on regarde si des points
//	de B2 sont dans l'englobant de B1. On les elimine au fur-et-a-mesure. 
//	- on s'arrete des que l'on sait qu'aucun point de B2 n'est dans l'englobant
//	( car on procede par elimination )
bool Intersect(C3DEngineAttributes::visibleZone &B1,C3DEngineAttributes::visibleZone &B2)
{
	bool res = false;

	CGenericVector<float> tmp;

	//	Inutile de tout parcourir, on cherche le point 
	//	minimum pour demarrer.
	for (int j=1;j<8;j++)
	{
		if (B1.tCoords[0].Y() > B1.tCoords[j].Y())
		{
			tmp = B1.tCoords[0];
			B1.tCoords[0] = B1.tCoords[j];
			B1.tCoords[j] = tmp;
		}
	}

	int pointIndex[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 0 };
	//int pointsCandidats[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	float last_dx = 1.0f;
	float last_dy = 0.0f;
	float last_last_dx = 1.0f;
	float last_last_dy = 0.0f;
	float last_norm = 1.0f;
	float last_last_norm = 1.0f;

    int i=0;
	for (i=0;i<7;i++)
	{
		// recherche du meilleur point de B1:
		// - on tourne dans le sens CCW,
		// - on recherche le projeté le + à droite sur le segment precedent.
		int best = 1;
		float best_d = -FLT_MAX;

		for (int j=i+1;j<9;j++)
		{
			float dx = B1.tCoords[pointIndex[j]].X() - B1.tCoords[pointIndex[i]].X();
			float dy = B1.tCoords[pointIndex[j]].Y() - B1.tCoords[pointIndex[i]].Y();

			float scal = dx*last_dx + dy*last_dy;
			float norm = sqrt(dx*dx + dy*dy);
			scal = scal / (last_norm * norm);
			if (scal > best_d)
			{
				best = j;
				best_d = scal;
				last_last_dx = dx;
				last_last_dy = dy;
				last_last_norm = norm;
			}
		}

		// on a trouve le meilleur : 
		// - on le memorise
		// - on filtre les points de B2
		int idx = pointIndex[i+1];
		pointIndex[i+1] = pointIndex[best];
		pointIndex[best] = idx;
/*
		for (j=0;j<8;j++)
		{
			if (pointsCandidats[j] >= 0)
			{
				float dx = B2.tCoords[j].X() - B1.tCoords[i].X();
				float dy = B2.tCoords[j].Y() - B1.tCoords[i].Y();

				float scal = dx*last_dx + dy*last_dy;
				if (scal > best_d)
				{
					pointsCandidats[j] = -1;
				}
			}
		}
*/
		last_dx = last_last_dx;
		last_dy = last_last_dy;
		last_norm = last_last_norm;
	}

	glBegin(GL_LINE_STRIP);
	for (i=0;i<8;i++)
	{
		CGenericVector<float> &P = B1.tCoords[pointIndex[i]];
		glVertex3f(P.X(),P.Y(),-5.0f);
		if ((i>0) && (pointIndex[i] ==0))
			break;
	}
	glEnd();

	return res;
}

void C3DEngine::addToVisibleSet(const CBoundingBox *bbox,CGenericMatrix<float> &boxTransform)
{
	C3DEngineAttributes::visibleZone	zone;
	zone.pBbox = bbox;

	CGenericMatrix<float> currentTransform;
	CGenericMatrix<float> visibleTransform;

	currentTransform = m_pAttributes->currentModelview;
    visibleTransform = m_pAttributes->visibilityProjection;
	currentTransform *= boxTransform;
	visibleTransform *= boxTransform;

	GL_MATRIX currentModelview;
	GL_MATRIX currentProjection;
	Generic_to_MATRIX(currentModelview,currentTransform);
	Generic_to_MATRIX(currentProjection,visibleTransform);

	bbox->getP(zone.minZone,zone.maxZone,currentProjection,currentModelview);

	// valid projected bbox ?
	if ((zone.maxZone.x <= zone.minZone.x) ||
		(zone.maxZone.y <= zone.minZone.y))
		return;
	if ((zone.minZone.x > 1.0f) || (zone.maxZone.x < -1.0f) ||
		(zone.minZone.y > 1.0f) || (zone.maxZone.y < -1.0f))
		return;
	if (zone.maxZone.z >= 0)
		return;

	CGenericVector<float> p;
	//	Optimiser en ne calculant que les coordonnees necessaires
	p.Set(bbox->xMin(),bbox->yMin(),bbox->zMin(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[0] = p;
	p.Set(bbox->xMax(),bbox->yMin(),bbox->zMin(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[1] = p;
	p.Set(bbox->xMin(),bbox->yMax(),bbox->zMin(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[2] = p;
	p.Set(bbox->xMax(),bbox->yMax(),bbox->zMin(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[3] = p;
	p.Set(bbox->xMin(),bbox->yMin(),bbox->zMax(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[4] = p;
	p.Set(bbox->xMax(),bbox->yMin(),bbox->zMax(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[5] = p;
	p.Set(bbox->xMin(),bbox->yMax(),bbox->zMax(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[6] = p;
	p.Set(bbox->xMax(),bbox->yMax(),bbox->zMax(),1.0f);
	p *= visibleTransform;
	p.X() /= p.H(); p.Y() /= p.H();
	zone.tCoords[7] = p;

	if (m_pAttributes->visibility.size() == 0)
	{
		m_pAttributes->visibility.push_back(zone);
		return;
	}

	m_pAttributes->visibilityResult.clear();
	vector<C3DEngineAttributes::visibleZone> newZones;
	newZones.push_back(zone);

	while (newZones.size() > 0)
	{
		C3DEngineAttributes::visibleZone P = newZones.back();
		newZones.pop_back();

		if (m_pAttributes->visibility.size() == 0)
			m_pAttributes->visibilityResult.push_back(P);
		else 
		{
			while (m_pAttributes->visibility.size() > 0)
			{
				C3DEngineAttributes::visibleZone M = m_pAttributes->visibility.back();
				m_pAttributes->visibility.pop_back();

				// intersection ?
				if ((P.minZone.x < M.maxZone.x) && (M.minZone.x < P.maxZone.x) &&
					(P.minZone.y < M.maxZone.y) && (M.minZone.y < P.maxZone.y))
				{
					if (P.maxZone.z > M.maxZone.z)
					{
						// compute visibility of existing oldZone M by splitting:
						// - first horizontal subdivide: up
						if ((P.minZone.y > M.minZone.y) && (P.minZone.y < M.maxZone.y))
						{
							C3DEngineAttributes::visibleZone newZone = M;
							newZone.maxZone.y = P.minZone.y;
							m_pAttributes->visibilityResult.push_back(newZone);
							M.minZone.y = P.minZone.y;
						}
						// - second horizontal subdivide: down
						if ((P.maxZone.y > M.minZone.y) && (P.maxZone.y < M.maxZone.y))
						{
							C3DEngineAttributes::visibleZone newZone = M;
							newZone.minZone.y = P.maxZone.y;
							m_pAttributes->visibilityResult.push_back(newZone);
							M.maxZone.y = P.maxZone.y;
						}
						// - third vertical subdivide : left
						if ((P.minZone.x > M.minZone.x) && (P.minZone.x < M.maxZone.x))
						{
							C3DEngineAttributes::visibleZone newZone = M;
							newZone.maxZone.x = P.minZone.x;
							m_pAttributes->visibilityResult.push_back(newZone);
							M.minZone.x = P.minZone.x;
						}
						// - fourth vertical subdivide : right
						if ((P.maxZone.x > M.minZone.x) && (P.maxZone.x < M.maxZone.x))
						{
							C3DEngineAttributes::visibleZone newZone = M;
							newZone.minZone.x = P.maxZone.x;
							m_pAttributes->visibilityResult.push_back(newZone);
							M.maxZone.x = P.maxZone.x;
						}
						// At this point, the remaining of M is covered by P,
						// we should remove M.
				//		if ((M.maxZone.x > M.minZone.x) || (M.maxZone.y > M.minZone.y))
				//		{
				//			bool res = Intersect(P,M);
				//		}
					}
					else
					{
						// M is visible, compute visibility of new zone P
						m_pAttributes->visibilityResult.push_back(M);
						
						// - first horizontal subdivide: up
						if ((M.minZone.y > P.minZone.y) && (M.minZone.y < P.maxZone.y))
						{
							C3DEngineAttributes::visibleZone newZone = P;
							newZone.maxZone.y = M.minZone.y;
							newZones.push_back(newZone);
							P.minZone.y = M.minZone.y;
						}
						// - second horizontal subdivide: down
						if ((M.maxZone.y > P.minZone.y) && (M.maxZone.y < P.maxZone.y))
						{
							C3DEngineAttributes::visibleZone newZone = P;
							newZone.minZone.y = M.maxZone.y;
							newZones.push_back(newZone);
							P.maxZone.y = M.maxZone.y;
						}
						// - third vertical subdivide : left
						if ((M.minZone.x > P.minZone.x) && (M.minZone.x < P.maxZone.x))
						{
							C3DEngineAttributes::visibleZone newZone = P;
							newZone.maxZone.x = M.minZone.x;
							newZones.push_back(newZone);
							P.minZone.x = M.minZone.x;
						}
						// - fourth vertical subdivide : right
						if ((M.maxZone.x > P.minZone.x) && (M.maxZone.x < P.maxZone.x))
						{
							C3DEngineAttributes::visibleZone newZone = P;
							newZone.minZone.x = M.maxZone.x;
							newZones.push_back(newZone);
							P.maxZone.x = M.maxZone.x;
						}
						// At this point, new zone P has been split up,
						// the remaining part is covered, proceed with
						// splits from the begining.
						P.maxZone.x = P.minZone.x;
						P.maxZone.y = P.minZone.y;
						break;
					}
				}
				// No intersection, the zone M if part of the visible result
				else
				{
					m_pAttributes->visibilityResult.push_back(M);
				}
			}
			//	If the new zone zone has not been reducted to nil,
			//	there is a remaining part to add to the result.
			if ((P.minZone.x < P.maxZone.x) && (P.minZone.y < P.maxZone.y))
				m_pAttributes->visibilityResult.push_back(P);
		}
	}

	//	Compute existing zones visibility
	while (m_pAttributes->visibilityResult.size() > 0)
	{
		m_pAttributes->visibility.push_back(m_pAttributes->visibilityResult.back());
		m_pAttributes->visibilityResult.pop_back();
	}
}

void C3DEngine::resolveVisibility(void)
{
	m_pAttributes->visibleBBox.clear();
	vector<C3DEngineAttributes::visibleZone>::iterator itr_vs = m_pAttributes->visibility.begin();
	while (itr_vs != m_pAttributes->visibility.end())
	{
		C3DEngineAttributes::visibleZone zone = (*itr_vs);
		itr_vs++;

		map<const CBoundingBox*,vector<C3DEngineAttributes::visibleZone> >::iterator itr = m_pAttributes->visibleBBox.find(zone.pBbox);
		if (itr != m_pAttributes->visibleBBox.end())
			(*itr).second.push_back(zone);
		else
		{
			vector<C3DEngineAttributes::visibleZone> v;
			v.push_back(zone);
			m_pAttributes->visibleBBox.insert(map<const CBoundingBox*,vector<C3DEngineAttributes::visibleZone> >::value_type(zone.pBbox,v));
		}
	}
}

bool C3DEngine::isVisible(const CBoundingBox* bbox)
{
	map<const CBoundingBox*,vector<C3DEngineAttributes::visibleZone> >::iterator itr = m_pAttributes->visibleBBox.find(bbox);
	if (itr != m_pAttributes->visibleBBox.end())
	{
		size_t nb = (*itr).second.size();
		return (nb > 0);
	}
	else
		return false;
}

C3DEngine::CLIP_RESULT C3DEngine::glClip(const CBoundingBox *const bbox,bool scanModelview) const
{
	float radius;
	GL_COORD_VERTEX center;

	bbox->getSphere(center,radius);

	CGenericMatrix<float> M;
	//	Check if the object is in front of the viewer
    if (scanModelview)
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
    else
        M = getAttributes()->modelview;

	//	Need to recompute the radius in case the modelview
	//	is scaled or transformed by something else than a translation or rotation.
	CGenericVector<float> C(center);
	CGenericVector<float> C2(center);
	C *= M;
	C2.Z() += radius;
	C2 *= M;
	C2 -= C;
	float newRadius = C2.Norm();

	C3DEngine::CLIP_RESULT res = C3DEngine::CLIP_NONE;
	size_t numPlane = 0;
	while (	(res != C3DEngine::CLIP_FULL) && 
			(numPlane < m_pAttributes->clippingPlanes.size()))
	{
		CGenericVector<float> &V = m_pAttributes->clippingPlanes[numPlane++];
		float dist = C % V;
		if (dist - newRadius > V.H())
			res = C3DEngine::CLIP_FULL;
		else if (dist + newRadius > V.H())
			res = C3DEngine::CLIP_PARTS;
	}

	return res;
}

C3DEngine::CLIP_RESULT C3DEngine::glClip(const CBoundingBox * const bbox,bool scanModelview,const CGenericMatrix<float> *transform) const
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	bbox->get(xmin,ymin,zmin,xmax,ymax,zmax);

	CGenericMatrix<float> M;
	float winz = -1;
		
	//	Check if the object is in front of the viewer
    if (scanModelview)
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,M);
    else
        M = getAttributes()->modelview;

    if (transform != NULL)
        M *= *transform;  
	
    winz =  MIN(xmin * M[8],xmax * M[8]) + 
                MIN(ymin * M[9],ymax * M[9]) +
                MIN(zmin * M[10],zmax * M[10]) + 
                M[11];
    if (winz > 0)
		return CLIP_FULL;     // no part of the box can be visible
    winz = MAX(xmin * M[8],xmax * M[8]) + 
               MAX(ymin * M[9],ymax * M[9]) +
               MAX(zmin * M[10],zmax * M[10]) + 
               M[11] ;
    if (winz < -getAttributes()->farPlane)
        return CLIP_FULL;

	CGenericMatrix<float> P = getAttributes()->projection;
    P *=  M;
	CGenericVector<float> V;
    

	//
	// project BBox and check if it intersects Viewport
	//
	CGenericVector<float> XMin(xmin*P[0],xmin*P[4],0,xmin*P[12]);
    CGenericVector<float> XMax(xmax*P[0],xmax*P[4],0,xmax*P[12]);

    CGenericVector<float> YMin(ymin*P[1],ymin*P[5],0,ymin*P[13]);
    CGenericVector<float> YMax(ymax*P[1],ymax*P[5],0,ymax*P[13]);

    CGenericVector<float> ZMin(zmin*P[2],zmin*P[6],0,zmin*P[14]);
    CGenericVector<float> ZMax(zmax*P[2],zmax*P[6],0,zmax*P[14]);

    CGenericVector<float> H(P[3],P[7],P[11],P[15]);
    float	winx[8];
	float	winy[8];

    V.H(XMin.H()+YMin.H()+ZMin.H()+H.H());
    V.X(XMin.X()+YMin.X()+ZMin.X()+H.X());
    V.Y(XMin.Y()+YMin.Y()+ZMin.Y()+H.Y());
    winx[0] = V.X() / V.H();
	winy[0] = V.Y() / V.H();

    V.H(XMin.H()+YMin.H()+ZMax.H()+H.H());
    V.X(XMin.X()+YMin.X()+ZMax.X()+H.X());
    V.Y(XMin.Y()+YMin.Y()+ZMax.Y()+H.Y());
    winx[1] = V.X() / V.H();
	winy[1] = V.Y() / V.H();

    V.H(XMax.H()+YMin.H()+ZMax.H()+H.H());
    V.X(XMax.X()+YMin.X()+ZMax.X()+H.X());
    V.Y(XMax.Y()+YMin.Y()+ZMax.Y()+H.Y());
    winx[2] = V.X() / V.H();
	winy[2] = V.Y() / V.H();

    V.H(XMax.H()+YMin.H()+ZMin.H()+H.H());
    V.X(XMax.X()+YMin.X()+ZMin.X()+H.X());
    V.Y(XMax.Y()+YMin.Y()+ZMin.Y()+H.Y());
    winx[3] = V.X() / V.H();
	winy[3] = V.Y() / V.H();

    V.H(XMin.H()+YMax.H()+ZMin.H()+H.H());
    V.X(XMin.X()+YMax.X()+ZMin.X()+H.X());
    V.Y(XMin.Y()+YMax.Y()+ZMin.Y()+H.Y());
    winx[4] = V.X() / V.H();
	winy[4] = V.Y() / V.H();

    V.H(XMin.H()+YMax.H()+ZMax.H()+H.H());
    V.X(XMin.X()+YMax.X()+ZMax.X()+H.X());
    V.Y(XMin.Y()+YMax.Y()+ZMax.Y()+H.Y());
    winx[5] = V.X() / V.H();
	winy[5] = V.Y() / V.H();

    V.H(XMax.H()+YMax.H()+ZMax.H()+H.H());
    V.X(XMax.X()+YMax.X()+ZMax.X()+H.X());
    V.Y(XMax.Y()+YMax.Y()+ZMax.Y()+H.Y());
    winx[6] = V.X() / V.H();
	winy[6] = V.Y() / V.H();

    V.H(XMax.H()+YMax.H()+ZMin.H()+H.H());
    V.X(XMax.X()+YMax.X()+ZMin.X()+H.X());
    V.Y(XMax.Y()+YMax.Y()+ZMin.Y()+H.Y());
    winx[7] = V.X() / V.H();
	winy[7] = V.Y() / V.H();

	//	clip the 12 segments of the BBox
	CLIP_RESULT intersect = CLIP_FULL;
    int nbIntersect = 0;
	int pos = 0;

	while ((pos < 24) && 
              ((pos == 2*nbIntersect) || (nbIntersect == 0)))
	{
		int idx0 = indexes[pos];
		int idx1 = indexes[pos+1];

		if (winx[idx1] < winx[idx0])
		{
			xmin = winx[idx1];
			xmax = winx[idx0];
		}
		else
		{
			xmin = winx[idx0];
			xmax = winx[idx1];
		}

		if (winy[idx1] < winy[idx0])
		{
			ymin = winy[idx1];
			ymax = winy[idx0];
		}
		else
		{
			ymin = winy[idx0];
			ymax = winy[idx1];
		}

		if ((-1.0f < xmax) && (1.0f > xmin))
		{
			if ((-1.0f < ymax) && (1.0f > ymin))
				nbIntersect++;
		}
		pos+=2;
	}

    if (nbIntersect == 12)
		intersect = C3DEngine::CLIP_NONE;
	else if (nbIntersect > 0)
		intersect = C3DEngine::CLIP_PARTS;
	
	CATCH_GL_ERROR

	return intersect;
}

void C3DEngine::normals(GL_COORD_VERTEX &v,GL_COORD_VERTEX &normal,GL_COORD_VERTEX &binormal)
{
	normal.y = 0.0f;
	normal.h = 1.0f;
	binormal.h = 1.0f;

	if ((v.x == 0) && (v.z == 0))
	{
		normal.x = 0.0f;
		normal.z = 1.0f;
	}
	else
	{
		float d2 = 1.0f / sqrt(v.z*v.z + v.x*v.x);
		normal.x = v.z * d2;
		normal.z = -v.x * d2;
	}

	binormal.x = (v.y*normal.z - v.z*normal.y);
	binormal.y = (v.z*normal.x - v.x*normal.z);
	binormal.z = (v.x*normal.y - v.y*normal.x);

	if (binormal.y < 0)
	{
		normal.x = -normal.x;
		normal.z = -normal.z;

		binormal.x = -binormal.x;
		binormal.y = -binormal.y;
		binormal.z = -binormal.z;
	}
}

void C3DEngine::normals(CGenericVector<float> &v,CGenericVector<float> &normal,CGenericVector<float> &binormal)
{
	normal.Y(0.0f);
	normal.H(1.0f);
	binormal.H(1.0f);

	if ((v.X() == 0) && (v.Z() == 0))
	{
		normal.X(0.0f);
		normal.Z(1.0f);
	}
	else
	{
		float d2 = 1.0f / sqrt(v.Z()*v.Z() + v.X()*v.X());
		normal.X(v.Z() * d2);
		normal.Z(-v.X() * d2);
	}

	binormal = v ^ normal;

	if (binormal.Y() < 0)
	{
		normal = -normal;
		binormal = -binormal;

		normal.H(1.0f);
		binormal.H(1.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
//	Engine collisions
//
///////////////////////////////////////////////////////////////////////////////////////
bool C3DEngine::glAddForCollision(CPhysics *obj)
{
	GL_MATRIX	glMView;
	const GL_MATRIX	&transform = obj->getObject()->getTransform();

	glGetFloatv(GL_MODELVIEW_MATRIX,glMView);

	for (int j=0; j<4 ; j++)
	{
		float x0 = glMView[j];
		float y0 = glMView[j+4];
		float z0 = glMView[j+8];
		float h0 = glMView[j+12];

		for (int i=0;i<4;i++)
		{
			m_pAttributes->currentModelview[i+4*j] = x0 * transform[4*i] + y0 * transform[4*i+1] + z0 * transform[4*i+2] + h0 * transform[4*i+3];
		}
	}
	
	m_pAttributes->currentStatus.clear();

    //  TODO : compute only the z-component for camera collision : see C3DEngine::Clip
	IntersectBBox(	m_camera,
					obj->getObject()->getObject()->boundingBox(),
					m_pAttributes->currentModelview,
					m_pAttributes->currentStatus);

	if (m_pAttributes->currentStatus.m_intersect > 0)
		onCameraCollision(obj);

	m_collision->Insert(obj,m_pAttributes->currentModelview,m_pAttributes->currentStatus);

	CATCH_GL_ERROR

	return (m_pAttributes->currentStatus.m_backZ == 0);
}

bool C3DEngine::collisionReady(void) const
{
	return	(m_collision->m_up == NULL) &&
			(m_collision->m_down == NULL) &&
			(m_collision->m_left == NULL) &&
			(m_collision->m_right == NULL) &&
			(m_collision->m_front == NULL) &&
			(m_collision->m_back == NULL) ;
}

void C3DEngine::clearCollision(void)
{
	if (m_collision->m_up) { delete m_collision->m_up; m_collision->m_up = NULL; }
	if (m_collision->m_down) { delete m_collision->m_down; m_collision->m_down = NULL; }
	if (m_collision->m_left) { delete m_collision->m_left; m_collision->m_left = NULL; }
	if (m_collision->m_right) { delete m_collision->m_right; m_collision->m_right = NULL; }
	if (m_collision->m_front) { delete m_collision->m_front; m_collision->m_front = NULL; }
	if (m_collision->m_back) { delete m_collision->m_back; m_collision->m_back = NULL; }

	C3DEngineAttributes::m_iNbCollisions= 0;
}


#define INTERSECT\
	if (t < 1.0)\
	{\
		IntersectStatus(B1,min,max,status); \
		return;\
	}

void C3DEngine::IntersectBBox(const CBoundingBox *B1,
							  const CBoundingBox *B2,
							  GL_MATRIX &M,
							  CCollisionStatus &status)
{
	GL_COORD_VERTEX tmp;
	GL_COORD_VERTEX org;
	GL_COORD_VERTEX dir;

	GL_COORD_VERTEX H;
	GL_COORD_VERTEX W;
	GL_COORD_VERTEX D;

	GL_COORD_VERTEX max;
	GL_COORD_VERTEX min;

	//	Check if an intersection is possible
	B1->getCenter(min);
	B2->getCenter(tmp);
	max.x = M.rowx.x*tmp.x + M.rowx.y*tmp.y + M.rowx.z*tmp.z + M.rowx.h;
	max.y = M.rowy.x*tmp.x + M.rowy.y*tmp.y + M.rowy.z*tmp.z + M.rowy.h;
	max.z = M.rowz.x*tmp.x + M.rowz.y*tmp.y + M.rowz.z*tmp.z + M.rowz.h;
	max.h = 1.0;
	
	if (sqrt(	(max.x - min.x)*(max.x - min.x) +
				(max.y - min.y)*(max.y - min.y) +
				(max.z - min.z)*(max.z - min.z))	
		> 
		(B1->getDiameter() + B2->getDiameter()))
		return;
	
	B2->get(org,dir);


	//	First, transform width, height and depth
	//	width => max.x - min.x
	W.x = M.rowx.x*(dir.x - org.x);
	W.y = M.rowy.x*(dir.x - org.x);
	W.z = M.rowz.x*(dir.x - org.x);
	W.h = 1.0;
	//	height => max.y - min.y
	H.x = M.rowx.y*(dir.y - org.y);
	H.y = M.rowy.y*(dir.y - org.y);
	H.z = M.rowz.y*(dir.y - org.y);
	H.h = 1.0;
	//	depth => max.z - min.z
	D.x = M.rowx.z*(dir.z - org.z);
	D.y = M.rowy.z*(dir.z - org.z);
	D.z = M.rowz.z*(dir.z - org.z);
	D.h = 1.0;
	
	//	Second, transform B2 into B1's coordinates
	min.x = M.rowx.x*org.x + M.rowx.y*org.y + M.rowx.z*org.z + M.rowx.h;
	min.y = M.rowy.x*org.x + M.rowy.y*org.y + M.rowy.z*org.z + M.rowy.h;
	min.z = M.rowz.x*org.x + M.rowz.y*org.y + M.rowz.z*org.z + M.rowz.h;
	min.h = 1.0;

	max.x = M.rowx.x*dir.x + M.rowx.y*dir.y + M.rowx.z*dir.z + M.rowx.h;
	max.y = M.rowy.x*dir.x + M.rowy.y*dir.y + M.rowy.z*dir.z + M.rowy.h;
	max.z = M.rowz.x*dir.x + M.rowz.y*dir.y + M.rowz.z*dir.z + M.rowz.h;
	max.h = 1.0;


	//	now we can check the 12 segments of the bounding box
	float t = FLT_MAX;
	status.m_backZ = 8;

	//	4 depth
	org.x = min.x; org.y = min.y; org.z = min.z;
	t = B1->intersect(org,D);
	INTERSECT

	org.x = min.x + H.x; org.y = min.y + H.y; org.z = min.z + H.z;
	t = B1->intersect(org,D);
	INTERSECT

	org.x = min.x + W.x; org.y = min.y + W.y; org.z = min.z + W.z;
	t = B1->intersect(org,D);
	INTERSECT

	org.x = org.x + H.x; org.y = org.y + H.y; org.z = org.z + H.z;
	t = B1->intersect(org,D);
	INTERSECT

	//	4 width
	org.x = min.x; org.y = min.y; org.z = min.z;
	t = B1->intersect(org,W);
	INTERSECT

	org.x = min.x + H.x; org.y = min.y + H.y; org.z = min.z + H.z;
	t = B1->intersect(org,W);
	INTERSECT

	org.x = min.x + D.x; org.y = min.y + D.y; org.z = min.z + D.z;
	t = B1->intersect(org,W);
	INTERSECT

	org.x = org.x + H.x; org.y = org.y + H.y; org.z = org.z + H.z;
	t = B1->intersect(org,W);
	INTERSECT

	//	4 height
	org.x = min.x; org.y = min.y; org.z = min.z;
	t = B1->intersect(org,H);
	INTERSECT

	org.x = min.x + W.x; org.y = min.y + W.y; org.z = min.z + W.z;
	t = B1->intersect(org,H);
	INTERSECT

	org.x = min.x + D.x; org.y = min.y + D.y; org.z = min.z + D.z;
	t = B1->intersect(org,H);
	INTERSECT

	org.x = org.x + W.x; org.y = org.y + W.y; org.z = org.z + W.z;
	t = B1->intersect(org,H);
	INTERSECT
}

void C3DEngine::IntersectStatus(const CBoundingBox *Box,GL_COORD_VERTEX &min,GL_COORD_VERTEX &max,CCollisionStatus &status)
{
	float xmin,xmax,ymin,ymax,zmin,zmax;
	Box->get(xmin,ymin,zmin,xmax,ymax,zmax);

	status.m_backZ = 0;

	//	SSE should implement this very fast
	if (min.x < xmin) status.m_leftX += 2; else status.m_rightX += 2;
	if (min.y < ymin) status.m_downY += 2; else status.m_upY += 2;
	if (min.z < zmin) status.m_backZ += 2; else status.m_frontZ += 2;

	if (max.x < xmin) status.m_leftX += 2; else status.m_rightX += 2;
	if (max.y < ymin) status.m_downY += 2; else status.m_upY += 2;
	if (max.z < zmin) status.m_backZ += 2; else status.m_frontZ += 2;

	if (min.x < xmax) status.m_leftX += 2; else status.m_rightX += 2;
	if (min.y < ymax) status.m_downY += 2; else status.m_upY += 2;
	if (min.z < zmax) status.m_backZ += 2; else status.m_frontZ += 2;

	if (max.x < xmax) status.m_leftX += 2; else status.m_rightX += 2;
	if (max.y < ymax) status.m_downY += 2; else status.m_upY += 2;
	if (max.z < zmax) status.m_backZ += 2; else status.m_frontZ += 2;

	status.m_intersect = 0x0070;
}


///////////////////////////////////////////////////////////////////////////////////////
//	Virtual defaults :
//	we do nothing here, user will derive a 3DEngine
//	to implement behaviour.
void C3DEngine::onCameraCollision(CPhysics *) const
{
}

void C3DEngine::onObjectCollision(CPhysics *,CPhysics *) const
{
}
///////////////////////////////////////////////////////////////////////////////////////


void C3DEngine::collideObjects(CPhysics *obj1,CPhysics *obj2) const
{
	//	Compute resultants for both objects
	CGenericVector<float> P = obj1->getSpeed() * obj1->mass + obj2->getSpeed() * obj2->mass;
	//float E0 = obj1->E + obj2->E;

	//	Compute resultant vectors
	switch (obj1->collisionModel)
	{
		case CPhysics::CGL_PLANAR_COLLISION:
		{

			break;
		}
		case CPhysics::CGL_SPHERE_COLLISION:
		{
			break;
		}
	}

	switch (obj2->collisionModel)
	{
		case CPhysics::CGL_PLANAR_COLLISION:
		{
			break;
		}
		case CPhysics::CGL_SPHERE_COLLISION:
		{
			break;
		}
	}
	
	C3DEngineAttributes::m_iNbCollisions++;
}

