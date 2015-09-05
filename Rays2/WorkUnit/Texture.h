// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_)
#define AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTexture  
{
public:
	CTexture();
	virtual ~CTexture();

	void GetTexture(unsigned int &w,unsigned int &h,unsigned char *&data);
	void SetTexture(unsigned int w,unsigned int h,unsigned char *data);

	CWVector& GetMapColor(const CGenericVector<float> &texel);

private:

	unsigned int	m_width;
	unsigned int	m_height;
	unsigned int	m_row;

	unsigned char	*m_map;

	CWVector		tmpColor;


};

#endif // !defined(AFX_TEXTURE_H__86A35E18_AD72_4A70_B7A8_2B547F23E591__INCLUDED_)
