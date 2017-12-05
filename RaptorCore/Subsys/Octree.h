// Octree.h: interface for the Octree class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_OCTREE_H__FC2A5101_AB9A_11D1_B467_444553540000__INCLUDED_)
#define AFX_OCTREE_H__FC2A5101_AB9A_11D1_B467_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_BASETREE_H__1B592F1F_2E70_4061_8065_471B51A6F2C7__INCLUDED_)
	#include "BaseTree.h"
#endif

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
    #include "Engine/BoundingBox.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3D;

//! 
//! This class implements a standard octree 
//! using binary space partition in each direction
//! The template parameter enables user to specify an octree for a more convenient usage.
//!
template <class USER_DATA_t>
class COctree : public CBoundingBox, public CBaseTree<USER_DATA_t>
{
public:
	COctree(void);
	virtual ~COctree(void);

    //!
    //! Tree manipulators members
    //!
	typedef typename CBaseTree<USER_DATA_t>::TREE_ITEM BASETREE_ITEM;

    //! Inserts an object into the octree
    void addObject(CObject3D * const obj, USER_DATA_t userData);

    //! Returns the list of objects 'viewable' from this octree
    vector<BASETREE_ITEM> glClip(bool scanModelview) const;

    //! Returns the list of objects from this tree level ( including all childs )
    vector<BASETREE_ITEM> getObjects(void) const;

    //! Removes empty childs from the tree for faster clipping
    void compress(void);


    //!
    //! Tree status members
    //!

    //!  Returns the number of objects in this tree.
    unsigned int getNbObjects(void) const;

    //! Returns the deepest tree branch from this level.
    unsigned int getMaxDepth() const;

    //! For debugging purposes only : a simple of the octree bboxes
    void glRender(void);


private:
    //! Forbidden operators
    COctree(const COctree&) {};
    COctree& operator=(const COctree&) {return *this; };

    //! This method clears the tree structure : all childs are deleted, 
    //! and the list of objects is updated with the sum of child's objects.
    void clearChilds(void);

    //!	Balance the tree : computes all childs 
	void balanceTree(void);

    //! This method is used to add items to childs but without updating the bbox
    //! ( it is determined by the parent )
    void addItem(BASETREE_ITEM item);

    //! The octree will have a maximum number of eight childs.
    //! This implementation with pointers allows easier debugging or optimisation,
    //! specially when compared to a vector implementation.
	COctree *child[8];

    //! This boolean indicates that the tree must be balanced to enclose properly all objects
    bool    m_bNeedBalance;

	//!	Octree node/leaf size
	static const unsigned int OCTREE_TRESHOLD;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OCTREE_H__FC2A5101_AB9A_11D1_B467_444553540000__INCLUDED_)

