// GLLod.cpp: implementation of the CGLLod class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_GLLOD_H__95ED92DC_1BF7_4869_912C_655779B2ED7B__INCLUDED_)
	#include "GLLod.h"
#endif

#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DSet.h"
#endif

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
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


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CGLLod::CGLLodClassID lodId;
static CPersistentObjectType<CGLLod> lodFactory(lodId);
const CPersistence::CPersistenceClassID& CGLLod::CGLLodClassID::GetClassId(void)
{
	return lodId;
}

CGLLod::CGLLod(CObject3D * const level0, const std::string& name)
	:CObject3DInstance(lodId,name)
{
    if (level0 != NULL)
        addLevel(0,level0);
}

CGLLod::~CGLLod()
{
	// the first level must be set because it is the one
	// referenced in the instance.
	m_pReference = getLod(0);

	vector<LOD>::iterator pos = lods.begin();
    while (pos != lods.end())
    {
		const LOD& lod = *pos++;
		lod.obj->unregisterDestruction(this);
	}

	lods.clear();
}

void CGLLod::unLink(const CPersistence* obj)
{
    vector<LOD>::iterator pos = lods.begin();
    while (pos != lods.end())
    {
        if (obj == static_cast<CPersistence*>((*pos).obj))
        {
            lods.erase(pos);
            pos = lods.begin();
        }
        else
            pos++;
    }
    
    CObject3DInstance::unLink(obj);
}

vector<CObject3DContour*> CGLLod::createContours(void)
{
    vector<CObject3DContour*>  res;

    vector<LOD>::iterator pos = lods.begin();
    while (pos != lods.end())
    {
        vector<CObject3DContour*> pContours = (*pos++).obj->createContours();
        if (!pContours.empty())
        {
            vector<CObject3DContour*>::const_iterator it = pContours.begin();
            while (it != pContours.end())
                res.push_back(*it++);
        }
    }

    return res;
}


CObject3D* const CGLLod::getLod(unsigned int numLod) const
{ 
	if ((unsigned int)(lods.size()) > numLod) 
		return lods[numLod].obj; 
	else 
		return NULL; 
}

bool CGLLod::addLevel(float fromDepth, CObject3D *obj)
{
	if (obj == NULL)
		return false;

	LOD lod;
	lod.fromDepth = fromDepth;
	lod.obj = obj;

    obj->registerDestruction(this);

	if (lods.size() == 0)
	{
		lods.push_back(lod);
		instantiate(obj);
	}
	else
	{
		vector<LOD>::iterator pos = lods.begin();
		while ((pos != lods.end())&&((*pos).fromDepth<fromDepth))
			pos++;

		//	Insertion
		if (pos != lods.end())
			lods.insert(pos,lod);
		//	Add tail
		else
			lods.push_back(lod);
	}

	return true;
}


__inline CObject3D* const CGLLod::glGetLod(void) const
{
    CGenericMatrix<float> transform;
    glGetFloatv(GL_MODELVIEW_MATRIX,transform.matrix());
    CGenericMatrix<float> distance;
    distance = m_transform;
    distance *= transform;

	float d = sqrt(distance[12]*distance[12] + distance[13]*distance[13] + distance[14]*distance[14]);
	int pos = 0;
	int max = lods.size();
	while ((pos < max) && (lods[pos].fromDepth<d))
		pos++;

	if (pos >= max)
		pos = max - 1;

    return lods[pos].obj;
}

void CGLLod::glRender()
{
	m_pReference = glGetLod();

	CATCH_GL_ERROR

	CObject3DInstance::glRender();
}


void CGLLod::glClipRender()
{
    m_pReference = glGetLod();

	CATCH_GL_ERROR

	CObject3DInstance::glClipRender();
}


bool CGLLod::exportObject(CRaptorIO& o)
{
	/*
	CObject3D *Aux;

	CObject3DInstance::exportObject(o);

	int nbObj = lods.size();

	o << nbObj;

	for (int i=0;i<nbObj;i++)
	{
		LOD& lod = lods[i];

		o << lod.fromDepth;
		
		Aux = lod.obj;
		unsigned int ID = Aux->getId();
		o << ID;

		if (ID == ID_CLASS_3DSET)
			((C3DSet*)Aux)->exportObject(o);
		else if (ID == ID_CLASS_GEOMETRY)
			((CGeometry*)Aux)->exportObject(o);
	}
	*/

	return false;
}


bool CGLLod::importLevel(CRaptorIO& io)
{
    string name;
    io >> name;

    LOD lod;

	string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "depth")
			io >> lod.fromDepth;
        else if (data == "reference")
        {
            io >> name;
			CPersistence* object = CPersistence::FindObject(name);
            if (object != NULL)
            {
				const CPersistentObject &op = CObjectFactory::GetInstance()->getObject(object);
				lod.obj = op;
            }
        }
        else
			io >> name;

		data = io.getValueName();
    }
	io >> name;

    return addLevel(lod.fromDepth, lod.obj);
}


bool CGLLod::importObject(CRaptorIO& io)
{
    string name;
	io >> name;

	string data = io.getValueName();
	while (!data.empty())
	{
		if (data == "name")
			CPersistence::importObject(io);
        else if (data =="Object3DInstance")
            CObject3DInstance::importObject(io);
        else if (data =="Level")
            importLevel(io);
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

	return true;
}

