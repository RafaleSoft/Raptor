// Projector.h: interface for the CProjector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
#define AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorDisplay;
class ITextureObject;


class RAPTOR_API CProjector : public CPersistence
{
public:
	CProjector(const std::string& name="PROJECTOR");
	virtual ~CProjector();

    //! Renders the projection on TMU. Current projector is updated.
    //! Objects that receive projection must not alter their TMU setup configured with CGL_TEXTURE_GEN_PROJECTION
    virtual void glRender(void);

    //! Toggles projection on/off,  keeping the current projector state.
    //! glActivate is not sufficient for rendering, glRender needs 
	//! to be called explicitely.
    virtual void glActivate(bool activate);

    //! Defines the projector's position
    void setPosition(const CGenericVector<float>& position);

    //! Defines the projector's direction
    void setDirection(const CGenericVector<float>& direction);

	//! Configures the projection with both position & direction
	void setProjector(	const CGenericVector<float>& position,
						const CGenericVector<float>& direction);

    //!	Specifie the projection texture of the projector.
	void glSetProjection(ITextureObject* T);

	//!	Specifies the projector cutoff in degrees.
	//! (from the cutoff of the holding light)
	void setCutOff(float cutoff);

	//! Set the distance at which the projection size is 
	//!	a unit sized square
	void setUnitDistance(float d);

	//! Inherited from CPersistence
	DECLARE_CLASS_ID(CProjectorClassID,"Projector",CPersistence)

	//!	For debugging purpose only
	void glRenderCone(float d);

private:
	float					m_cutoff;
	float					m_unitDistance;
    CGenericVector<float>	m_position;
    CGenericVector<float>	m_direction;

    GL_COORD_VERTEX	m_Sprojection;
	GL_COORD_VERTEX	m_Tprojection;
	GL_COORD_VERTEX	m_Qprojection;

    CReference<ITextureObject> m_pProjection;

    void solve();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)

