// CollisionDisplay.cpp: implementation of the CCollisionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Test2.h"
#include "CollisionDisplay.h"


#include "Raptordll.h"

#include "TestEngine.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/Object3DInstance.h"
#include "Engine/Physics.h"
#include "Engine/3DScene.h"
#include "System/Raptor.h"
#include "System/RaptorDisplay.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/GeometryEditor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisionDisplay::CCollisionDisplay():
	ball(NULL),
	instGround(NULL),
	instBall(NULL),
	physicsGround(NULL),
	physicsBall(NULL)
{

}

CCollisionDisplay::~CCollisionDisplay()
{
}

void CCollisionDisplay::Init()
{
	CGenericDisplay::Init();

	CPersistence *p = CPersistence::FindObject("GeoSphere0");
	if (p->getId().isSubClassOf(CGeometry::CGeometryClassID::GetClassId()))
		 ball = (CGeometry *)p;

	ground = new CGeometry("GROUND");
    
	ground->glSetVertices(8);
	ground->glSetPolygons(12);
	ground->glLockData();
		ground->addVertex(-10.0f,-6.0f,-10.0f,1.0f);
		ground->addVertex(-10.0f,-6.0f,10.0f,1.0f);
		ground->addVertex(10.0f,-6.0f,10.0f,1.0f);
		ground->addVertex(10.0f,-6.0f,-10.0f,1.0f);

		ground->addVertex(-10.0f,-5.0f,-10.0f,1.0f);
		ground->addVertex(-10.0f,-5.0f,10.0f,1.0f);
		ground->addVertex(10.0f,-5.0f,10.0f,1.0f);
		ground->addVertex(10.0f,-5.0f,-10.0f,1.0f);
/*
		ground->setTexCoord(0,0,0);
		ground->setTexCoord(1,0,2);
		ground->setTexCoord(2,2,2);
		ground->setTexCoord(3,2,0);

		ground->setTexCoord(4,2,2);
		ground->setTexCoord(5,2,0);
		ground->setTexCoord(6,0,0);
		ground->setTexCoord(7,0,2);
*/ 
		//	front
		ground->addFace(0,1,2);ground->addFace(2,3,0);
		//	left
		ground->addFace(4,0,3);ground->addFace(3,7,4);
		//	right
		ground->addFace(2,1,5);ground->addFace(5,6,2);
		//	down
		ground->addFace(1,0,4);ground->addFace(4,5,1);
		//	up
		ground->addFace(3,2,6);ground->addFace(6,7,3);
		//	back
		ground->addFace(7,6,5);ground->addFace(5,4,7);

		const CGeometryEditor &pEditor = ground->getEditor();
		pEditor.genNormals();
    ground->glUnLockData();

	instBall = new CObject3DInstance(ball,"ball instance");
	instBall->rotationX(-45.0f);
	instBall->rotationY(45.0f);
	instBall->translate(0.0f,10.0f,-10.0f);
	
	instGround = new CObject3DInstance(ground,"ground instance");
	instGround->translate(0.0f,-1.0f,-10.0f);
	
	physicsGround = new CPhysics(instGround,"PHYSICS_GROUND");	
	physicsGround->collisionModel = CPhysics::CGL_PLANAR_COLLISION;
	physicsBall = new CPhysics(instBall,"PHYSICS_BALL");
	physicsBall->mass = 10.0f;
	physicsBall->collisionModel = CPhysics::CGL_PLANAR_COLLISION;
	physicsBall->addGravity(); 

	C3DScene *pScene = new C3DScene("COLLISION_SCENE");
	pScene->addObject(instGround);
	pScene->addObject(instBall);

	CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	pDisplay->addScene(pScene);
}


void CCollisionDisplay::ReInit()
{
	CGenericDisplay::ReInit();

	instBall->instantiate(ball);
	instBall->translateAbsolute(0.0f,10.0f,-10.0f);
	physicsBall->setSpeed(0,0,0);
	physicsBall->animate(true);

    CRaptorDisplay* pDisplay = CRaptorDisplay::GetCurrentDisplay();
	CRenderingProperties *rp = pDisplay->getRenderingProperties();
	rp->setTexturing(CRenderingProperties::DISABLE);
    rp->setLighting(CRenderingProperties::DISABLE);
	rp->setBlending(CRenderingProperties::DISABLE);

	pDisplay->selectScene("COLLISION_SCENE");
}

void CCollisionDisplay::UnInit()
{
}

void CCollisionDisplay::Display()
{
	if (reinit)
		ReInit();

	C3DEngine::Get3DEngine()->clearCollision();
	
	C3DEngine::Get3DEngine()->glAddForCollision(physicsGround);
	C3DEngine::Get3DEngine()->glAddForCollision(physicsBall);
}