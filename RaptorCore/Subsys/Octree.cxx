// Octree.cxx: implementation of the Octree class.
//
//////////////////////////////////////////////////////////////////////
#if !defined __OCTREE_CXX__
#define __OCTREE_CXX__

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_OCTREE_H__FC2A5101_AB9A_11D1_B467_444553540000__INCLUDED_)
	#include "Octree.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif


RAPTOR_NAMESPACE

template <class USER_DATA_t>
const unsigned int COctree<USER_DATA_t>::OCTREE_TRESHOLD = 8;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class USER_DATA_t>
COctree<USER_DATA_t>::COctree(void)
    :m_bNeedBalance(false)
{
	for (unsigned int i=0;i<8;i++)
		child[i] = NULL;
}

template <class USER_DATA_t>
COctree<USER_DATA_t>::~COctree(void)
{
    clearChilds();
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::compress(void)
{
    unsigned int i=0;
    for (i=0 ; i<8 && child[i] != NULL ;  i++)
    {
        child[i]->compress();
        if (child[i]->getNbObjects() == 0)
        {
            delete child[i];
            child[i] = NULL;
        }
    }

    unsigned int freepos = 8;
    for (i=0 ; i<8 ; i++)
    {
        if (child[i] == NULL)
        {
            if (freepos == 8)
                freepos = i;
        }
        else if (freepos < 8)
        {
            child[freepos] = child[i];
            child[i] = NULL;
            freepos++;
        }
    }

    //! there is no child, reduce the bbox of self if it is not empty
	if ((child[0] == NULL) && (CBaseTree<USER_DATA_t>::m_objects.size() > 0))
    {
        CBoundingBox tmpBox;
        tmpBox.set(FLT_MAX,FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX);

        RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = CBaseTree<USER_DATA_t>::m_objects.begin();
        while (it != CBaseTree<USER_DATA_t>::m_objects.end())
        {
	        const BASETREE_ITEM &item = (*it++);
            const CBoundingBox * const bbox = item.pObject->boundingBox();
            tmpBox += *bbox;
        }
        CBoundingBox::operator=(tmpBox);
    }
    //  there are some childs : reduce the size to the childs + this level objects
    else if (child[0] != NULL)
    {
        CBoundingBox tmpBox;
        tmpBox.set(FLT_MAX,FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX);
        for (i=0 ; i<8 && child[i] != NULL ;  i++)
            tmpBox += *child[i];
        RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = CBaseTree<USER_DATA_t>::m_objects.begin();
        while (it != CBaseTree<USER_DATA_t>::m_objects.end())
        {
	        const BASETREE_ITEM &item = (*it++);
            const CBoundingBox * const bbox = item.pObject->boundingBox();
			tmpBox += *bbox;
        }
        CBoundingBox::operator=(tmpBox);

        // final compression : if possible, make a child with this level objects
        if ((i<8) && (child[i] == NULL))
        {
            tmpBox.set(FLT_MAX,FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX);
            child[i] = new COctree<USER_DATA_t>;
            it = CBaseTree<USER_DATA_t>::m_objects.begin();
            while (it != CBaseTree<USER_DATA_t>::m_objects.end())
            {
	            const BASETREE_ITEM &item = (*it++);
                const CBoundingBox * const bbox = item.pObject->boundingBox();
                tmpBox += *bbox;
            }
	        child[i]->CBoundingBox::operator=(tmpBox);
            child[i]->CBaseTree<USER_DATA_t>::m_objects = CBaseTree<USER_DATA_t>::m_objects;
            CBaseTree<USER_DATA_t>::m_objects.clear();
        }
    }
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::clearChilds(void)
{
    CBaseTree<USER_DATA_t>::m_objects = getObjects();

    for (unsigned int i=0 ; i<8 && child[i] != NULL ; i++)
    {
        delete child[i];
        child[i] = NULL;
    }
}

template <class USER_DATA_t>
unsigned int COctree<USER_DATA_t>::getMaxDepth() const
{
    unsigned int maxDepth = 0;

    for (unsigned int i=0 ; i<8 && child[i] != NULL ; i++)
        maxDepth = MAX(maxDepth,child[i]->getMaxDepth());

    return maxDepth + 1;
}

template <class USER_DATA_t>
size_t COctree<USER_DATA_t>::getNbObjects(void) const
{
    size_t res = CBaseTree<USER_DATA_t>::m_objects.size();

    for (unsigned int i=0 ; i<8 && child[i] != NULL ; i++)
        res += child[i]->getNbObjects();

    return res;
}

template <class USER_DATA_t>
vector<RAPTOR_TYPENAME COctree<USER_DATA_t>::BASETREE_ITEM> COctree<USER_DATA_t>::getObjects(void) const
{
    vector<BASETREE_ITEM> res = CBaseTree<USER_DATA_t>::m_objects;
    for (unsigned int i=0 ; i<8 && child[i] != NULL ; i++)
	{
        vector<BASETREE_ITEM> childObjects = child[i]->getObjects();

        RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = childObjects.begin();
	    while (it != childObjects.end())
	        res.push_back(*it++);
	}

    return res;
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::addObject(CObject3D * const obj,USER_DATA_t userData)
{
    //! Manage the bbox
    if (CBaseTree<USER_DATA_t>::m_objects.size() == 0)
        this->CBoundingBox::operator=(*obj->boundingBox());
    else
    {
        GL_COORD_VERTEX r_min;
        GL_COORD_VERTEX r_max;

        obj->boundingBox()->get(r_min,r_max);
        if (!isInBox(r_min) || !isInBox(r_max))
            m_bNeedBalance = true;

        add(*obj->boundingBox());
    }

    //! Add data to tree
    BASETREE_ITEM item;
    item.pObject = obj;
    item.userData = userData;
    addItem(item);
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::addItem(BASETREE_ITEM item)
{
    if (CBaseTree<USER_DATA_t>::m_objects.size() < OCTREE_TRESHOLD)
    {
        CBaseTree<USER_DATA_t>::m_objects.push_back(item);
        return;
    }
    
    //! Testing only the first child is sufficient.
    //! Further optimisation will remove empty childs to reduce clipping burden.
    m_bNeedBalance |= (child[0] == NULL);

    if (m_bNeedBalance)
    {
        CBaseTree<USER_DATA_t>::m_objects.push_back(item);
        balanceTree();
    }
    else 
    {
        //! Here, we need to add the child only if it is included in one single child.
        //! If the object intersects several childs, we keep it at this level.
        //! ( for faster clipping )
        const CBoundingBox * const bbox = item.pObject->boundingBox();
        GL_COORD_VERTEX c;
        getCenter(c);

		if ( (c.x > bbox->xMin() && c.x < bbox->xMax()) ||
             (c.y > bbox->yMin() && c.y < bbox->yMax()) ||
             (c.z > bbox->zMin() && c.z < bbox->zMax()))
            CBaseTree<USER_DATA_t>::m_objects.push_back(item);
        else for (unsigned int i=0;i<8 && child[i] != NULL;i++)
	    {
            GL_MATRIX voidMatrix;
            
		    if (child[i]->intersect(*bbox,voidMatrix))
            {
			    child[i]->addItem(item);
                break;
            }
	    }
    }
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::balanceTree(void)
{
    m_bNeedBalance = false;

    clearChilds();

	float xmin = 0.0f;
	float ymin = 0.0f;
	float zmin = 0.0f;
	float xmax = 0.0f;
	float ymax = 0.0f;
	float zmax = 0.0f;
	get(xmin,ymin,zmin,xmax,ymax,zmax);

    //! By default, we construct an isotropic COctree
    child[0] = new COctree<USER_DATA_t>;
	child[0]->set(xmin,ymin,zmin,0.5f*(xmin+xmax),0.5f*(ymin+ymax),0.5f*(zmin+zmax));
    child[1] = new COctree<USER_DATA_t>;
	child[1]->set(0.5f*(xmin+xmax),ymin,zmin,xmax,0.5f*(ymin+ymax),0.5f*(zmin+zmax));
    child[2] = new COctree<USER_DATA_t>;
	child[2]->set(xmin,0.5f*(ymin+ymax),zmin,0.5f*(xmin+xmax),ymax,0.5f*(zmin+zmax));
    child[3] = new COctree<USER_DATA_t>;
	child[3]->set(0.5f*(xmin+xmax),0.5f*(ymin+ymax),zmin,xmax,ymax,0.5f*(zmin+zmax));
    child[4] = new COctree<USER_DATA_t>;
	child[4]->set(xmin,ymin,0.5f*(zmin+zmax),0.5f*(xmin+xmax),0.5f*(ymin+ymax),zmax);
    child[5] = new COctree<USER_DATA_t>;
	child[5]->set(0.5f*(xmin+xmax),ymin,0.5f*(zmin+zmax),xmax,0.5f*(ymin+ymax),zmax);
    child[6] = new COctree<USER_DATA_t>;
	child[6]->set(xmin,0.5f*(ymin+ymax),0.5f*(zmin+zmax),0.5f*(xmin+xmax),ymax,zmax);
    child[7] = new COctree<USER_DATA_t>;
	child[7]->set(0.5f*(xmin+xmax),0.5f*(ymin+ymax),0.5f*(zmin+zmax),xmax,ymax,zmax);


    vector<BASETREE_ITEM> localItems;   // the list of objects for this level ( may be empty )

    GL_COORD_VERTEX c;
    getCenter(c);

	RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = CBaseTree<USER_DATA_t>::m_objects.begin();
	while (it != CBaseTree<USER_DATA_t>::m_objects.end())
	{
		const BASETREE_ITEM &item = (*it++);
		//! Here, we need to add the child only if it is included in one single child.
        //! If the object intersects several childs, we keep it at this level.
        //! ( for faster clipping )
        const CBoundingBox * const bbox = item.pObject->boundingBox();

		if ( (c.x > bbox->xMin() && c.x < bbox->xMax()) ||
             (c.y > bbox->yMin() && c.y < bbox->yMax()) ||
             (c.z > bbox->zMin() && c.z < bbox->zMax()))
            localItems.push_back(item);
        else for (unsigned int i=0;i<8 && child[i] != NULL;i++)
	    {
            GL_MATRIX voidMatrix;
            
		    if (child[i]->intersect(*bbox,voidMatrix))
            {
			    child[i]->addItem(item);
                break;
            }
	    }
	}

    //! All items have been dispatched to childs, we can clear this level.
    CBaseTree<USER_DATA_t>::m_objects = localItems;
}

template <class USER_DATA_t>
vector< RAPTOR_TYPENAME COctree<USER_DATA_t>::BASETREE_ITEM > COctree<USER_DATA_t>::glClip(bool scanModelview) const
{
    C3DEngine *engine = C3DEngine::Get3DEngine();
    vector<BASETREE_ITEM> res;
    
    C3DEngine::CLIP_RESULT clipResult = engine->glClip(this,scanModelview); //,NULL);

    if ( clipResult == C3DEngine::CLIP_FULL)
        return res;
    
    if (clipResult == C3DEngine::CLIP_NONE)
    {
        res = getObjects();
    }
    else    // CLIP_PARTS
    {
        res = CBaseTree<USER_DATA_t>::m_objects;
        for (unsigned int i=0;i<8 && child[i] != NULL;i++)
        {
            vector<BASETREE_ITEM> childClip = child[i]->glClip(false);

            RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = childClip.begin();
            while (it != childClip.end())
                res.push_back(*it++);
        }
    }

    return res;
}

template <class USER_DATA_t>
void COctree<USER_DATA_t>::glRender(void)
{
    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glColor4f(1.0f,0.0f,0.0f,1.0f);

    glBegin(GL_QUADS);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmax,ymin,zmin);glVertex3f(xmax,ymax,zmin);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymin,zmax);	glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymax,zmin);	glVertex3f(xmin,ymax,zmax);glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmin,ymin,zmin);	glVertex3f(xmin,ymin,zmax);glVertex3f(xmin,ymax,zmax);glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);	glVertex3f(xmax,ymax,zmax);glVertex3f(xmax,ymin,zmax);glVertex3f(xmax,ymin,zmin);
	glEnd();

    RAPTOR_TYPENAME vector<BASETREE_ITEM>::const_iterator it = CBaseTree<USER_DATA_t>::m_objects.begin();
	while (it != CBaseTree<USER_DATA_t>::m_objects.end())
	{
        glColor4f(1.0f,1.0f,0.0f,1.0f);

        const BASETREE_ITEM &item = (*it++);
        const CBoundingBox * const bbox = item.pObject->boundingBox();
        item.pObject->glRenderBBox(false);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        glColor4f(1.0f,0.5f,0.0f,1.0f);
        float xm,xM,ym,yM,zm,zM;
	    bbox->get(xm,ym,zm,xM,yM,zM);

        glBegin(GL_QUADS);
		    glVertex3f(xm,ym,zm);	glVertex3f(xM,ym,zm);glVertex3f(xM,yM,zm);glVertex3f(xm,yM,zm);
		    glVertex3f(xm,ym,zM);	glVertex3f(xM,ym,zM);glVertex3f(xM,yM,zM);glVertex3f(xm,yM,zM);
		    glVertex3f(xm,yM,zm);	glVertex3f(xm,yM,zM);glVertex3f(xM,yM,zM);glVertex3f(xM,yM,zm);
		    glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);glVertex3f(xM,ym,zM);glVertex3f(xM,ym,zm);
		    glVertex3f(xm,ym,zm);	glVertex3f(xm,ym,zM);glVertex3f(xm,yM,zM);glVertex3f(xm,yM,zm);
		    glVertex3f(xM,yM,zm);	glVertex3f(xM,yM,zM);glVertex3f(xM,ym,zM);glVertex3f(xM,ym,zm);
	    glEnd();
    }

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glPopAttrib();

    for (unsigned int i=0;i<8 && child[i] != NULL;i++)
        child[i]->glRender();
}

#endif

