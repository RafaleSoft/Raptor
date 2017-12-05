// 3DSetItem.h: interface for the C3DSetItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSETITEM_H__3AA83803_A8B2_457E_A80F_718D6678D740__INCLUDED_)
#define AFX_3DSETITEM_H__3AA83803_A8B2_457E_A80F_718D6678D740__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN

class CObject3D;


class C3DSetItem  
{
public:
	C3DSetItem(CObject3D *pObject);

	virtual ~C3DSetItem();

    //! Return the node object
    CObject3D* const getObject(void) const { return m_pObject; }

    //! Removes and delete first child item.
    //! @return : the holded object of the child removed.
    CObject3D* removeFirstChild();

    //! This method add an object after the last child;
    void addChild(C3DSetItem *pChild);

    //! Returns the next child:
    //! - first child if it exist
    //! - after item if it exist
    //! - father's next child.
    //! - NULL : the end of the set is reached.
    C3DSetItem* const getNextChild(void) const;

    //! Returns the previous item, following the same rules as above, but the opposite symetric.
    C3DSetItem* const getPreviousChild(void) const;

    //! Basic accessors
    C3DSetItem* const getBefore(void) const { return m_pBefore; };
    C3DSetItem* const getAfter(void) const { return m_pAfter; };
    C3DSetItem* const getFather(void) const { return m_pFather; };
    C3DSetItem* const getFirstChild(void) const { return m_pFirstChild; };
    C3DSetItem* const getLastChild(void) const { return m_pLastChild; };


private:
    //! Forbidden operators
    C3DSetItem() {};
    C3DSetItem(const C3DSetItem& rsh) {};
    C3DSetItem &operator=(const C3DSetItem& rsh) { return *this; };

    //! the node object
    CObject3D		*m_pObject;

    //! Links to surrouding hierarchy
    C3DSetItem		*m_pBefore;
	C3DSetItem		*m_pAfter;
	C3DSetItem		*m_pFather;

    C3DSetItem		*m_pFirstChild;
    C3DSetItem		*m_pLastChild;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSETITEM_H__3AA83803_A8B2_457E_A80F_718D6678D740__INCLUDED_)

