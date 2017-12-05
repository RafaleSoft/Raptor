// SimpleLightObserver.h: interface for the CSimpleLightObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLELIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_)
#define AFX_SIMPLELIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)
    #include "GLHierarchy/LightObserver.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
    #include "GLHierarchy/ObjectReference.h"
#endif

RAPTOR_NAMESPACE_BEGIN

//! This class implements a multiple light observer suitable for bumping.
class CSimpleLightObserver : public CLightObserver, public CObjectReference
{
public:
	CSimpleLightObserver() {};

    //! Implements @see CLightObserver update method.
    //! For bumping, this observer only retains active lights, ignoring all other events.
    virtual void update(CLight *light,CLightObserver::UPDATE_KIND kind)
	{	};

private:
	virtual ~CSimpleLightObserver() {};
};


RAPTOR_NAMESPACE_END


#endif // !defined(AFX_SIMPLELIGHTOBSERVER_H__238FC166_A3BC_4D77_8FD4_0A42DB45280F__INCLUDED_)

