// VideoStream.cpp: implementation of the CVideoStream class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_VIDEOSTREAM_H__F651DAEA_2E78_45B6_B06E_BBAD563283E5__INCLUDED_)
    #include "VideoStream.h"
#endif
#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
    #include "AnimatorStream.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoStream::CVideoStream(CAnimatorStream *pStream,unsigned int nStream)
    :m_pStream(pStream),m_nStream(nStream),m_streamPos(~0)
{

}

CVideoStream::~CVideoStream()
{

}

void CVideoStream::glGenerate(CTextureObject* T)
{
    if ((T == NULL) || (!m_bEnabled))
        return;

    unsigned int pos = m_pStream->getFramePosition(m_nStream);
    if (m_streamPos == pos)
        return;
    else
        m_streamPos = pos;

    unsigned int width;
    unsigned int height;
    bool bFrame = m_pStream->getFrameSizes(m_nStream,width,height);
    const unsigned char* pixels = m_pStream->glGetStreamBuffer(m_nStream);

    if ((bFrame) && (pixels != NULL))
    {
        //  Initialize texture for subloading if it has not been done yet.
        if ((T->getWidth() == 0) || (T->getHeight() == 0))
        {
            m_pStream->glReleaseStreamBuffer(m_nStream);

			CTextureFactory &factory = CTextureFactory::getDefaultFactory();
            CTextureFactoryConfig& rConfig = factory.getConfig();

            bool previousResize = rConfig.useTextureResize();
            rConfig.useTextureResize(false);

            T->setSize(width,height);
			CImage buffer;
			buffer.allocatePixels(width, height);
			unsigned int *tbuf = (unsigned int*)buffer.getPixels();

            for (unsigned int i=0;i<height*width;i++)
		        tbuf[i] = 0xFFFFFFFF;

			factory.glLoadTexture(T, buffer);

            rConfig.useTextureResize(previousResize);

            pixels = m_pStream->glGetStreamBuffer(m_nStream);
        }

#ifdef GL_EXT_bgra
        GLuint mode = GL_BGR_EXT;
#else
        GLuint mode = GL_RGBA;
#endif
        glTexSubImage2D(GL_TEXTURE_2D,
					    0,  // level
					    0,  // xoffset
					    0,  // yoffset
					    width,
					    height,
					    mode,
					    GL_UNSIGNED_BYTE,
					    pixels);
    }

    m_pStream->glReleaseStreamBuffer(m_nStream);

    CATCH_GL_ERROR
}


unsigned int CVideoStream::getGenerateWidth(void) const
{
    unsigned int width;
    unsigned int height;
    m_pStream->getFrameSizes(m_nStream,width,height);

    return width;
}

unsigned int CVideoStream::getGenerateHeight(void) const
{
    unsigned int width;
    unsigned int height;
    m_pStream->getFrameSizes(m_nStream,width,height);

    return height;
}


