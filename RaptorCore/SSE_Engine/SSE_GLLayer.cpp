// SSE_3DEngine.cpp: implementation of the CSSE_3DEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_SSE_GLLAYER_H__6ED4D4AE_1B6E_4B5F_B9D5_144BD23B795B__INCLUDED_)
	#include "SSE_GLLayer.h"
#endif
#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
	#include "GLHierarchy/GL2DFont.h"
#endif

#include "Subsys/FreeType/FTGlyphBitmap.h"
#include "Subsys/FreeType/TTBitmapFont.h"

RAPTOR_NAMESPACE


#ifdef RAPTOR_SSE2_CODE_GENERATION

CSSE_GLLayer::CSSE_GLLayer(int xpos,int ypos,unsigned int width,unsigned int height):
	CGLLayer(xpos,ypos,width,height)
{
}

CSSE_GLLayer::~CSSE_GLLayer()
{
}

//	Assume buffer size is a multiple of 2 pixels !
void CSSE_GLLayer::clear(unsigned long color)
{
	__m128 pattern;
	pattern.m128_i32[0] = color;
	pattern = _mm_shuffle_ps(pattern,pattern,0);
	__m128 *buffer = (__m128*)getBuffer();
	int nb = (m_layerWidth*m_layerHeight>>2) - 1;
	while (nb >= 0)
		buffer[nb--] = pattern;
	m_bRedraw = true;
}


void CSSE_GLLayer::drawAPoint(unsigned int x,unsigned int y,unsigned long color)
{
	unsigned __int32 *buffer = (unsigned __int32*)getBuffer();

	// expand colors
	__m64 oldcolor = _mm_unpacklo_pi8(_mm_cvtsi32_si64(buffer[y*m_layerWidth+x]),_mm_setzero_si64());
	__m64 newcolor = _mm_unpacklo_pi8(_mm_cvtsi32_si64(color),_mm_setzero_si64());

	// alpha * color
	unsigned int a = ((color>>16)&0xff00)/255;
	__m64 alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64(a),0x40);
	newcolor = _mm_srli_pi16(_mm_mullo_pi16(newcolor, alpha),8);
	
	// (1 - alpha) * buffer
	alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64((256 - a) + 0x1000000),0x40);
	oldcolor = _mm_srli_pi16(_mm_mullo_pi16(oldcolor, alpha),8);

	// finalcolor = alpha * newcolor + (1-alpha) * oldcolor
	newcolor = _mm_add_pi16(newcolor,oldcolor);
	buffer[y*m_layerWidth+x] = _mm_cvtsi64_si32(_mm_packs_pu16(newcolor,newcolor));
	 _mm_empty();

    m_bRedraw = true;
}


//	Bresenham algorithm for line drawing
#if 0

