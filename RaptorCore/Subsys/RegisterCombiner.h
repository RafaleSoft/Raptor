// RegisterCombiner.h: interface for the CRegisterCombiner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_)
#define AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CRegisterCombiner  
{
public:
	CRegisterCombiner();
	virtual ~CRegisterCombiner();

    //!	Renders a register combiner separately
	//!	( tmus are supposed to be set properly )
    void glRender(void);


#if defined(GL_NV_register_combiners)

    typedef enum
    {
        CGL_MULT_MULT_SUM=0x0,	// 0000
        CGL_MULT_MULT_MUX=0x1,	// 0001
        CGL_MULT_DOT_DISCARD=0x2,	// 0010
        CGL_DOT_MULT_DISCARD=0x4,	// 0100
        CGL_DOT_DOT_DISCARD=0x6	// 0110
    } COMBINER_FUNCTION;

    class GL_COMBINER_INPUT
    {
    public:
        GL_COMBINER_INPUT();

	    GLenum	inputRGB;
	    GLenum	mappingRGB;
	    GLenum	usageRGB;
	    GLenum	inputAlpha;
	    GLenum	mappingAlpha;
	    GLenum	usageAlpha;
    };

    class GL_COMBINER
    {
    public:
        GL_COMBINER();

	    GL_COMBINER_INPUT	a;
	    GL_COMBINER_INPUT	b;
	    GL_COMBINER_INPUT	c;
	    GL_COMBINER_INPUT	d;
	    COMBINER_FUNCTION 		functionRGB;
	    COMBINER_FUNCTION 		functionAlpha;
	    GLenum				scale;
	    GLenum				bias;
	    GLenum				abOutputRGB;
	    GLenum				cdOutputRGB;
	    GLenum				sumOutputRGB;
	    GLenum				abOutputAlpha;
	    GLenum				cdOutputAlpha;
	    GLenum				sumOutputAlpha;
	    bool				active;
    };
    typedef GL_COMBINER *LP_GL_COMBINER;

    //!
    //!	Pixel shader : the real attributes.
    //!
	GL_COMBINER			r0;
	GL_COMBINER			r1;
	GL_COMBINER			r2;
	GL_COMBINER			r3;
	GL_COMBINER			r4;
	GL_COMBINER			r5;
	GL_COMBINER			r6;
	GL_COMBINER			r7;

	GL_COMBINER_INPUT	a;
	GL_COMBINER_INPUT	b;
	GL_COMBINER_INPUT	c;
	GL_COMBINER_INPUT	d;
	GL_COMBINER_INPUT	e;
	GL_COMBINER_INPUT	f;
	GL_COMBINER_INPUT	g;
	GLenum				inputAlpha;
	GLenum				mappingAlpha;
	GLenum				usageAlpha;
	GLenum				color_sum;
	GLenum				multiplier;
	GLenum				function;
#endif

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_REGISTERCOMBINER_H__734BF776_1E3A_45AA_9ED4_7F3344110DB3__INCLUDED_)

