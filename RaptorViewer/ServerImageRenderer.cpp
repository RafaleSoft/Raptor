#include "stdafx.h"

#if !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)
	#include "ServerImageRenderer.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif


CServerImageRenderer::CServerImageRenderer(size_t width,size_t height)
	:CRectangle(),
	m_pImage(NULL),m_serverWidth(width),m_serverHeight(height)
{
	setName("SERVER_IMAGE");
	//removeModel(CGeometry::CGL_NORMALS);
	
	setDimensions(2.0f, 2.0f);
	translate(0.0f, 0.0f, -10.0f);

	glLockData();
	updateBBox();
	glUnLockData();
}

CServerImageRenderer::~CServerImageRenderer(void)
{
}

bool CServerImageRenderer::hasImagesAvailable(void) const
{
	return (m_pImageDatas.size() > 0);
}

void CServerImageRenderer::glClipRender(void)
{
	glRender();
}

void CServerImageRenderer::glUpdateImage(void)
{
	CRaptorNetwork::IMAGE_COMMAND *image = NULL;

	CRaptorLock lock(tMutex);
	if (m_pImageDatas.size())
	{
		image = m_pImageDatas[0];
		m_pImageDatas.erase(m_pImageDatas.begin());
	}

	if (image != NULL)
	{
		m_pImage->glvkRender();
		GLuint mode = GL_RGB;
		glTexSubImage2D(	GL_TEXTURE_2D,
							0,  // level
							image->header.xOffset,
							image->header.yOffset,
							image->header.blocWidth,
							image->header.blocHeight,
							mode,
							GL_UNSIGNED_BYTE,
							&image->pData);
		
		//!	This bloc has been initially allocated as a byte array
		//!	by the network client. Deletion must match.
		delete[] image;
	}
}

void CServerImageRenderer::setImageData(CRaptorNetwork::IMAGE_COMMAND *in)
{
	if (NULL == in)
		return;

	CRaptorLock lock(tMutex);

	std::cout << "Set image data ..." << std::endl;

	/*
	size_t size = 4 * in->header.blocWidth * in->header.blocHeight;

	CImage img;
	img.allocatePixels(in->header.blocWidth, in->header.blocHeight);
	unsigned char* px = img.getPixels();
	unsigned char *src = (unsigned char*)&in->pData;

	int pos = 0;
	for (size_t i = 0; i<size; i += 4, pos += 3)
	{
		px[i + 0] = src[pos + 0];
		px[i + 1] = src[pos + 1];
		px[i + 2] = src[pos + 2];
		px[i + 3] = 255;
	}

	CImage::IImageIO *io = img.getImageKindIO("jpg");
	io->storeImageFile("grab.jpg", &img);
	*/
	
	m_pImageDatas.push_back(in);
}

void CServerImageRenderer::glInitImage()
{
	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	m_pImage = factory.glCreateTexture(	ITextureObject::CGL_COLOR24, //_ALPHA,
										ITextureObject::CGL_BILINEAR);

	factory.glResizeTexture(m_pImage,m_serverWidth,m_serverHeight);

	CShader *pShader = getShader();
	CTextureUnitSetup *pTMU = pShader->glGetTextureUnitsSetup();
	pTMU->setDiffuseMap(m_pImage, CTextureUnitSetup::CGL_OPAQUE);
}
