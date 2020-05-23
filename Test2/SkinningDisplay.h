// SkinningDisplay.h: interface for the CSkinningDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINNINGDISPLAY_H__BCAD90AE_7E66_475E_81EB_BAA928072EA9__INCLUDED_)
#define AFX_SKINNINGDISPLAY_H__BCAD90AE_7E66_475E_81EB_BAA928072EA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericDisplay.h"

#include "GLHierarchy/ProgramParameters.h"


class CSkinningDisplay : public CGenericDisplay
{
public:
	CSkinningDisplay();
	virtual ~CSkinningDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Vertex skinning demo"; };


private:
	//	vertex skinning
    CShadedGeometry	*tube;
	CShadedGeometry	*tube2;
	CImageModifier	*modifier;
	CTextureSet		*texture;
	CGLLayer		*layer;

    CShader *skinning;
	CProgramParameters::CParameter<GL_MATRIX> skinningMatrix;
    CProgramParameters	params;
    CProgramParameters	params2;

	CTextureObject		*t;
	ITextureObject		*t2;

	virtual void ReInit();

    virtual void UnInit();
};

#endif // !defined(AFX_SKINNINGDISPLAY_H__BCAD90AE_7E66_475E_81EB_BAA928072EA9__INCLUDED_)
