// 3DSetItem.cpp: implementation of the C3DSetItem class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DSETITEM_H__3AA83803_A8B2_457E_A80F_718D6678D740__INCLUDED_)
    #include "3DSetItem.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSetItem::C3DSetItem(CObject3D *pObject)
    :m_pBefore(NULL),m_pAfter(NULL),m_pFather(NULL),
    m_pLastChild(NULL),m_pFirstChild(NULL)
{
    m_pObject = pObject;
}

C3DSetItem::~C3DSetItem()
{
    if (m_pAfter!=NULL)
		m_pAfter->m_pBefore = m_pBefore;

	if (m_pBefore!=NULL)
		m_pBefore->m_pAfter = m_pAfter;

    if (m_pFather != NULL)
    {
        if (m_pFather->m_pFirstChild == this)
            m_pFather->m_pFirstChild = m_pAfter;
        if (m_pFather->m_pFirstChild == NULL)
            m_pFather->m_pLastChild = NULL;
    }

    C3DSetItem *it = m_pFirstChild;
    while (it != NULL)
    {
        C3DSetItem *tmp = it->m_pAfter;
        delete it;
        it = tmp;
    }
}


CObject3D* C3DSetItem::removeFirstChild()
{
	C3DSetItem *res = m_pFirstChild;

	if (m_pFirstChild != NULL)
	{
		m_pFirstChild = m_pFirstChild->m_pAfter;
		if (m_pFirstChild == NULL)
			m_pLastChild = NULL;
	}

    CObject3D *o = NULL;
	if (NULL != res)
	{
		o = res->getObject();
		delete res;
	}
    
	return o;
}

void C3DSetItem::addChild(C3DSetItem *pChild)
{
    if (m_pFirstChild == NULL)
    {
        m_pFirstChild = pChild;
        m_pLastChild = pChild;
        pChild->m_pFather = this;
    }
    else
    {
        m_pLastChild->m_pAfter = pChild;
        pChild->m_pFather = this;
        pChild->m_pBefore = m_pLastChild;

        m_pLastChild = pChild;
    }
}

C3DSetItem* const C3DSetItem::getNextChild(void) const
{
    if (m_pFirstChild != NULL)
        return m_pFirstChild;
    else if (m_pAfter != NULL)
        return m_pAfter;
    else if (m_pFather != NULL)
        return m_pFather->m_pAfter;
    else
        return NULL;
}

C3DSetItem* const C3DSetItem::getPreviousChild(void) const
{
    if (m_pLastChild != NULL)
        return m_pLastChild;
    else if (m_pBefore != NULL)
        return m_pBefore;
    else if (m_pFather != NULL)
        return m_pFather->m_pBefore;
    else
        return NULL;
}


