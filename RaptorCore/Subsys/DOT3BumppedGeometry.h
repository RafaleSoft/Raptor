// BumppedGeometry.h: interface for the CBumppedGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)
	#include "GLHierarchy/BumppedGeometry.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CSimpleLightObserver;


class CDOT3BumppedGeometry : public CBumppedGeometry
{
public:
	//!	Default constructor.
	CDOT3BumppedGeometry(const std::string& name);

	//! Destructor.
	virtual ~CDOT3BumppedGeometry();

	//!	Rendering ( see base class )
	//!	Rendering is always done using the current rendering model.
	virtual void glRender();

	//! Same as above .
	CBumppedGeometry& operator=(const CDOT3BumppedGeometry &geo);


private:
	//	Coordinates for shader T&L:
	//	- X is the light position relative to the object.
	//	it is deduced from light's absolute position and the current transform.
	//	- V is the viewer position relative to the object :
	//	if the current transform is the matrix composed with
	//	rotation R and translation T, V = -t(R).T ( t(R) denotes transpose of R
	// - A is the light attenuation factors
	// - S is the light's specular
	GL_COORD_VERTEX	V;

	CGenericVector<float>	X;
	GL_COORD_VERTEX A;
	CColor::RGBA	S;

	//!	Recomputes the bump colors ( dynamic normals ), according light position
	//!	Returns the number of lights that are relevant in the current context.
	virtual unsigned int glUpdateLightPosition(void);

#ifdef RAPTOR_SSE_CODE_GENERATION
	void setLightPositionDOT3SSE(void);
#endif
	void setLightPositionDOT3(void);

	//!	Configure internal shader texture unit setup
	//virtual void setRenderingModel(const CRenderingModel& model);
	virtual void setRenderingModel(CRenderingModel::MODEL model);
	
	//!	A light observer to be notified from lightupdates.
	static CSimpleLightObserver	*m_pObserver;

	//!	Diffuse Light vector coordinates
	unsigned char		*bumpDiffusePx;

	//!	Specular Light vector coordinates
	unsigned char		*bumpSpecularPx;
};

RAPTOR_NAMESPACE_END
