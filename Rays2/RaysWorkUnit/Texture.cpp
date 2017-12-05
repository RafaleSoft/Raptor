// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Subsys/CodeGeneration.h"

#include "Texture.h"
#include "Raytracer.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture()
{
	m_width = 0;
	m_height = 0;
	m_row = 0;
	m_map = NULL;
}

CTexture::~CTexture()
{
	if (m_map != NULL)
		delete [] m_map;
}

void CTexture::GetTexture(unsigned int &w,unsigned int &h,unsigned char *&data)
{
	w = m_width;
	h = m_height;
	data = m_map;
}

void CTexture::SetTexture(unsigned int w,unsigned int h,unsigned char *data)
{
	if (data != NULL)
	{
		if (m_map != NULL)
		{
			delete [] m_map;
			m_map = NULL;
		}

		m_width = w;
		m_height = h;
		m_row = w * 8;

		unsigned char *tmp = new unsigned char[m_row * h];

		if (tmp == NULL)
			std::cout << "Insufficient memory for work unit !!!" << std::endl;
		else
		{
			m_map = tmp;

			__asm
			{
				mov esi,data
				mov edi,tmp
				mov ecx,w
				mov eax,h
				pxor mm1,mm1
				mul ecx
bcl:
				movd mm0,[esi]
				punpcklbw mm0,mm1
				add esi,4
				movq [edi],mm0
				add edi,8
				dec eax
				jnz bcl

				emms
			}
		}

		stringstream offsets;
		offsets << "this = " << this;
		offsets << " &m_map = " << &m_map;
		offsets << " &tmpColor = " << &tmpColor;
		offsets << " &m_row = " << &m_row;
		std::cout << offsets.str() << std::endl;
	}
}


#define BLEND_PIXEL(n) \
{ \
	__asm lea esi,pix##n \
	__asm movq mm2,[edi] \
	__asm movq mm1,[esi] \
	__asm pmullw mm2,mm1 \
	__asm paddw mm0,mm2 \
}

