// 3DSet.cpp: implementation of the C3DSet class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DSet.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
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
#if !defined(AFX_OBJECT3DCONTOUR_H__C0C2B562_ABBC_4B04_A1E7_E0727FAC66AB__INCLUDED_)
	#include "GLHierarchy/Object3DContour.h"
#endif
#if !defined(AFX_3DSETITEM_H__3AA83803_A8B2_457E_A80F_718D6678D740__INCLUDED_)
    #include "Subsys/3DSetItem.h"
#endif
#if !defined(AFX_OBJECT3DCONTAINERNOTIFIER_H__BF1EABCD_500E_4D7C_8561_2C535DF0640A__INCLUDED_)
    #include "Subsys/Object3DContainerNotifier.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

RAPTOR_NAMESPACE

C3DSet::C3DSetIterator::C3DSetIterator()
{
    m_pItem = NULL;
}

C3DSet::C3DSetIterator::C3DSetIterator(C3DSetItem *pos)
{
    m_pItem = pos;
}

C3DSet::C3DSetIterator::C3DSetIterator(const C3DSet::C3DSetIterator& rsh)
{
    m_pItem = rsh.m_pItem;
}

C3DSet::C3DSetIterator& C3DSet::C3DSetIterator::operator++(int)
{
    if (m_pItem != NULL)
        m_pItem = m_pItem->getNextChild();

    return *this;
}

C3DSet::C3DSetIterator& C3DSet::C3DSetIterator::operator--(int)
{
    if (m_pItem != NULL)
        m_pItem = m_pItem->getPreviousChild();

    return *this;
}

C3DSet::C3DSetIterator& C3DSet::C3DSetIterator::operator+(unsigned int nb)
{
    unsigned int pos = 0;

    while ((m_pItem != NULL) && (pos++ < nb))
        m_pItem = m_pItem->getNextChild();

    return *this;
}

