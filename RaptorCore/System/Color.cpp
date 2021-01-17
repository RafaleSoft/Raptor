/***************************************************************************/
/*                                                                         */
/*  Color.cpp                                                              */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
    #include "CGLTypes.h"
#endif
#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "Color.h"
#endif

RAPTOR_NAMESPACE


CColor::RGBA CColor::RGBA::operator+(const CColor::RGBA& r_color) const
{
	CColor::RGBA c(	r + r_color.r,
					g + r_color.g,
					b + r_color.b,
					a + r_color.a);
	return c;
}

CColor::RGBA CColor::RGBA::operator*(const CColor::RGBA& r_color) const
{
	CColor::RGBA c(	r * r_color.r,
					g * r_color.g,
					b * r_color.b,
					a * r_color.a);
	return c;
}

CColor::RGBA CColor::RGBA::operator*(float f) const
{
	CColor::RGBA c(r * f, g * f, b * f, a * f);
	return c;
}

CColor::RGBA::operator CColor::CYMK() const
{
    CColor::CYMK res;

    res.c = 1 - r;
    res.y = 1 - g;
    res.m = 1 - b;

    res.k = MIN(res.c,MIN(res.y,res.m));
    if (res.k >= 1)
    {
        res.c = 0.0f;
        res.y = 0.0f;
        res.m = 0.0f;
        res.k = 1.0f;
    }
    else
    {
        res.c = (res.c - res.k) / (1 - res.k);
        res.y = (res.y - res.k) / (1 - res.k);
        res.m = (res.m - res.k) / (1 - res.k);
    }

    return res;
}

CColor::CYMK::operator CColor::RGBA() const
{
    CColor::RGBA res;

    res.r = (1 - c) * (1 - k);
    res.g = (1 - m) * (1 - k);
    res.b = (1 - y) * (1 - k);
    res.a = 1.0f;

    return res;
}

CColor::CYMK::operator CColor::YUVA() const
{
    CColor::YUVA res;

    return res;
}

CColor::RGBA::operator CColor::YUVA() const
{
    CColor::YUVA res;

    res.y = 0.299 * r + 0.587 * g + 0.114 * b;
    res.u = -0.14713 * r - 0.28886 * g + 0.436 * b;
    res.v = 0.615 * r - 0.51498 * g - 0.10001 * b;
    res.a = a;

    return res;
}


CColor::YUVA::operator CColor::RGBA() const
{
    CColor::RGBA res;

    res.r = 1.0 * y + 0.0 * u + 1.13983 * v;
    res.g = 1.0 * y - 0.39465 * u - 0.58060 * v;
    res.b = 1.0 * y + 2.03211 * u + 0.0 * v;
    res.a = a;

    return res;
}

CColor::YUVA::operator CColor::CYMK() const
{
    CColor::CYMK res;

    return res;
}

CColor::RGBA::operator CColor::HSVA() const
{
    CColor::HSVA res;

	res.a = a;
    res.v = MAX(r,MAX(g,b));
	res.h = res.s = 0.0f;
	float C = res.v - MIN(r,MIN(b,g)); 

	if (res.v != 0)
	{
		if (C != 0)
		{
			res.s = C / res.v;
			res.h = (r == res.v) ?	(g - b) / C :
					(g == res.v) ?	2.0 + (b - r) / C :
									4.0 + (r - g) / C;
			res.h /= 6.0f;
			if (res.h < 0)
				res.h += 1.0f;
		}	
	}

    return res;
}

CColor::HSVA::operator CColor::RGBA() const
{
    CColor::RGBA res;

	res.a = a;
	res.g = res.b = res.r = 0.0f;

	float C = s*v;
	float H = h * 6.0f;
	if (H != 0)
	{
		float X = C * (1.0f-fabs(fmodf(H,2.0f)-1));
		if (H < 1.0f)
		{ res.r = C; res.g = X; }
		else if (H < 2.0f)
		{ res.r = X; res.g = C; }
		else if (H < 3.0f)
		{ res.g = C; res.b = X; }
		else if (H < 4.0f)
		{ res.g = X; res.b = C; }
		else if (H < 5.0f)
		{ res.r = X; res.b = C; }
		else
		{ res.r = C; res.b = X; }

		float m = v - C;
		res.r += m;
		res.g += m;
		res.b += m;
	}

    return res;
}

CColor::RGBA::operator CColor::HSLA() const
{
    CColor::HSLA res;

	res.a = a;
	res.h = res.s = 0.0f;
	float M = MAX(r,MAX(g,b));
	float m = MIN(r,MIN(b,g));
	float C = M - m;

	res.l = 0.5f*(M + m);
	if (res.l != 0)
	{
		if (C != 0)
		{
			if (res.l <= 0.5f)
				res.s = 0.5f * C / res.l;
			else
				res.s = 0.5f * C / (1.0f - res.l);

			res.h = (r == M) ?	(g - b) / C :
					(g == M) ?	2.0 + (b - r) / C :
								4.0 + (r - g) / C;
			res.h /= 6.0f;
			if (res.h < 0)
				res.h += 1.0f;
		}	
	}

    return res;
}

CColor::HSLA::operator CColor::RGBA() const
{
    CColor::RGBA res;

	res.a = a;
	res.g = res.b = res.r = 0.0f;

	float C = (1-fabs(2.0f*l-1)) * s;
	float H = h * 6.0f;
	if (H != 0)
	{
		float X = C * (1.0f-fabs(fmodf(H,2.0f)-1));
		if (H < 1.0f)
		{ res.r = C; res.g = X; }
		else if (H < 2.0f)
		{ res.r = X; res.g = C; }
		else if (H < 3.0f)
		{ res.g = C; res.b = X; }
		else if (H < 4.0f)
		{ res.g = X; res.b = C; }
		else if (H < 5.0f)
		{ res.r = X; res.b = C; }
		else
		{ res.r = C; res.b = X; }

		float m = l - 0.5f * C;
		res.r += m;
		res.g += m;
		res.b += m;
	}

    return res;
}

CColor::RGBA::operator CColor::HCYA() const
{
    CColor::HCYA res;

	res.a = a;
	res.h = 0.0f;
	float M = MAX(r,MAX(g,b)); 
	res.c = M - MIN(r,MIN(b,g)); 
	res.y = 0.299 * r + 0.587 * g + 0.114 * b;

	if (res.c != 0)
	{
		res.h = (r == M) ?	(g - b) / res.c :
				(g == M) ?	2.0 + (b - r) / res.c :
							4.0 + (r - g) / res.c;
		res.h /= 6.0f;
		if (res.h < 0)
			res.h += 1.0f;
	}

    return res;
}

CColor::HCYA::operator CColor::RGBA() const
{
    CColor::RGBA res;

	res.a = a;
	res.g = res.b = res.r = 0.0f;

	float H = h * 6.0f;
	if (H != 0)
	{
		float X = c * (1.0f-fabs(fmodf(H,2.0f)-1));
		if (H < 1.0f)
		{ res.r = c; res.g = X; }
		else if (H < 2.0f)
		{ res.r = X; res.g = c; }
		else if (H < 3.0f)
		{ res.g = c; res.b = X; }
		else if (H < 4.0f)
		{ res.g = X; res.b = c; }
		else if (H < 5.0f)
		{ res.r = X; res.b = c; }
		else
		{ res.r = c; res.b = X; }

		float m = y - (0.299 * res.r + 0.587 * res.g + 0.114 * res.b);
		res.r += m;
		res.g += m;
		res.b += m;
	}

    return res;
}


