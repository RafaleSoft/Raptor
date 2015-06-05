// VertexShadersDisplay.h: interface for the CVertexShadersDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERTEXSHADERSDISPLAY_H__5039FA06_AF60_4E40_8C6D_4D454353B263__INCLUDED_)
#define AFX_VERTEXSHADERSDISPLAY_H__5039FA06_AF60_4E40_8C6D_4D454353B263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

class ShaderModifier;
class SeeGround;
class CTextureWaves;
class CSky;

class CVertexShadersDisplay : public CGenericDisplay
{
public:
	CVertexShadersDisplay();
	virtual ~CVertexShadersDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Vertex shaders"; };

	void switchBump() { showBump = !showBump; };

private:
	virtual void ReInit();

    virtual void UnInit();

	CObject3D		*see;
	bool			showBump;

	C3DScene		*pScene;
	ShaderModifier	*shaderModifier;
	CLight			*pLight;
	SeeGround		*ground;
	CTextureWaves	*tw;
	CSky			*sky;
};

#endif // !defined(AFX_VERTEXSHADERSDISPLAY_H__5039FA06_AF60_4E40_8C6D_4D454353B263__INCLUDED_)
