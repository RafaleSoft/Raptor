// AmbientOcclusionShader.h: interface for the CAmbientOcclusionShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)
#define AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#ifndef __SIMD_H__
	#include "Subsys/SimdLib/simd.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;
class CRaptorDisplay;
class CObject3DInstance;

class CAmbientOcclusionShader :	public CShader
{
public:
	CAmbientOcclusionShader(const std::string& name = "AOSHADER");
	virtual ~CAmbientOcclusionShader(void);

	//!	Implement base class: @see CShader.
	virtual void glRender();

	void glRenderResult();

	//!	Initialise all resources to compute Ambient occlusion
	//!	in real time on the GPU
	bool glInitAOCompute(void);

	//!	Initialise the vertex texture map and geometry reference vertices
	bool glSetCoords(GL_COORD_VERTEX* refVertex, unsigned int nbVertex);

	//!	Initialise the vertex texture map and geometry reference vertices
	bool glSetNormals(GL_COORD_VERTEX* refNormal, unsigned int nbVertex);

	//!	Initialise the texture coordinates that will place each vertex
	//!	in the output texture.
	bool glSetTexCoords(GL_TEX_VERTEX* refTexCoords, unsigned int nbVertex);

	//! Initialise date pointer for rendering
	//! (can be relocated)
	bool glSetRefData(GL_COORD_VERTEX* refVertex,GL_COORD_VERTEX* refNormal,GL_TEX_VERTEX* refTexCoords);

	//! Defines occluders list used to compute ambient occlusion.
	void setOccluders(const vector<CAmbientOcclusionShader*>& occluders,
					  const vector<CGenericMatrix<float> >& transforms);

	//! Defines transform provider.
	void setTransformSource(CObject3DInstance* tSource)
	{ m_tranformRef = tSource; }

	//! Retrieve transform provider.
	const CObject3DInstance* getTransformSource(void) const
	{ return m_tranformRef; }


private:
	unsigned int		m_refNbVertex;
	GL_COORD_VERTEX*	m_refVertex;
	GL_COORD_VERTEX*	m_refNormal;
	GL_TEX_VERTEX*		m_refTexCoords;

	CTextureObject*		m_pVertexMap;
	CTextureObject*		m_pNormalMap;
	CTextureObject*		m_pAOMap;
	CRaptorDisplay*		m_pAOBuffer;

	RAPTOR_HANDLE		m_AOMapSetup;
	CShader*			m_pAOcomputeRef;
	CObject3DInstance*	m_tranformRef;

	vector<CGenericMatrix<float> > m_transforms;
	vector<CAmbientOcclusionShader*> m_occluders;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_AMBIENTOCCLUSIONSHADER_H__FA8234C4_82B1_49D3_ABAA_7FCE45EDDCAD__INCLUDED_)

