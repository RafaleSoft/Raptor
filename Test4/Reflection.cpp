#include "StdAfx.h"

#include "GLHierarchy/Shader.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/Material.h"
#include "GLHierarchy/Geometry.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/PerlinNoise.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureObject.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/TextureFactoryConfig.h"

#include "Reflection.h"


CReflection::CReflection(float width,float height,int hcels,int vcels)
    :CBasicObjects::CRectMesh()
{
	setDimensions(width,height,hcels,vcels);
	const CGeometryEditor& pEditor = getEditor();
	pEditor.scaleTexCoords(10.0f,5.0f);

    CShader *pShader = getShader();
    CMaterial *pMaterial = pShader->getMaterial();
    pMaterial->setAmbient(0.1f , 0.1f , 0.1f , 0.7f);
    pMaterial->setDiffuse(0.5f , 0.7f , 0.9f , 0.7f);
    pMaterial->setSpecular(1.0f , 1.0f , 1.0f , 0.7f);
    pMaterial->setShininess(10.0f);

	CTextureFactory &f = CTextureFactory::getDefaultFactory();
    CTextureFactoryConfig &config = f.getConfig();
    config.setBumpAmplitude(3.0f);
    config.setCurrentAnisotropy(16.0f);

	CPerlinNoise *pNoise = new CPerlinNoise(CVaArray<CImage::IImageOP::OP_KIND>(CImage::IImageOP::BUMPMAP_LOADER));
    CTextureObject *T = f.glCreateDynamicTexture(	ITextureObject::CGL_COLOR24_ALPHA,
													CTextureObject::CGL_MULTIPLY,
													ITextureObject::CGL_TRILINEAR,
													pNoise);
	T->glvkUpdateClamping(ITextureObject::CGL_REPEAT);

	CTextureUnitSetup *tus = pShader->glGetTextureUnitsSetup();

    T->setSize(512,512);
    pNoise->glGenerate(T);
    tus->setNormalMap(T);

    T = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_MULTIPLY,ITextureObject::CGL_BILINEAR);
    T->glSetTransparency(230);

    f.glLoadTexture(T,"Datas\\marble5.jpg");
    tus->setDiffuseMap(T);

	CGeometry::CRenderingModel model(	CGeometry::CRenderingModel::CGL_FRONT_GEOMETRY |
										CGeometry::CRenderingModel::CGL_NORMALS |
										CGeometry::CRenderingModel::CGL_TEXTURE);
    setRenderingModel(model);
}

CReflection::~CReflection(void)
{
}

void CReflection::glRender()
{
    CRenderingProperties props;
    props.setBlending(CRenderingProperties::ENABLE);
    props.setLighting(CRenderingProperties::ENABLE);
    props.setTexturing(CRenderingProperties::ENABLE);
    props.glPushProperties();
	glColor4f(1.0,0.0,0.0,1.0);

	CShadedGeometry::glRender();

    props.glPopProperties();
}