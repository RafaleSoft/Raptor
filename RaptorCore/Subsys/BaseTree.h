// Octree.h: interface for the Octree class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_BASETREE_H__1B592F1F_2E70_4061_8065_471B51A6F2C7__INCLUDED_)
#define AFX_BASETREE_H__1B592F1F_2E70_4061_8065_471B51A6F2C7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

RAPTOR_NAMESPACE_BEGIN

class CObject3D;
template <class T> class CContainerNotifier;

//! 
//! This class implements a standard octree 
//! using binary space partition in each direction
//! The template parameter enables user to specify an octree for a more convenient usage.
//!
template <class USER_DATA_t>
class CBaseTree
{
public:
    //! The kind of object held in the octree.
    typedef struct TREE_ITEM_t
    {
        CObject3D	*pObject;
        USER_DATA_t	userData;
    } TREE_ITEM;


public:
	//! Virtual destructor
	virtual ~CBaseTree() {};

	//! Inserts an object into the tree
	virtual void addObject(CObject3D * const obj, USER_DATA_t userData) = 0;

	//! Returns the list of objects from this tree level ( including all childs )
    virtual vector<TREE_ITEM> getObjects(void) const = 0;

	//! Returns the list of objects 'viewable' from this octree
    virtual vector<TREE_ITEM> glClip(bool scanModelview) const = 0;

	//! For debugging purposes only : a simple of the octree bboxes
    virtual void glRender(void) = 0;


protected:
	//! Reserved constructor
	CBaseTree() {};

	//! The data managed by the octree.
    vector<TREE_ITEM>  m_objects;


private:
    //! Forbidden operators
    CBaseTree(const CBaseTree&) {};
    CBaseTree& operator=(const CBaseTree&) {return *this; };
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BASETREE_H__1B592F1F_2E70_4061_8065_471B51A6F2C7__INCLUDED_)

