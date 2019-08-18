// ShadowMapDisplay.h: interface for the CShadowMapDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWMAPDISPLAY_H__2B12BC0C_31FD_41BD_A51C_EA5FD340F101__INCLUDED_)
#define AFX_SHADOWMAPDISPLAY_H__2B12BC0C_31FD_41BD_A51C_EA5FD340F101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

class CShadowMapDisplay : public CGenericDisplay
{
public:
	CShadowMapDisplay();
	virtual ~CShadowMapDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Shadow mapping"; };


private:
	virtual void	ReInit();

    virtual void UnInit();

	CShadedGeometry	*redBall;
	CShadedGeometry	*greenBall;
	CLight		*m_light;
	CTextureSet	*texture;
	CMaterial	*redMat;
	CMaterial	*greenMat;

	IViewPoint			*vp;

	CFragmentShader		*pfsSetup;
	CVertexProgram_old  *pvsSetup;

	C3DScene			*m_pScene;
	CObject3DInstance	**m_pInstances;
	CSimpleObject		*pSimpleObject;
	CSimpleObject		*pSimpleObject2;
	
	float dt;

	// combiners setup for shadow maps rendering
	RAPTOR_HANDLE c1,c2;
};

#endif // !defined(AFX_SHADOWMAPDISPLAY_H__2B12BC0C_31FD_41BD_A51C_EA5FD340F101__INCLUDED_)
