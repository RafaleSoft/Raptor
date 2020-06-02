#if !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)
#define AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif


RAPTOR_NAMESPACE


class CServerImageRenderer : public CObject3D
{
public:
	CServerImageRenderer(size_t width,size_t height);
	virtual ~CServerImageRenderer(void);

	bool hasImagesAvailable(void) const;

	void glInitImage();

	void setImageData(unsigned char*);

	DECLARE_OBJECT3D_NOMANIPULATORS

private:
	CServerImageRenderer();
	virtual void glClipRender(void);
	
	CRaptorMutex	tMutex;

	RAPTOR_HANDLE	drawBuffer;
	ITextureObject	*m_pImage;
	unsigned int	m_serverWidth;
	unsigned int	m_serverHeight;

	vector<unsigned char*> m_pImageDatas;
	
};


#endif // !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)