C3DSet::C3DSetIterator& C3DSet::C3DSetIterator::operator-(unsigned int nb)
{
    unsigned int pos = 0;

    while ((m_pItem != NULL) && (pos < nb))
        m_pItem = m_pItem->getPreviousChild();

    return *this;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static C3DSet::C3DSetClassID setId;
static CPersistentObjectType<C3DSet> setFactory(setId);
const CPersistence::CPersistenceClassID& C3DSet::C3DSetClassID::GetClassId(void)
{
	return setId;
}


C3DSet::C3DSet(const std::string& name)
    :CObject3D(setId,name),
    m_pRoot(NULL)
{
    m_pRoot = new C3DSetItem(NULL);

    m_pObserver = new CObject3DContainerNotifier<C3DSet,CObject3D*>(*this,&C3DSet::notifyFromChild);
}

C3DSet::~C3DSet()
{
    if (m_pRoot != NULL)
    {
        C3DSetItem  *it = m_pRoot->getFirstChild();
        while (it != NULL)
        {
            it->getObject()->unregisterDestruction(this);
            it->getObject()->removeContainerNotifier(m_pObserver);
            it = it->getNextChild();
        }
        delete m_pRoot;
    }

    delete m_pObserver;
}

void C3DSet::notifyFromChild(CObject3D* child)
{
    if ((child != NULL) &&
        (m_pObserver->getEvent() == CContainerNotifier<CObject3D*>::UPDATEBBOX))
    {
        GL_COORD_VERTEX min;
		GL_COORD_VERTEX max;

		child->getBoundingBox(min,max);

        if (!boundingBox()->isInBox(min.x,min.y,min.z) ||
            !boundingBox()->isInBox(max.x,max.y,max.z))
            rescanBBox();
    }   
}

vector<CObject3DContour*> C3DSet::createContours(void)
{
    vector<CObject3DContour*>  res;
    CObject3DContour *pContour = new CObject3DContour;
    res.push_back(pContour);

    pContour->init(this);

    return res;
}

void C3DSet::unLink(const CPersistence* p)
{
    removeChild((CObject3D*)p);
}

C3DSet::C3DSetIterator  C3DSet::getIterator(void) const
{
	C3DSet::C3DSetIterator it(m_pRoot);

    return it;
}

CObject3D *const C3DSet::getChild (const C3DSet::C3DSetIterator& position) const
{
    if (position() != NULL)
        return position()->getObject();
    else
        return NULL;
}

bool C3DSet::addChild (CObject3D *pObject, const C3DSetIterator& position)
{ 
	if ((pObject != NULL) && (pObject != this) && (position() != NULL))
	{   
        if (position() != NULL)
        {
            GL_COORD_VERTEX Min;
            GL_COORD_VERTEX Max;

            if (m_pRoot->getFirstChild() == NULL)
                pObject->getBoundingBox(Min,Max);
            else
            {
                CBoundingBox tmpBox = *boundingBox();
                tmpBox += *pObject->boundingBox();
                tmpBox.get(Min,Max);
            }

            setBoundingBox(Min,Max);

            C3DSetItem *newObject = new C3DSetItem(pObject);
            position()->addChild(newObject);

            pObject->registerDestruction(this);

            pObject->addContainerNotifier(m_pObserver);
        }
	
		return true;
	}
	else
		return false;
}

bool C3DSet::removeChild(CObject3D *pObject)
{
    C3DSetItem  *it = m_pRoot->getFirstChild();
    bool res = false;

    while (it != NULL)
    {
        if (it->getObject() == pObject)
        {
            pObject->removeContainerNotifier(m_pObserver);
            delete it;  //  automagically detach the object at deletion
            it = NULL;
            res = true;
        }
        else
            it = it->getNextChild();
    }

    rescanBBox();

    return res;
}

void C3DSet::rescanBBox(void)
{
    C3DSetItem  *it = m_pRoot->getFirstChild();
    if (it == NULL)
    {
        GL_COORD_VERTEX nil(0.0f,0.0f,0.0f,1.0f);
        setBoundingBox(nil,nil);
        return;
    }

    GL_COORD_VERTEX Max(FLT_MAX,FLT_MAX,FLT_MAX,1.0f);
    GL_COORD_VERTEX Min(-FLT_MAX,-FLT_MAX,-FLT_MAX,1.0f);
	CBoundingBox tmpBox;
    tmpBox.set(Max,Min);

    while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
			tmpBox += *obj->boundingBox();

		it = it->getNextChild();
	}

    tmpBox.get(Min,Max);
    setBoundingBox(Min,Max);
}

void C3DSet::scale(float sx,float sy,float sz)
{	
	GL_COORD_VERTEX Center;
    GL_COORD_VERTEX Center2;
	getCenter(Center);

    //! After scaling the local objet, we can ignore childs' notification
    //! because they will remain in the box.
    //! To avoid computation limitations ( e.g. of using floats ), a call to rescanBBox could be added.
    scaleBoundingBox(sx,sy,sz);
    m_pObserver->enable(false);

    C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
        {
		    obj->scale(sx,sy,sz);
		    obj->getCenter(Center2);
		    obj->translate((Center.x-Center2.x)*(1-sx),(Center.y-Center2.y)*(1-sy),(Center.z-Center2.z)*(1-sz));
        }
		
        it = it->getNextChild();
	}

    m_pObserver->enable(true);
}


void C3DSet::translateAbsolute(float tx,float ty,float tz)
{
	GL_COORD_VERTEX c;
	getCenter(c);

    //! See comment in ::scale
    translateCenter(tx-c.x,ty-c.y,tz-c.z);
    m_pObserver->enable(false);

    C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->translate(tx-c.x,ty-c.y,tz-c.z);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
}


void C3DSet::translate(float tx,float ty,float tz)
{
    //! See comment in ::scale
	translateCenter(tx,ty,tz);
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->translate(tx,ty,tz);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
}

