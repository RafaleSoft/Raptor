// Object3DShadowAttributes.h: interface for the CObject3DShadowAttributes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)
#define AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_LIGHTOBSERVER_H__D6E2195C_21B4_42FB_ABA5_4219EECFF076__INCLUDED_)
	#include "GLHierarchy/LightObserver.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CObject3DShadowAttributes// : public CLightObserver
{
public:
	CObject3DShadowAttributes();
	virtual ~CObject3DShadowAttributes();

    CLight				*m_pLight;
    CObject3D           *m_pObject;

    //  visibility queries if available
    unsigned int visibilityQuery;
    unsigned int visibilityCount;
    bool    queryIssued;

    //  rebuild shadow object when light change
	bool					reBuild;
    CGenericVector<float>	plane;
    CGenericVector<float>	normal;
    float                   extrusion;

    //  shadow color for contours or planars
	float					m_color_red;
	float					m_color_green;
	float					m_color_blue;
	float					m_color_alpha;
	CGenericVector<float>	m_light;
	CGenericMatrix<float>	m_projection;
	CGenericMatrix<float>	m_lightProjection;

    //  shdowColors are neede per contour.
	vector<unsigned int	*> m_shadowColors;

#if defined(GL_ARB_imaging)
	PFN_GL_BLEND_COLOR_ARB_PROC				glBlendColorARB;
#endif
#if defined(GL_EXT_stencil_two_side)
	PFN_GL_ACTIVE_STENCIL_FACE_EXT_PROC		glActiveStencilFaceEXT;
#endif

    __inline void RAPTOR_FASTCALL glQueryLightPosition(CGenericVector<float> &lpos)
    {
	    if (m_pLight != NULL)
	    {
			glGetTransposeFloatv(GL_MODELVIEW_MATRIX,m_projection);
			lpos = m_inverseProjection.setInverseOf(m_projection) * m_pLight->getLightEyePosition();
	    }
	    else
		    lpos = m_light;
    }
	// Remove dependency to current light:
	// the current light is context dependant and must be set 
	// before any calculation.
/*
    __inline void update(CLight *light,UPDATE_KIND kind)
    {
        m_pLight = light;
        reBuild = true;
    }
*/
private:
	C3DEngineMatrix m_inverseProjection;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_OBJECT3DSHADOWATTRIBUTES_H__4FAFE331_A695_471C_AD48_0F541948AAAD__INCLUDED_)

