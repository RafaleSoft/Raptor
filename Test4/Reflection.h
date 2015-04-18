#pragma once

#include "GLHierarchy/BumppedGeometry.h"
#include "Toolbox/BasicObjects.h"

RAPTOR_NAMESPACE

class CReflection : public CBasicObjects::CRectMesh// CBumppedGeometry
{
public:
    CReflection(float width,float height,int hcels,int vcels);
    virtual ~CReflection(void);

    virtual void glRender();

private:
};
