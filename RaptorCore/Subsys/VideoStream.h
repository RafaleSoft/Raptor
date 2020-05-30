// VideoStream.h: interface for the CVideoStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOSTREAM_H__F651DAEA_2E78_45B6_B06E_BBAD563283E5__INCLUDED_)
#define AFX_VIDEOSTREAM_H__F651DAEA_2E78_45B6_B06E_BBAD563283E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
#include "GLHierarchy/TextureGenerator.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CAnimatorStream;


class CVideoStream : public ITextureGenerator  
{
public:
	CVideoStream(CAnimatorStream *pStream,unsigned int nStream);
	virtual ~CVideoStream();

    //! Returns the kind of render target texture generator.
    virtual ITextureGenerator::GENERATOR_KIND    getKind(void) const { return ITextureGenerator::ANIMATED; };

    //! This method must be implemented in subclasses to produce 
    //! an array of texels and load the currently bound texture object.
    //! To get texture parameters, the currently bound texture object is passed in
    virtual void glGenerate(ITextureObject* );

    //! This method returns the width of the generator
    virtual unsigned int getGenerateWidth(void) const;

    //! This method returns the height of the generator
    virtual unsigned int getGenerateHeight(void) const;

private:
    CAnimatorStream *m_pStream;
    unsigned int         m_nStream;
    unsigned int         m_streamPos;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VIDEOSTREAM_H__F651DAEA_2E78_45B6_B06E_BBAD563283E5__INCLUDED_)

