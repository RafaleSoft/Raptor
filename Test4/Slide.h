#pragma once

#include "GLHierarchy/SimpleObject.h"

RAPTOR_NAMESPACE_BEGIN
class CMaterial;
class ITextureObject;
class CShadedGeometry;
RAPTOR_NAMESPACE_END


class CPathObject;

RAPTOR_NAMESPACE


class CSlide : public CSimpleObject
{
public:
    CSlide(const string &,CPathObject &);
    virtual ~CSlide(void);

	// Returns the slide position along its 3DPath
	float getPos(void) const { return tpos; };

	// Implements CSimpleObect
    virtual void glRender(void);

	// Translates a slide along its 3DPath
    void translate(float dt);

	// Highlights a slide
    void highLight(bool highlight) { m_bHighLight = highlight; };

	// Defines a video as the background of the slide
    void setVideo(const string& vName);

private:
    float				tpos;
	float				tw;
	float				th;
    CPathObject			&mPath;
    ITextureObject		*mVideo;
    unsigned int        mStream;

    static bool			m_bHighLight;
    static CMaterial	*m_pMaterial;
    static CMaterial	*m_pMaterial2;
    static CMaterial	*m_pMaterial3;
    static CShadedGeometry  *m_pSlide;
};
