#include "StdAfx.h"

#include "GLHierarchy/Shader.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/PerlinNoise.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/ITextureObject.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureFactoryConfig.h"

#include "Reflection.h"
#include "ToolBox/Imaging/BumpmapLoader.h"

CReflection::CReflection(float width,float height,int hcels,int vcels)
    :CBasicObjects::CRectMesh()
{
	props = CRaptorDisplay::GetCurrentDisplay()->createRenderingProperties();
	props->setBlending(IRenderingProperties::ENABLE);
	props->setLighting(IRenderingProperties::ENABLE);
	props->setTexturing(IRenderingProperties::ENABLE);

	setDimensions(width,height,hcels,vcels);
	const CGeometryEditor& pEditor = getEditor();
	pEditor.scaleTexCoords(10.0f,5.0f);

    CShader *pShader = getShader();
    CMaterial *pMaterial = pShader->getMaterial();
    pMaterial->setAmbient(0.1f , 0.1f , 0.1f , 0.7f);
    pMaterial->setDiffuse(0.5f , 0.7f , 0.9f , 0.7f);
    pMaterial->setSpecular(1.0f , 1.0f , 1.0f , 0.7f);
    pMaterial->setShininess(10.0f);

	CTextureFactory &f = CTextureFactory::glGetDefaultFactory();
    CTextureFactoryConfig &config = f.getConfig();
    config.setBumpAmplitude(3.0f);
    config.setCurrentAnisotropy(16.0f);

	CBumpmapLoader loader(f.getConfig().getBumpAmplitude());
	CPerlinNoise pNoise(&loader);
    ITextureObject *T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
											ITextureObject::CGL_TRILINEAR);
	T->glvkUpdateClamping(ITextureObject::CGL_REPEAT);

	CTextureUnitSetup *tus = pShader->glGetTextureUnitsSetup();

    T->setSize(512,512);
    pNoise.glGenerate(T,0,0,512,512);
    tus->setNormalMap(T, CTextureUnitSetup::CGL_OPAQUE);


    T = f.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
							ITextureObject::CGL_BILINEAR);
    f.glSetTransparency(T, 230);

    f.glLoadTexture(T,"Datas\\marble5.jpg");
    tus->setDiffuseMap(T, CTextureUnitSetup::CGL_MULTIPLY);

	setRenderingModel(CGeometry::CGL_FRONT_GEOMETRY);
	addModel(CGeometry::CGL_NORMALS);
	addModel(CGeometry::CGL_TEXTURE);
}

CReflection::~CReflection(void)
{
}

void CReflection::glRender()
{
    props->glPushProperties();
	glColor4f(1.0,0.0,0.0,1.0);

	CShadedGeometry::glRender();

    props->glPopProperties();
}