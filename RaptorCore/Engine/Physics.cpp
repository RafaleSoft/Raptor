/***************************************************************************/
/*                                                                         */
/*  Physics.cpp                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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

#if !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Physics.h"
#endif

#if !defined(AFX_OBJECT3DINSTANCE_H__A2627662_F5F9_11D3_9142_CFEB8E9F2745__INCLUDED_)
	#include "GLHierarchy/Object3DInstance.h"
#endif


#include <math.h>

#ifdef BUILD_PHYSX
	#include <PxFoundation.h>
#endif


//////////////////////////////////////////////////////////////////////
// Const and globals
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

#pragma warning (disable : 4711)	//	hide warning 4711 : inline constructors
CGenericVector<float>	CPhysics::dV;
CGenericVector<float>	CPhysics::M;
CGenericMatrix<float>	CPhysics::Rotation;

const double CPhysics::_g = 9.8;
const double CPhysics::_G = 6.67e-11;

static CPhysics::CPhysicsClassID physicsId;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
//	Export a default physics behaviour : the standard PFD
void defaultPfd(CPhysics *physics,
				CGenericVector<float> &accel,
				CGenericVector<float> &raccel)
{
	size_t nb = physics->getNbForces();

	if (nb>0)
	{
		CPhysics::CForce *f = physics->getForce(0);


		if (f->vector_attached)
			accel = CPhysics::Rotation * f->vect;
		else
			accel = f->vect;

		if (f->position_attached)
			CPhysics::dV = ((CPhysics::Rotation * f->pos) - physics->G);
		else
			CPhysics::dV = (f->pos - physics->G);

		raccel = (CPhysics::dV ^ accel);

		if (nb>1)
			for (size_t i = 1; i<nb; i++)
			{
				f = physics->getForce(i);

				if (f->vector_attached)
					accel += CPhysics::Rotation * f->vect;
				else
					accel += f->vect;

				if (f->position_attached)
					CPhysics::dV = ((CPhysics::Rotation * f->pos) - physics->G);
				else
					CPhysics::dV = (f->pos - physics->G);

				if (f->vector_attached)
					raccel += (CPhysics::dV ^ (CPhysics::Rotation * f->vect));
				else
					raccel += (CPhysics::dV ^ f->vect);
			}

		//	compute result :
		//	- acceleration
		//	- angular acceleration
		float one_over_mass = 1 / physics->mass;
		raccel.H(0.0f);
		accel.H(0.0f);
		accel *= one_over_mass;
		raccel *= one_over_mass;

		// now, remove the diracs
		for (int i = nb - 1; i >= 0; i--)
		{
			f = physics->getForce(i);
			if (f->dirac)
			{
				physics->removeForce(f);
			}
		}
	}
	else
	{
		accel.Zero();
		raccel.Zero();
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPhysics::CPhysics(CObject3DInstance *obj,const std::string& name)
	:CPersistence(physicsId,name),
	G(),
	mass(1.0), 	E(0.0f),
	object(obj), pfd(NULL),
	collisionModel(CGL_NO_COLLISION)
{
	accel.Zero();
	raccel.Zero();
	speed.Zero();
	rspeed.Zero();
	dx.Zero();
	drx.Zero();
	friction.One();
	afriction.One();

	if (object != NULL)
	{
		GL_COORD_VERTEX v;
		object->getCenter(v);
		G.Set(v.x,v.y,v.z,1.0f);

        object->registerDestruction(this);
	}

	Rotation.Ident();
}


CPhysics::~CPhysics()
{
	//!	Avoid Engine continue to use this object beeing deleted.
	animate(false);

    if (object != NULL)
        object->unregisterDestruction(this);
}

void CPhysics::unLink(const CPersistence* obj)
{
    if (obj == static_cast<CPersistence*>(object))
    {
        object = NULL;
        G.Set(0.0f,0.0f,0.0f,1.0f);
    }
}

//////////////////////////////////////////////////////////////////////
// Physics operations
//////////////////////////////////////////////////////////////////////
void CPhysics::setObject(CObject3DInstance *obj)
{
    if (object != NULL)
        object->unregisterDestruction(this);

	object = obj; 
	if (object != NULL)
	{
		GL_COORD_VERTEX v;
		object->getCenter(v);
		G.Set(v.x,v.y,v.z,1.0f);

        object->registerDestruction(this);
	}
	else
		G.Set(0.0f,0.0f,0.0f,1.0f);
}


CPhysics::CForce* CPhysics::addGravity(void)
{
	CForce* g = new CForce;

	g->dirac = false;
	g->position_attached = true;
	g->vector_attached = false;
	g->pos = G;
	g->vect.Set(0.0f,-9.8f * mass, 0.0f, 0.0f);

	forces.push_back(g);

	return g;
}


bool RAPTOR_FASTCALL CPhysics::addForce(CForce* force)
{
	if (force != NULL)
	{
		//	force Position must have H coordinate
		force->pos.H(1.0f);
		//	force Vector must not have H coordinate
		force->vect.H(0.0f);

		forces.push_back(force);
		return true;
	}
	else
		return false;
}


CPhysics::CForce* CPhysics::removeForce(CForce* force)
{
	CForce* pForce = NULL;
	vector<CForce*>::iterator pos = forces.begin();
	
	while ((pos != forces.end()) && (*pos != force))
		pos++;

	if (pos != forces.end())
	{
		pForce = *pos;
		forces.erase(pos);
	}

	return pForce;
}

void RAPTOR_FASTCALL CPhysics::deltaTime(float dt)
{
	const GL_MATRIX &m = object->getTransform();

	CPhysics::Rotation = m;
	CPhysics::Rotation.Transpose();

	if (pfd != NULL)
		pfd(this,accel,raccel);
	else
		defaultPfd(this,accel,raccel);

	dV = accel * dt;
	M = raccel * dt;
	
	dx = speed * dt;
	drx = rspeed * dt;

	speed += dV;
	rspeed += M;
	
	speed *= friction;
	rspeed *= afriction;

	E = 0.5f*mass * ((speed*speed) + (rspeed*rspeed)).Length();

	if (object != NULL)
	{
		object->translate(dx.X(),dx.Y(),dx.Z());

		double angle = drx.Normalize();

		object->rotation(angle,drx.X(),drx.Y(),drx.Z());
		
		drx.H((float)angle);

		G += dx;
		G.H(1.0f);
	}
}


CPhysics::CForce* RAPTOR_FASTCALL CPhysics::getForce(size_t index) const
{
	if (index < forces.size())
		return forces[index];
	else
		return NULL;
}


void CPhysics::setFriction(float fx,float fy,float fz)
{
	friction.X() = MIN( 1.0f , MAX(fx,0.0f) );
	friction.Y() = MIN( 1.0f , MAX(fy,0.0f) );
	friction.Z() = MIN( 1.0f , MAX(fz,0.0f) );
}

void CPhysics::setAngularFriction(float fx,float fy,float fz)
{
	afriction.X() = MIN( 1.0f , MAX(fx,0.0f) );
	afriction.Y() = MIN( 1.0f , MAX(fy,0.0f) );
	afriction.Z() = MIN( 1.0f , MAX(fz,0.0f) );
}