CColor::RGBA& CTexture::GetMapColor(const CGenericVector<float> &texel)
{
	if (m_map == NULL)
	{
		tmpColor.r = tmpColor.g = tmpColor.b = 0;
		tmpColor.a = 1.0f;

		return tmpColor;
	}

	float px = texel.X();
	float py = texel.Y();
	//float pz = hit.Z();

	float dx,dy;
	float p1,p2,p3,p4;
	short	pix1[4];
	short	pix2[4];
	short	pix3[4];
	short	pix4[4];

	// mapping entre 0 et 1
	if ((px < 0.0f) || (px > 1.0f))
		px = (float)(px - floor(px));

	if ((py < 0.0f) || (py > 1.0f))
		py = (float)(py - floor(py));

	// proportionnalite a la taille de la texture
	px *= m_width;
	py *= m_height;

	// calcul de l'offset	( RGBA )
	unsigned int tx = (int)(floor(px));	
	unsigned int ty = (int)(floor(py));

	// filtrage bi-lineaire par defaut
	dx = px - tx;
	dy = py - ty;

	// compute 64bit color offset without overflow
	if (tx == m_width)
		tx = 0;
	if (ty == m_height)
		ty = 0;
	unsigned int ofs = (m_width * ty + tx) << 3;


	unsigned char *data = m_map;
	int row = m_row;

	// Cas ou le point est a gauche du centre du texel	
	if (dx < 0.5f)
	{
		dx = (0.5f - dx);


		// Cas ou le point est au dessus du centre du texel			
		if (dy < 0.5f)
		/* Dans ce cas, les trois texels 	*/
		/* supplementaires a entrer en compte	*/
		/* sont les trois adjacents en haut et	*/
		/* sur la gauche.			*/
		{
			dy = (0.5f - dy);
			p1 = (1.0f - dx) * (1.0f - dy);
			p2 = dx * (1.0f - dy);
			p3 = dx * dy;
			p4 = (1.0f - dx) * dy;

			pix1[0] = pix1[1] = pix1[2] = pix1[3] = (short)(255 * p1);
			pix2[0] = pix2[1] = pix2[2] = pix2[3] = (short)(255 * p2);
			pix3[0] = pix3[1] = pix3[2] = pix3[3] = (short)(255 * p3);
			pix4[0] = pix4[1] = pix4[2] = pix4[3] = (short)(255 * p4);

			__asm
			{
				mov edi,data
				add edi,ofs
				lea esi,pix1
				movq mm0,[edi]
				movq mm1,[esi]
				pmullw mm0,mm1
			}

			if (tx > 0) 
			{
				__asm
				{
					lea esi,pix2
					movq mm2,[edi-8]
					movq mm1,[esi]
					pmullw mm2,mm1
					paddw mm0,mm2
				}
			}
			else
				BLEND_PIXEL(2)
		
			if ((tx > 0) && (ty > 0)) 
			{
				__asm
				{
					push edi
					mov eax,row
					sub edi,8
					sub edi,eax
					BLEND_PIXEL(3)
					pop edi
				}
			}
			else
				BLEND_PIXEL(3)
		
			if (ty > 0) 
			{
				__asm
				{
					push edi
					mov eax,row
					sub edi,eax
					BLEND_PIXEL(4)
					pop edi
				}
			}
			else
				BLEND_PIXEL(4)
		}





		// Cas ou le point est en dessous du 	centre du texel			
		else
		{
			dy = (dy - 0.5f);
			p1 = (1.0f - dx) * (1.0f - dy);
			p2 = dx * (1.0f - dy);
			p3 = dx * dy;
			p4 = (1.0f - dx) * dy;

			pix1[0] = pix1[1] = pix1[2] = pix1[3] = (short)(255 * p1);
			pix2[0] = pix2[1] = pix2[2] = pix2[3] = (short)(255 * p2);
			pix3[0] = pix3[1] = pix3[2] = pix3[3] = (short)(255 * p3);
			pix4[0] = pix4[1] = pix4[2] = pix4[3] = (short)(255 * p4);

			__asm
			{
				mov edi,data
				add edi,ofs
				lea esi,pix1
				movq mm0,[edi]
				movq mm1,[esi]
				pmullw mm0,mm1
			}

			if (tx > 0) 
			{
				__asm
				{
					lea esi,pix2
					movq mm2,[edi-8]
					movq mm1,[esi]
					pmullw mm2,mm1
					paddw mm0,mm2
				}
			}
			else
				BLEND_PIXEL(2)

			if ((tx > 0) && (ty < m_height - 1)) 
			{
				__asm
				{
					push edi
					mov eax,row
					sub edi,8
					add edi,eax
					BLEND_PIXEL(3)
					pop edi
				}
			}
			else
				BLEND_PIXEL(3)

			if (ty < m_height - 1) 
			{
				__asm
				{
					push edi
					mov eax,row
					add edi,eax
					BLEND_PIXEL(4)
					pop edi
				}
			}
			else
				BLEND_PIXEL(4)
		}
	}



	/* Cas ou le point est a droite du centre	*/
	/* du texel					*/
	else
	{
		dx = (dx - 0.5f);

		// Cas ou le point est au dessus du centre du texel
		if (dy < 0.5f)
		/* Dans ce cas, les trois texels 	*/
		/* supplementaires a entrer en compte	*/
		/* sont les trois adjacents en haut et	*/
		/* sur la droite.			*/
		{
			dy = (0.5f - dy);
			p1 = (1.0f - dx) * (1.0f - dy);
			p2 = dx * (1.0f - dy);
			p3 = dx * dy;
			p4 = (1.0f - dx) * dy;

			pix1[0] = pix1[1] = pix1[2] = pix1[3] = (short)(255 * p1);
			pix2[0] = pix2[1] = pix2[2] = pix2[3] = (short)(255 * p2);
			pix3[0] = pix3[1] = pix3[2] = pix3[3] = (short)(255 * p3);
			pix4[0] = pix4[1] = pix4[2] = pix4[3] = (short)(255 * p4);

			__asm
			{
				mov edi,data
				add edi,ofs
				lea esi,pix1
				movq mm0,[edi]
				movq mm1,[esi]
				pmullw mm0,mm1
			}
			

			if (tx < m_width - 1)
			{
				__asm
				{
					lea esi,pix2
					movq mm2,[edi+8]
					movq mm1,[esi]
					pmullw mm2,mm1
					paddw mm0,mm2
				}
			}
			else
				BLEND_PIXEL(2)
			
			if ((tx < m_width - 1) && (ty > 0))
			{
				__asm
				{
					push edi
					mov eax,row
					add edi,8
					sub edi,eax
					BLEND_PIXEL(3)
					pop edi
				}
			}
			else
				BLEND_PIXEL(3)

			if (ty > 0)
			{
				__asm
				{
					push edi
					mov eax,row
					sub edi,eax
					BLEND_PIXEL(4)
					pop edi
				}
			}
			else
				BLEND_PIXEL(4)
		}



		else
		{
			dy = (dy - 0.5f);
			p1 = (1.0f - dx) * (1.0f - dy);
			p2 = dx * (1.0f - dy);
			p3 = dx * dy;
			p4 = (1.0f - dx) * dy;

			pix1[0] = pix1[1] = pix1[2] = pix1[3] = (short)(255 * p1);
			pix2[0] = pix2[1] = pix2[2] = pix2[3] = (short)(255 * p2);
			pix3[0] = pix3[1] = pix3[2] = pix3[3] = (short)(255 * p3);
			pix4[0] = pix4[1] = pix4[2] = pix4[3] = (short)(255 * p4);

			__asm
			{
				mov edi,data
				add edi,ofs
				lea esi,pix1
				movq mm0,[edi]
				movq mm1,[esi]
				pmullw mm0,mm1
			}

			if (tx < m_width - 1)
			{
				__asm
				{
					lea esi,pix2
					movq mm2,[edi+8]
					movq mm1,[esi]
					pmullw mm2,mm1
					paddw mm0,mm2
				}
			}
			else
				BLEND_PIXEL(2)

			if ((tx < m_width - 1) && (ty < m_height - 1))
			{
				__asm
				{
					push edi
					mov eax,row
					add edi,8
					add edi,eax
					BLEND_PIXEL(3)
					pop edi
				}
			}
			else
				BLEND_PIXEL(3)

			if (ty < m_height - 1)
			{
				__asm
				{
					push edi
					mov eax,row
					add edi,eax
					BLEND_PIXEL(4)
					pop edi
				}
			}
			else
				BLEND_PIXEL(4)
		}
	}

	float *vect = tmpColor.operator float *();
	__asm
	{
		mov edi,vect
		movq [edi],mm0
		emms
	}

	return tmpColor;
}