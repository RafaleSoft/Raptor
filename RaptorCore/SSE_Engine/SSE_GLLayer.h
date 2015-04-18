// SSE_GLLayer.h: interface for the CSSE_GLLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_GLLAYER_H__6ED4D4AE_1B6E_4B5F_B9D5_144BD23B795B__INCLUDED_)
#define AFX_SSE_GLLAYER_H__6ED4D4AE_1B6E_4B5F_B9D5_144BD23B795B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GLLAYER_H__12EA9DBD_DCDE_4C66_B607_DD32C023C8EF__INCLUDED_)
	#include "GLHierarchy/GLLayer.h"
#endif


RAPTOR_NAMESPACE_BEGIN

#ifdef RAPTOR_SSE2_CODE_GENERATION

    class RAPTOR_API CSSE_GLLayer : public CGLLayer
    {
    public:
		CSSE_GLLayer(int xpos,int ypos,unsigned int width,unsigned int height);
		virtual ~CSSE_GLLayer();

		//! Implements simd version of @see CGLLayer
		virtual void clear(unsigned long color);

		//! Implements simd version of @see CGLLayer
		virtual void drawAPoint(unsigned int x,unsigned int y,unsigned long color);

		//! Implements simd version of @see CGLLayer
		virtual void drawALine(int x0,int y0,int x1,int y1,unsigned long color);

		//! Implements simd version of @see CGLLayer
		virtual void drawRectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color);

		//! Implements simd version of @see CGLLayer
		virtual void drawARectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color);

		//! Implements simd version of @see CGLLayer
		virtual void drawAPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels);

		//! Implements simd version of @see CGLLayer
		void drawAText(int x0,int y0,const std::string& text,CGLFont *font,unsigned long color);
	};

#endif

RAPTOR_NAMESPACE_END

#endif	// !defined AFX_SSE_GLLAYER_H__6ED4D4AE_1B6E_4B5F_B9D5_144BD23B795B__INCLUDED_