void CGLLayer::drawLine(int x1,int y1,int x2,int y2,unsigned long color)
{
	int dinc1;
	int dinc2;
	int screeninc1;
	int screeninc2;
	int xinc1 = 0;
	int yinc1 = 0;
	int xinc2 = 1;
	int yinc2 = 1;
	int deltax = (short)(abs(x2-x1));
	int deltay = (short)(abs(y2-y1));

    m_bRedraw = true;
	
	// A fast ASM implementation of bresenham line algorithm
	// kept for backup and/or reference.
	// But with compiler optimisation, only the final while/loop
	// is faster.
	__asm
	{
		mov edi,this

		xor ecx,ecx
		mov eax,deltax		;	ax = deltax
		mov ebx,deltay		;	bx = deltay

		cmp eax,ebx
		jl	negatif
	
							;	if (deltax>=deltay)
		inc eax
		shl ebx,1			;	bx = 2 * deltay
		mov ecx,eax			;	cx = numpixels = deltax + 1

		dec eax
		mov dinc1,ebx		;	dinc1 = 2 * deltay
		sub ebx,eax
		mov edx,ebx			;	dx = 2 * deltay - deltax
		sub ebx,eax
		mov dinc2,ebx		;	dinc2 = 2 * deltay - 2 * deltax

		
		mov eax,0x0004
		mov xinc1,eax		;	xinc1 = 4 xinc2 = 4
		mov xinc2,eax
		mov yinc2,eax
		xor eax,eax
		mov yinc1,eax		;	yinc1 = 0 yinc2 = 4
		
		jmp positif
							; else ( deltax < deltay )
negatif:
		shl eax,1			;	ax = 2 * deltax
		inc ebx
		mov ecx,ebx			;	cx = numpixels = deltay + 1
		
		dec ebx
		mov dinc1,eax		;	dinc1 = 2 * deltax
		sub eax,ebx
		mov edx,eax			;	dx = 2 * deltax - deltay
		sub eax,deltay
		mov dinc2,eax		;	dinc2 = 2 * deltax - 2 * deltay

		
		mov eax,0x0004
		mov yinc1,eax		;	yinc1 = 4 yinc2 = 4
		mov yinc2,eax
		mov xinc2,eax
		xor eax,eax			
		mov xinc1,eax		;	xinc1 = 0 xinc2 = 4
		

positif:		
		mov eax,x2
	    cmp x1,eax
		jng suite			;	x1>x2 => inverser xinc_

		mov eax,xinc1		;	xinc1 = -xinc1
		mov ebx,xinc2		;	xinc2 = -xinc2
	    neg eax
		neg ebx
	    mov xinc1,eax
	    mov xinc2,ebx

	suite:
	    mov eax,y2			;	y1>y2 => inverser yinc_
	    cmp y1,eax
	    jng suite2

	    mov eax,yinc1		;	yinc1 = -yinc1
		mov ebx,yinc2		;	yinc2 = -yinc2
	    neg eax
		neg ebx
	    mov yinc1,eax
		mov yinc2,ebx

	suite2:
		push edx

	    mov eax,y1
	    //mov ebx,[edi+WIDTH_OFFSET]	;	ebx = m_layerWidth
        mov ebx,[edi].m_layerWidth
	    mul ebx
	    add eax,x1
		shl eax,2			;	offset in bytes = pixel offset *4 => rgba
	    //add eax,[edi+BUFFER_OFFSET]	;	offset = offset + m_pBuffer
        add eax,[edi].m_pBuffer
		mov edi,eax

	    mov eax,yinc1		;	yinc1
	    mul ebx
	    add eax,xinc1		;	xinc1
	    mov screeninc1,eax	;	screeninc1:=yinc1*m_layerWidth+xinc1;

	    mov eax,yinc2		;	yinc2
	    mul ebx
	    add eax,xinc2		;	xinc2
	    mov screeninc2,eax	;	screeninc2:=yinc2*m_layerWidth+xinc2;

	    mov eax,color
	    mov ebx,dinc1
		pop edx

	bres1:
		mov [edi],eax
		cmp edx,0
		jnl bres2

		add edx,ebx
		add edi,screeninc1
		jmp bres3

	bres2:
		add edx,dinc2
		add edi,screeninc2

	bres3:
		loop bres1
	}
}

#endif


