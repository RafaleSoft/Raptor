// Physics.h: interface for the CPhysics class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
#define AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3DInstance;


class RAPTOR_API CPhysics : public CPersistence, public CTimeObject
{
public:
	//	Physical constants models
	static const double _g;	//! gravity acceleration
	static const double _G;	//!	universal attraction

	typedef enum
	{
		CGL_NO_COLLISION,
		CGL_PLANAR_COLLISION,
		CGL_SPHERE_COLLISION
	} COLLISION_MODEL;

    
    //!		CPhysics force equation solver( units in m/s^2 )
    //!	A user defined pfd will work with all the forces
    //!	provided by the physics object to which it is linked.
    //!	There is no special case for diracs, except the fact
    //!	that they will appear only once
    typedef void (RAPTOR_CCALL *FORCES_PROC)(	CPhysics *physics,
												CGenericVector<float> &accel,
												CGenericVector<float> &raccel);

    
    //	Definition of a force that will be computed by Pfd
    //	Rq : fVect.H must be set to 0.0f
    //	Each coordinate of vect define force intensity in
    //	stadard units : i.e. in Newton
    //	( N = kg * m * s^-2 )
    class CForce
    {
    public:
		CForce():pos()
	    {
			vect.Zero();
		    dirac = false;
		    position_attached = false;
		    vector_attached = false;
	    }

        virtual ~CForce() {};

        CGenericVector<float>	vect;
	    CGenericVector<float>	pos;
	    bool					dirac;
	    bool					position_attached;	//	fPos will translate along with physics transform
	    bool					vector_attached;	//	fVect will be oriented with physics transform

    };


//	properties, initialised by the user
public:
	float	mass;	// essential parameter for pfd
					//	CAUTION : mass ( kg ) > 0 !
	CGenericVector<float>	G;		// a well known point ;-)

	COLLISION_MODEL	collisionModel;


	//!
	//!	physical result of transformations
	//!	given real time if a CAnimator exists
	//!	otherwise, computed by deltaTime(dt)
	//!
	float	E;	// energy: E = Ec + J
				// E = 1/2.m.vx^2 + 1/2.m.vr^2-> collisions

				//	dx = (V + 1/2dV) * dt  where dV = A * dt
	CGenericVector<float>	dx;
				//	drx = (Vr + 1/2dVr) * dt where dVr = Ar * dt
				//	for easy use, drx is transformed as:
				//	=> drx = rotation axis
				//	=> drx.h = angle of rotation
	CGenericVector<float>	drx;

	static CGenericVector<float>	dV;
	static CGenericVector<float>	M;
	static CGenericMatrix<float>	Rotation;


public:
	CPhysics(	CObject3DInstance *obj = NULL,
				const std::string& name = "PHYSICS");

	virtual ~CPhysics();

	//	Force management
	void SetPfd(FORCES_PROC newpfd)	{ pfd = newpfd; };

	unsigned int getNbForces(void) const { return forces.size(); };

	CForce* RAPTOR_FASTCALL getForce(unsigned int index) const;
	//	Adds the gravity force to the object.
	//	this method should be called only once per 
	//	physics, otherwise the linked object
	//	will virtually be as many times more heavy
	//	as you will call AddGravity
	//	The method returns the computed force
	CForce* addGravity(void);

	//	Returns true if the force is actually added
	bool RAPTOR_FASTCALL addForce(CForce *force);

	//	Returns NULL is this force is unknown
	CForce* removeForce(CForce *force);	// RemoveForce is needed to remove
												// forces that are not diracs.
												// Diracs are automatically
												// removed after a pfd calculation
												// The user must keep track of all the
												// forces managed, because Physics do
												// not delete them !

	//	Dirach modelisation is necessary to initiate movement
	void setAcceleration(float xa,float ya,float za) 
    { accel.Set(xa,ya,za,0.0f); };

	void setSpeed(float xs,float ys,float zs) 
    { speed.Set(xs,ys,zs,0.0f); };

	void setAngularSpeed(float xas,float yas,float zas) 
    { rspeed.Set(xas,yas,zas,0.0f); };

	//	Friction parameter can be provided to the physics
	//	calculation. Each vector coordinate specifies a
	//	coefficient ( in [0..1] ) used as a friction resultant
	//	in each unit vector direction
	void setFriction(float fx,float fy,float fz);
	void setAngularFriction(float fx,float fy,float fz);

	//	Physics is applied on an "physical" object
	void setObject(CObject3DInstance *obj);
	CObject3DInstance *getObject(void) const { return object; };

	//	Physical states ( in addition with public E,dx & drx )
	const CGenericVector<float>& getSpeed(void) const { return speed; };
	const CGenericVector<float>& getAcceleration(void) const { return accel; };

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CPhysicsClassID,"Physics",CPersistence)



protected:
	FORCES_PROC pfd;

	CGenericVector<float>	accel;
	CGenericVector<float>	speed;
	CGenericVector<float>	raccel;
	CGenericVector<float>	rspeed;
	CGenericVector<float>	friction;
	CGenericVector<float>	afriction;

	//!	Compute physics resultant after dt time increment
	virtual void RAPTOR_FASTCALL deltaTime(float dt);


private:
    //!	Inherit from persistence to observe shader life-cycle
	void unLink(const CPersistence* obj);

//	physic is applyed on geometry.
//	fast movement can be achieved using an instance of a geometry
//	because it only affects its internal transform matrix
	CObject3DInstance	*object;
	vector<CForce*>		forces;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)

