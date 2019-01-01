#pragma once

#include "GLHierarchy/SimpleObject.h"

RAPTOR_NAMESPACE_BEGIN
class C3DPath;
RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

class CPathObject : public CSimpleObject
{
public:
    CPathObject(void);
    virtual ~CPathObject(void);

    virtual void glRender(void);

    GL_COORD_VERTEX getTPos(float t);

private:
    C3DPath      *m_pPath;
};
