// BumppedGeometry.h: interface for the CBumppedGeometry class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)
#define AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Geometry.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CLight;
class CShader;
class CBumpLightObserver;
class CTextureObject;


class RAPTOR_API CBumppedGeometry : public CGeometry  
{
public:
	CBumppedGeometry(const std::string& name = "BUMPPED GEOMETRY");
	virtual ~CBumppedGeometry();

	//!	Rendering ( see base class )
	//!	Rendering is always done using the current rendering model.
	virtual void glRender();

	//!	Specific additional bump rendering properties
	//!	to the CGeometry rendering model ( CGeometry::SetRenderingModel
	//!	is called from this method, so only one set is necessary )
	virtual void setRenderingModel(const CRenderingModel& model);

	virtual CShader	* const getShader(void) const { return m_pBumpShader; }

	//!	Sets the texture map that will be used as the diffuse component of the bumpping.
	void setDiffuseMap(CTextureObject* diffuse);

    //! Same as above with normal
	void setNormalMap(CTextureObject* normal);

     //!	Sets the texture map that will be used as the environment component of the bumpping.
    void setEnvironmentMap(CTextureObject* environment);

	//!	A copy operator.
	//!	The specific bump data is copied, the geometry
	//!	part is exactly the same except normals that are recomputed
	//!	to generate tangents an dbinormals.
	CBumppedGeometry& operator=(const CGeometry &geo);
    //! Same as above .
    CBumppedGeometry& operator=(const CBumppedGeometry &geo);

	//!	Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CBumppedGeometryClassID,"BumppedGeometry",CGeometry)


protected:
	//!	Builds the correct Vertex Shader.
	//!	This method must derived for specific vertex shader construction
	virtual void glBuildShader(void);

	//!	Recomputes the bump colors ( dynamic normals ), according light position
	//!	Returns the number of lights that are relevant in the current context.
	virtual unsigned int glUpdateLightPosition(void);

    //! Specific constructor for derived classes
    CBumppedGeometry(	const std::string& name, 
						const CPersistence::CPersistenceClassID &classID);

	//!	Texture setups
	CReference<CTextureObject>	diffuseMap;
	CReference<CTextureObject>	normalMap;
	CReference<CTextureObject>	envMap;

	//! Shaders for various light configurations.
	CShader*			m_pBumpShader;


private:
	//!	Implements CPersistence
	void unLink(const CPersistence* p);

	//!	Initialize shaders and observers
	void init(void);

	//!	Configure internal shader texture unit setup
	virtual void glSetTextureUnits(void);

    void glRenderEMBM();

    //!	A light observer to be notified from lightupdates.
	static CBumpLightObserver	*m_pObserver;

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

    CGenericVector<float>	X2;
    GL_COORD_VERTEX A2;
	CColor::RGBA	S2;

	CGenericVector<float>	X3;
    GL_COORD_VERTEX A3;
	CColor::RGBA	S3;


    //! Shaders for various light configurations.
	static CShader*		m_pBumpShader2Lights;
    static CShader*		m_pBumpShaderAmbient;
	static CShader*		m_pBumpShader3Lights;

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)

