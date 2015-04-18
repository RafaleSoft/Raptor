#if !defined(AFX_TESTDOC_H__39546B30_247F_403A_9C99_72019F779844__INCLUDED_)
#define AFX_TESTDOC_H__39546B30_247F_403A_9C99_72019F779844__INCLUDED_

#pragma once

#include "System/RenderEntryPoint.h"
#include "Raptordll.h"

RAPTOR_NAMESPACE


class CTest5Doc : public CRenderEntryPoint
{
public:
	CTest5Doc(const RAPTOR_HANDLE&,const char* title);
    virtual ~CTest5Doc();

	virtual	void GLInitContext(void);

    virtual void glRender();

private:
	CRaptorDisplay*	m_pDisplay;
	CRaptorDisplay*	m_pDisplayBuffer;
	CTextureObject*	m_pBufferTexture;
	CTextureObject*	m_pTexture;

	RAPTOR_HANDLE	m_device;
};

#endif // !defined(AFX_TESTDOC_H__39546B30_247F_403A_9C99_72019F779844__INCLUDED_)

