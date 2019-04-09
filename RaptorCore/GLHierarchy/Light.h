// Light.h: interface for the CLight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
#define AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_MATERIAL_H__B42ABB88_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Material.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CLightAttributes;
class ITextureObject;
class CObject3D;
class CProjector;
class CLightGlow;


class RAPTOR_API CLight : public CMaterial
{
public:
	CLight(const std::string& name = "GL_LIGHT");
	virtual ~CLight();


    //!
	//!	Rendering
    //!

    //! Base rendering. ( It must be called if any parameter has changed to
    //! be handled properly by GL and by the engine )
    virtual void glRender(void);


    //! Renders the 'lens flare' of the light. 
    //! If no flares are defined, this method can be used to compute light's volume visibility.
	//virtual void glRenderFlare(void);

    //! Renders the 'lens glow' of the light. 
	//virtual void glRenderGlow(void);

	//!	Renders the light glow and lens flare of this light.
	void glRenderEffects(void);


    //! Renders the BBox of the light's lighted zone ( if not infinite )
    void glRenderLightBBox(void);

	//!	Switch on the light. ( the light is mapped to hardware, T&L uses this light )
	void glActivate(bool spot = false);

    //!	Switch on the light. ( the light is unmapped thus releasing a resource for
    //! any other light if needed )
	void glDeActivate(void);

    //! Returns true if light is active ( understood as mapped to hardware )
	bool isActive(void);

	//!
	//!	Common Light parameters
    //!

    //! Defines light's position, given in current viewpoint coordinates.
    //! Internal light's parameter are managed in eye space coordinates.
    //! Thus, to apply light's position properly, user must call Render here under.
    //! Light's rendering must be called early in scene rendering to avoid beeing moved
    //! along with objects' transform, otherwise, be sure current viewpoint is not modified.
	void RAPTOR_FASTCALL setLightPosition(const GL_COORD_VERTEX& position);

	//! Returns 'absolute' light position set here above
	GL_COORD_VERTEX RAPTOR_FASTCALL getLightPosition(void) const;

	//! Returns the light's position in eye space : before clip projection.
	const CGenericVector<float>& RAPTOR_FASTCALL getLightEyePosition(void) const;

	//! Returns the light's position in eye clip space : projected in unit cube.
	GL_COORD_VERTEX RAPTOR_FASTCALL getLightClipPosition(void) const;


    //! This method computes light's projection. Raptor scene will call this method only once per frame.
    //! Without a scene, this method must be called as soon as possible after the position is set
    //! because eye position as well as clip space position will be computed. Those values
    //! are fundamental for shadows or pixel lighting calculations.
    //! @param grab: if true, positions are computed using GL current matrices. if false, Raptor engine is used.
    void RAPTOR_FASTCALL glProject(bool grab = false);

    //! Sets light direction for spot lights or directional lighting used in shadows,
    //! no matter in which coordinate system, it is a vector
	void RAPTOR_FASTCALL setLightDirection(const GL_COORD_VERTEX& direction);

    //! Returns light direction set here above
	GL_COORD_VERTEX RAPTOR_FASTCALL getLightDirection(void) const;


	//!
	//!	Spot/Directional Light parameters
    //!

    //! Defines spot cut-off angle.
	void RAPTOR_FASTCALL setCutOff(float cutoff);

    //! Defines the attenuation of light as a function of the distance
	void RAPTOR_FASTCALL setAttenuation(float quadratic,float linear, float constant);

    //! This method defines a value in [0.0 .. 1.0] which is the minimum attenuation value
    //! that allows an object to be lighted. Below this value, the object will be considered
    //! out of reach from the light. This value directly affects GetLightDMax() result.
    void RAPTOR_FASTCALL setSpotTreshold(float t);

    //! Returns spot parameters:
    //! - result.x = quadratic term of attenuation
    //! - result.y = linear term of  attenuation
    //! - result.z = constant term of attenuation
    //! - result.h = cut-off angle
	GL_COORD_VERTEX RAPTOR_FASTCALL getSpotParams(void) const;

    //! Returns the distance at wich light's attenuation is equivalent to no lighting
    float RAPTOR_FASTCALL getLightDMax(void) const;

    //! Returns the relative light attenuation at a given position.
    //! @param atPosition : where intensity is requested, in eye space coordinates
    //! @return attenuation factor in [0..1]
	float getLightAttenuation(CGenericVector<float> atPosition) const;

    //!	Specifie the lens flare texture of the light
	//!	Use size parameter to minify/magnify projection size
	void addLensFlare(ITextureObject* T,float size);

    //! Specifies the glow texture of the light.
    void setGlow(CLightGlow *T);

	//! Specifies the texture projector of the spot light.
    void setProjector(CProjector *P);

    //! Define the light 'volume' which can be assimilated to the theorical volume emitting  the light.
    //! The volume size is used to attenuate light using the visible percentage of the volume
    void setLightVolume(float size);

    //! Returns the light volume visibility. This value is relevent only if glRenderFlare is called
    //! ( it is called by C3DScene ). 
    //! The returned value is the visibility percentage in O.O .. 1.0
    float getLightVisibility(void) const;

	//!	Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CLightClassID,"Light",CMaterial)


private:
	//! Inherited to handle occlusion queries display resources
    virtual void unLink(const CPersistence*);

	CLightAttributes	*m_pAttributes;
	CProjector			*m_pProjector;
	CLightGlow			*m_pGlow;

    bool importSpotParams(CRaptorIO& io);
    bool importFlare(CRaptorIO& io);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)