//	Bresenham algorithm for line drawing
void CSSE_GLLayer::drawALine(int x1,int y1,int x2,int y2,unsigned long color)
{
	m_bRedraw = true;
	
	int xinc1 = 0;
	int yinc1 = 1;
	int xinc2 = 1;
	int yinc2 = 1;
	int deltax = (short)(abs(x2-x1));
	int deltay = (short)(abs(y2-y1));
	int dinc1 = 2 * deltax;
	int dinc2 = 2 * deltax - 2 * deltay;
	int d = 2 * deltax - deltay;
	int numpixels = deltay + 1;

	if (deltax>=deltay)
	{
		dinc1 = 2 * deltay;
		dinc2 = 2 * deltay - 2 * deltax;
		xinc1 = 1;
		yinc1 = 0;
		d = 2 * deltay - deltax;
		numpixels = deltax + 1;
	}

	if (x1>x2)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1>y2)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	int screeninc1 = yinc1 * m_layerWidth + xinc1;
	int screeninc2 = yinc2 * m_layerWidth + xinc2;
	unsigned int offset = y1 * m_layerWidth + x1;
	unsigned __int32 *buffer = (unsigned __int32*)getBuffer();

	__m64 newcolor = _mm_unpacklo_pi8(_mm_cvtsi32_si64(color),_mm_setzero_si64());
	unsigned int a = ((color>>16)&0xff00)/255;
	__m64 alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64(a),0x40);
	newcolor = _mm_srli_pi16(_mm_mullo_pi16(newcolor, alpha),8);
	__m64 one_minus_alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64((256 - a) + 0x1000000),0x40);

	while (numpixels > 0)
	{
		__m64 oldcolor = _mm_cvtsi32_si64(buffer[offset]);
		oldcolor = _mm_unpacklo_pi8(oldcolor,_mm_setzero_si64());

		// finalcolor = alpha * newcolor + (1-alpha) * oldcolor
		oldcolor = _mm_mullo_pi16(oldcolor, one_minus_alpha);
		oldcolor = _mm_add_pi16(newcolor,_mm_srli_pi16(oldcolor,8));
		buffer[offset] = _mm_cvtsi64_si32(_mm_packs_pu16(oldcolor,oldcolor));

		if (d > 0)
		{
			d +=  dinc2;
			offset += screeninc2;
		}
		else
		{
			d += dinc1;
			offset += screeninc1;
		}
		numpixels --;
	}

	_mm_empty();
}

void CSSE_GLLayer::drawARectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color)
{
#if 1
	unsigned __int32 *buffer = (unsigned __int32*)getBuffer();
	__m64 newcolor = _mm_unpacklo_pi8(_mm_cvtsi32_si64(color),_mm_setzero_si64());
	unsigned int a = ((color>>16)&0xff00)/255;
	__m64 alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64(a),0x40);
	newcolor = _mm_srli_pi16(_mm_mullo_pi16(newcolor, alpha),8);
	__m64 one_minus_alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64((256 - a) + 0x1000000),0x40);

	unsigned int offset = y0 * m_layerWidth;
	for (unsigned int j=y0;j<y1;j++)
	{
		//	further optimisation could be to process the first
		//	odd column and then process pixels by groups of 2
		for (unsigned int i=x0;i<x1;i++)
		{
			// expand colors
			__m64 oldcolor = _mm_cvtsi32_si64(buffer[offset+i]);
			oldcolor = _mm_unpacklo_pi8(oldcolor,_mm_setzero_si64());
	
			// finalcolor = alpha * newcolor + (1-alpha) * oldcolor
			oldcolor = _mm_mullo_pi16(oldcolor, one_minus_alpha);
			oldcolor = _mm_add_pi16(newcolor,_mm_srli_pi16(oldcolor,8));
			buffer[offset+i] = _mm_cvtsi64_si32(_mm_packs_pu16(oldcolor,oldcolor));
		}
		offset += m_layerWidth;
	}
	 _mm_empty();
