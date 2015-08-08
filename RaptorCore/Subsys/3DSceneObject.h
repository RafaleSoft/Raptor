// 3DSceneObject.h: interface for the C3DSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
#define AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_LIGHTATTRIBUTES_H__B0A3AF95_90DC_4185_9747_B7F631DDB2BF__INCLUDED_)
	#include "LightAttributes.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CLight;



class C3DSceneObject  
{
public:
    //! Max number of supported rendering passes
	enum { NB_PASSES = 8 };

    //! An enum that defines a kind for a rendering pass
    typedef enum
    {
        DEPTH_PASS,
        AMBIENT_PASS,
        LIGHT_PASS,
        FULL_PASS,
    } PASS_KIND;

public:
	C3DSceneObject();
	virtual ~C3DSceneObject();

    //! this method renders the real object using the previous occlusion query
    //! Up to NB_PASSES are managed.
    bool glRenderPass(	unsigned int passNumber,
						const vector<CLight*> &lights,
						GLboolean proceedLights);

    //! This method chooses the appropriate light for self object rendering.
	//! The lights selected are the first and less then CLightAttributes::MAX_LIGHTS lights
	//! that contribute to shading taking into account attenuation (> 1/256 rgba)
    void selectLights(const vector<CLight*> &lights,const CGenericMatrix<float>& transform);

    //! This method renders the selected lights.
    //! The list in parameter is the list of all lights that will be switched on or off, depending on the 
    //! list of effective lights for this object.
    void glRenderLights(const vector<CLight*> &lights);

		//! This method renders initial occlusion bbox 
	void glRenderBBoxOcclusion(unsigned int passNumber);

	//! This method computes initial occlusion using bbox rendering
	static void glComputeBBoxOcclusion(	unsigned int passNumber,
										const vector<C3DSceneObject*> &occluded);

	//!	Specific object ordering
	struct zorder
	{
		bool operator()(C3DSceneObject* const x, C3DSceneObject* const y) const
		{
			if (x->z_order == y->z_order)
				return x->object.handle < y->object.handle;
			else
				return (x->z_order < y->z_order); 
		};
	};


    //!	Occlusion queries
	unsigned int	visibilityQuery[NB_PASSES];
	int				passVisibility[NB_PASSES];

    //!	The 'real' embedded object
	RAPTOR_HANDLE	object;

    
	//!	storage for z_ordering	( need multipass ? )
	float			z_order;
    float			z_span;

	CLight          *effectiveLights[CLightAttributes::MAX_LIGHTS];

    static PASS_KIND	m_currentPass;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)

