#pragma once


#include "System/RenderEntryPoint.h"
#include "Raptordll.h"

RAPTOR_NAMESPACE

class CTerrain;
class CSOARDoc : public CRenderEntryPoint
{
public:
	CSOARDoc(const RAPTOR_HANDLE&,CRaptorDisplay*);
	virtual ~CSOARDoc(void);

	virtual	void GLInitContext(int argc, char* argv[]);

    virtual void glRender();

private:
	CRaptorDisplay *m_pDisplay;
    RAPTOR_HANDLE m_wnd;
	
	CTerrain	*m_pTerrain;
	CTerrain	*m_pTerrain2;
	CTerrain	*m_pTerrain3;
	CTerrain	*m_pTerrain4;
};