#else	// This code can be faster if loop is reduced but not portable
	__asm
	{
		mov edi,this

		//	compute offset
	    mov eax,y0
	    //mov ebx,[edi+WIDTH_OFFSET]	;	ebx = m_layerWidth
        mov ebx,[edi].m_layerWidth
	    mul ebx
	    add eax,x0
		shl ebx,2
		shl eax,2
	    //add eax,[edi+BUFFER_OFFSET]	;	offset = offset + m_pBuffer
        add eax,[edi].m_pBuffer

		mov edi,eax			;	compute offsets
		mov esi,eax

		mov ecx,x1
		sub ecx,x0
		inc ecx
		
		mov edx,y1
		sub edx,y0
		inc edx

line:
		mov eax,color

		////////////////////////////////////
		//	put a pixel, with alpha blending
		push ebx
		mov ebx,eax			;	extract alpha
		shr ebx,24

		pxor mm0,mm0
		movd mm1,[edi]		;	extract buffer color in mm1
		punpcklbw mm1,mm0

		movd mm2,ebx
		movd mm3,eax
		punpcklbw mm3,mm0	;	newcolor in mm3

		mmx_pshufw(XMM2_XMM2,0) ;	mm2 = alpha

		mov eax,255
		sub eax,ebx
		movd mm0,eax

        mmx_pshufw(XMM0_XMM0,0) ;	mm0 = 1 - alpha

		pmullw mm2,mm3		;	mm2 = alpha * newcolor
		pmullw mm0,mm1		;	mm0 = (1-alpha) * buffer color
		psrlw mm2,8
		psrlw mm0,8
		paddw mm0,mm2

		packuswb mm0,mm0	;	pack color from 64 bits to 32 bits
		movd eax,mm0		;	put color in buffer
		pop ebx
		////////////////////////////////////

		mov [edi],eax		;	put color
		add edi,4
		loop line

		add esi,ebx
		mov edi,esi

		mov ecx,x1
		sub ecx,x0
		inc ecx

		dec edx
		jnz line

		emms
	}
#endif
    m_bRedraw = true;
}

void CSSE_GLLayer::drawRectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color)
{
	//
	// This code can be slower ... need investigations
	//	and/or optimisations.
	//
#ifdef RAPTOR_AVX_CODE_GENERATION
	unsigned int * buffer = (unsigned int * )getBuffer();
	__m256 *m256_buffer = (__m256*)buffer;
	__m256 newcolor;
	*((unsigned int *)&newcolor.m256_f32[0]) = color;
	*((unsigned int *)&newcolor.m256_f32[4]) = color;
	newcolor = _mm256_shuffle_ps(newcolor,newcolor,0);
	
	for (unsigned int j=y0;j<y1;j++)
	{
		unsigned int offset = j * m_layerWidth + x0;

		for (unsigned int i=0;i<(offset&7);i++)
			buffer[offset+i] = color;

		unsigned int m256_offset = offset / 8;
		for (unsigned int i=x0;i<x1;i+=8,offset+=8)
			_mm256_store_ps((float*)&(m256_buffer[m256_offset++]),newcolor);

		for (unsigned int i=0;i<(x1&7);i++)
			buffer[offset++] = color;
	}
#else
	unsigned int * buffer = (unsigned int * )getBuffer();
	__m128 *m128_buffer = (__m128*)buffer;
	__m128 newcolor;
	newcolor.m128_i32[0] = color;
	newcolor = _mm_shuffle_ps(newcolor,newcolor,0);
	
	for (unsigned int j=y0;j<y1;j++)
	{
		unsigned int offset = j * m_layerWidth + x0;

		for (unsigned int i=0;i<(offset&3);i++)
			buffer[offset+i] = color;

		// In some cases, _mm_stream_ps could improve performance
		unsigned int m128_offset = offset / 4;
		for (unsigned int i=x0;i<x1;i+=4,offset+=4)
			_mm_store_ps((float*)&m128_buffer[m128_offset++],newcolor);

		for (unsigned int i=0;i<(x1&3);i++)
			buffer[offset++] = color;
	}
#endif

    m_bRedraw = true;
}


