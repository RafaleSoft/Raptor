// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_)
#define AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __GENERIC_VECTOR_H__
	#include "SimdLib/GenericVector.h"
#endif
#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif
RAPTOR_NAMESPACE


class CTexture  
{
public:
	CTexture();
	virtual ~CTexture();

	void GetTexture(unsigned int &w,unsigned int &h,unsigned char *&data);
	void SetTexture(unsigned int w,unsigned int h,unsigned char *data);

	CColor::RGBA& GetMapColor(const CGenericVector<float> &texel);

private:

	unsigned int	m_width;
	unsigned int	m_height;
	unsigned int	m_row;

	unsigned char	*m_map;

	CColor::RGBA	tmpColor;
};

#endif // !defined(AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_)