void C3DSet::rotationX(float rx)
{
	GL_COORD_VERTEX Center;
	getCenter(Center);

    //! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationX(rx,Center.y,Center.z);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotationY(float ry)
{
	GL_COORD_VERTEX Center;
	getCenter(Center);

	//! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationY(ry,Center.x,Center.z);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotationZ(float rz)
{
	GL_COORD_VERTEX Center;
	getCenter(Center);

	//! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationZ(rz,Center.x,Center.y);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotationX(float rx,float cy,float cz)
{
	//! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationX(rx,cy,cz);
		
        it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotationY(float ry,float cx,float cz)
{
    //! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationY(ry,cx,cz);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotationZ(float rz,float cx,float cy)
{
	//! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->rotationZ(rz,cx,cy);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::rotation(double angle,float ax,float ay,float az)
{
	//! See comment in ::scale
    m_pObserver->enable(false);

	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
            obj->rotation(angle,ax,ay,az);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

void C3DSet::transform(GL_MATRIX &m)
{
	//! See comment in ::scale
    m_pObserver->enable(false);
				
	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->transform(m);

		it = it->getNextChild();
	}

    m_pObserver->enable(true);
    rescanBBox();
}

bool RAPTOR_FASTCALL C3DSet::fullClip(const CGenericMatrix<float> &transform)
{
    earlyClip = C3DEngine::Get3DEngine()->glClip(boundingBox(),false,&transform);

    C3DSetItem  *it = m_pRoot->getFirstChild();

	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
            obj->fullClip(transform);

        it = it->getNextChild();
    }

    return (earlyClip != C3DEngine::CLIP_UNKNOWN);
}

void C3DSet::glRender()
{
	if (!properties.isVisible())
		return;

    C3DSetItem  *it = m_pRoot->getFirstChild();

	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->glRender();

		it = it->getNextChild();
	}

    CATCH_GL_ERROR

    CRaptorInstance::GetInstance().iRenderedObjects++;
}

void C3DSet::glClipRender()
{
	if (!properties.isVisible())
		return;

    C3DEngine::CLIP_RESULT clip = earlyClip;
    //if (!isEarlyClipEnabled())
	    clip = this->glClip();

    if (clip == C3DEngine::CLIP_FULL)
		return;

	C3DSetItem  *it = m_pRoot->getFirstChild();

	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
        {
		    if (clip == C3DEngine::CLIP_NONE)
			    obj->glRender();
		    else
			    obj->glClipRender();
        }

		it = it->getNextChild();
	}

    CATCH_GL_ERROR

	CRaptorInstance::GetInstance().iRenderedObjects++;
}

void C3DSet::glRenderAllBBox(bool filled)
{
	if (!properties.isVisible())
		return;

	C3DSetItem  *it = m_pRoot->getFirstChild();

	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
        {
			if (obj->getId().isSubClassOf(C3DSet::C3DSetClassID::GetClassId()))
			    ((C3DSet *)obj)->glRenderAllBBox(filled);
		    else
			    obj->glRenderBBox(filled);
        }

		it = it->getNextChild();
	}

    CATCH_GL_ERROR
}


bool C3DSet::exportObject(CRaptorIO& o)
{
	int nbObj = 0;

	CPersistence::exportObject(o);

	GL_COORD_VERTEX Min;
	GL_COORD_VERTEX Max;
    getBoundingBox(Min,Max);

	o << Min.x;
	o << Min.y;
	o << Min.z;
	o << Max.x;
	o << Max.y;
	o << Max.z;
	
	C3DSetItem  *it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
		nbObj++;
		it = it->getNextChild();
	}
	o << nbObj;

	it = m_pRoot->getFirstChild();
	while (it != NULL)
	{
        CObject3D *obj = it->getObject();
        if (obj != NULL)
		    obj->exportObject(o);

		it = it->getNextChild();
	}

	return true;
}

bool C3DSet::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Object3D")
                CObject3D::importObject(io);
		else if (data == "Child")
		{
            io >> name;
			data = io.getValueName();
			if (data == "name")
			{
			    io >> name;
			    CPersistence* object = CPersistence::FindObject(name);
                if (object != NULL)
                {
				    const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
                    C3DSet::C3DSetIterator pos = getIterator();
			        addChild(op,pos);
                }
            }
            io >> name;
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	return true;
}