void CSSE_GLLayer::drawAPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels)
{
	unsigned int offset = x0 + y0 * m_layerWidth;
	unsigned int o = 0;

	unsigned int * buffer = (unsigned int * )getBuffer();
	unsigned int * colors = (unsigned int * )pixels;
	for (unsigned int j=0;j<height;j++)
	{
		for (unsigned int i=0;i<width;i++)
		{
			unsigned int color = colors[o++];
			__m64 oldcolor = _mm_cvtsi32_si64(buffer[offset]);
			unsigned int a = ((color>>16)&0xff00)/255;
			__m64 newcolor = _mm_cvtsi32_si64(color);
			// erase alpha for new color (pixels)
			__m64 alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64(a),0x40);

			// expand colors
			oldcolor = _mm_unpacklo_pi8(oldcolor,_mm_setzero_si64());
			newcolor = _mm_unpacklo_pi8(newcolor,_mm_setzero_si64());
			newcolor = _mm_srli_pi16(_mm_mullo_pi16(newcolor, alpha),8);
			
			// keep alpha for new color (put a '1' for mullo_pi16)
			alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64((256 - a) + 0x1000000),0x40);
			oldcolor = _mm_srli_pi16(_mm_mullo_pi16(oldcolor, alpha),8);
			newcolor = _mm_add_pi16(newcolor,oldcolor);

			// finalcolor = alpha * newcolor + (1-alpha) * oldcolor
			buffer[offset++] = _mm_cvtsi64_si32(_mm_packs_pu16(newcolor,newcolor));
		}
		offset += (m_layerWidth-width);
	}

	_mm_empty();
    m_bRedraw = true;
}


void CSSE_GLLayer::drawAText(int x0,int y0,const std::string& text,
							 CGL2DFont *font,unsigned long color)
{
	const TTBitmapFont *fnt = font->getBitmapFont();
	FTGlyphBitmap* gbitmap;
	
	int px = x0;
	unsigned int width;
	unsigned int height;

	unsigned __int32 *buffer = (unsigned __int32*)getBuffer();
	__m64 newcolor = _mm_unpacklo_pi8(_mm_cvtsi32_si64(color),_mm_setzero_si64());
	unsigned int a = ((color>>16)&0xff00)/255;
	__m64 alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64(a),0x40);
	newcolor = _mm_srli_pi16(_mm_mullo_pi16(newcolor, alpha),8);
	__m64 one_minus_alpha = _mm_shuffle_pi16(_mm_cvtsi32_si64((256 - a) + 0x1000000),0x40);

	for (unsigned int i=0;i<text.size();i++)
	{
		gbitmap = fnt->getBitmap(text[i]);
		width = gbitmap->getWidth();

		if (width == 0)
		{
			px += gbitmap->getAdvance();
			continue;
		}

		int deltax = gbitmap->getDeltaX();
		int deltay = gbitmap->getDeltaY();
		height = gbitmap->getHeight();
		const GLubyte* bmp = (const GLubyte*)(gbitmap->getBitmap());

		//
		// Could be optimised further by using _mm_maskmove_si64
		// and by processing 8 bits at once, one mask per color channel.
		//
		unsigned int offset = (px + deltax) + (y0 + deltay) * m_layerWidth;
		for (unsigned int k=0;k<height;k++)
		{
			unsigned int bpos = k * gbitmap->getRowSize();
			unsigned char bitmask = 128;
			for (unsigned int j=0;j<width;j++)
			{
				if (bitmask == 0)
				{
					bpos++;
					bitmask = 128;
				}
				if (0 != (bmp[bpos] & bitmask))
				{
					// expand colors
					__m64 oldcolor = _mm_cvtsi32_si64(buffer[offset]);
					oldcolor = _mm_unpacklo_pi8(oldcolor,_mm_setzero_si64());
	
					// finalcolor = alpha * newcolor + (1-alpha) * oldcolor
					oldcolor = _mm_mullo_pi16(oldcolor, one_minus_alpha);
					oldcolor = _mm_add_pi16(newcolor,_mm_srli_pi16(oldcolor,8));
					buffer[offset] = _mm_cvtsi64_si32(_mm_packs_pu16(oldcolor,oldcolor));
				}
				offset++;
				bitmask /= 2;
			}
			bpos++;
			offset += (m_layerWidth - width);
		}

		px += gbitmap->getAdvance();
	}

	_mm_empty();
    m_bRedraw = true;
}

#endif

