/***************************************************************************/
/*                                                                         */
/*  BumppedGeometry.h                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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
class CEMBMShader;
class ITextureObject;


class RAPTOR_API CBumppedGeometry : public CGeometry  
{
public:
	//!	Default constructor.
	CBumppedGeometry(const std::string& name = "BUMPPED GEOMETRY");

	//!	Destructor.
	virtual ~CBumppedGeometry();

	//!	Rendering ( see base class )
	//!	Rendering is always done using the current rendering model.
	virtual void glRender();

	//!	Specific additional bump rendering properties
	//!	to the CGeometry rendering model ( CGeometry::SetRenderingModel
	//!	is called from this method, so only one set is necessary )
	virtual void setRenderingModel(CGeometry::RENDERING_MODEL model);

	virtual CShader	* const getShader(void) const;

	//!	Sets the texture map that will be used as the diffuse component of the bumpping.
	void setDiffuseMap(ITextureObject* diffuse);

    //! Same as above with normal
	void setNormalMap(ITextureObject* normal);

     //!	Sets the texture map that will be used as the environment component of the bumpping.
    void setEnvironmentMap(ITextureObject* environment);

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
    //! Specific constructor for derived classes
    CBumppedGeometry(	const std::string& name, 
						const CPersistence::CPersistenceClassID &classID);

	//!	Implements CPersistence
	virtual void unLink(const CPersistence* p);

	//!	Texture setups
	CReference<ITextureObject>	diffuseMap;
	CReference<ITextureObject>	normalMap;
	CReference<ITextureObject>	envMap;

	//! Shaders for various light configurations.
	CEMBMShader* m_pBumpShader;

private:
	//!	Initialize shaders and observers
	virtual void init(void);
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_BUMPPEDGEOMETRY_H__FDCE89B9_B923_4325_AB0D_A12486C6756D__INCLUDED_)

