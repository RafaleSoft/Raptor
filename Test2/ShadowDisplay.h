// ShadowDisplay.h: interface for the CShadowDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWDISPLAY_H__A671A014_FAAD_4435_8EAC_BCB85DAF134F__INCLUDED_)
#define AFX_SHADOWDISPLAY_H__A671A014_FAAD_4435_8EAC_BCB85DAF134F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GenericDisplay.h"

class CShadowDisplay : public CGenericDisplay
{
public:
	CShadowDisplay();
	virtual ~CShadowDisplay();

	virtual void Init();

	virtual void Display();

	virtual const char* getTitle(void) const
	{ return "Dynamic shadow volumes"; };

private:
	CTextureSet		*texture;
	CMaterial		*material;

	C3DSet			*sponge;
	CObject3DShadow	*contour;
	CLight			*m_light;
	CGLFont			*font;
	CShadedGeometry	*fulltext;
    CShadedGeometry	*knot;
    C3DScene		*m_pScene;
	CViewPoint		*m_pVP;

	float			dt;

	virtual void ReInit();

    virtual void UnInit();
};

#endif // !defined(AFX_SHADOWDISPLAY_H__A671A014_FAAD_4435_8EAC_BCB85DAF134F__INCLUDED_)
