// ProjectionDisplay.h: interface for the CProjectionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTIONDISPLAY_H__DF4C8A0E_432C_416B_A5B0_814324B0E02D__INCLUDED_)
#define AFX_PROJECTIONDISPLAY_H__DF4C8A0E_432C_416B_A5B0_814324B0E02D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"


class CProjectionDisplay : public CGenericDisplay
{
public:
	CProjectionDisplay();
	virtual ~CProjectionDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Projection demo [press C for caustics]"; };

	void	switchCaustics() { caustics = !caustics; };

private:
	virtual void ReInit();

    virtual void UnInit();

	bool		caustics;
	int			tindex;
	float		dt;

	CTextureSet	*texture;
	CObject3DInstance	*m_pBall;
	CObject3DInstance	*m_pPlane;

	CLight			*m_light;
	CTextureSet		*m_caustics;
    CProjector		*m_projector;
	CShader			*m_shader;
    ITextureObject	*cube;

	// Projection scene
	C3DScene *m_pScene;
};

#endif // !defined(AFX_PROJECTIONDISPLAY_H__DF4C8A0E_432C_416B_A5B0_814324B0E02D__INCLUDED_)
