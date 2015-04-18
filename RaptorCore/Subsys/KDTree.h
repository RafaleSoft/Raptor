// KDTree.h: interface for the KDTree class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_KDTREE_H__2BD065C8_BF3F_4AA1_BD52_C072ADEC3A53__INCLUDED_)
#define AFX_KDTREE_H__2BD065C8_BF3F_4AA1_BD52_C072ADEC3A53__INCLUDED_

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
//! This class implements a standard KDTree 
//! The template parameter enables user to specify a KDTree for a more convenient usage.
//!
template <class USER_DATA_t>
class CKDTree : public CBaseTree<USER_DATA_t>
{
public:
	CKDTree(void);
	virtual ~CKDTree(void);

	//!
    //! Tree manipulators members
    //!
	typedef typename CBaseTree<USER_DATA_t>::TREE_ITEM BASETREE_ITEM;

	//! Inserts an object into the KDTree
    void addObject(CObject3D * const obj, USER_DATA_t userData);

    //! Returns the list of objects from this tree level ( including all childs )
    vector<BASETREE_ITEM> getObjects(void) const;

	//! Returns the list of objects 'viewable' from this KDTree
    vector<BASETREE_ITEM> glClip(bool scanModelview) const;

    //! Constructs the balanced KDTree
    void compress(void);

    //! For debugging purposes only : a simple of the octree bboxes
    void glRender(void);



private:
	typedef struct KdLine_t
	{
		int				idCoup;
		int				Imin;
		int				Imax;
		CBoundingBox	bbox;
	} KdLine;

	void calculParamKd(int *NVC,int *NBI);
	void ConstructionKD(void);
	void medianI(int i1,int i2,int coupure,int dim);

	//! The tree (its root).
	unsigned int	m_blocSize;
	unsigned int	m_nbLevels;
	KdLine			*m_pRoot;

	//! Global bounding box of the tree (all its elements)
	CBoundingBox	bbox;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_KDTREE_H__2BD065C8_BF3F_4AA1_BD52_C072ADEC3A53__INCLUDED_)

