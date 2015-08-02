// BoundingBox.cpp: implementation of the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoundingBox.h"
#include "Raytracer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CBoundingBox::nbIntersect = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBoundingBox::CBoundingBox()
{
	xmin=ymin=zmin=HUGE_REAL;
	xmax=ymax=zmax=-HUGE_REAL;
}

CBoundingBox::CBoundingBox(const CBoundingBox &r_box)
{
	xmin = r_box.xmin;
	ymin = r_box.ymin;
	zmin = r_box.zmin;

	xmax = r_box.xmax;
	ymax = r_box.ymax;
	zmax = r_box.zmax;
}

CBoundingBox::~CBoundingBox()
{
//	if (bsp != NULL)
//		delete bsp;
}

void CBoundingBox::Set(CGenericVector<float>& r_min,CGenericVector<float>& r_max)
{
	xmin = r_min.X();
	ymin = r_min.Y();
	zmin = r_min.Z();

	xmax = r_max.X();
	ymax = r_max.Y();
	zmax = r_max.Z();
}

void CBoundingBox::Set(float r_xmin,float r_ymin,float r_zmin,float r_xmax,float r_ymax,float r_zmax)
{
	xmin = r_xmin;
	ymin = r_ymin;
	zmin = r_zmin;

	xmax = r_xmax;
	ymax = r_ymax;
	zmax = r_zmax;
}

void CBoundingBox::Get(CGenericVector<float>& r_min,CGenericVector<float>& r_max) const
{
	r_min.Set(xmin,ymin,zmin,1.0f);
	r_max.Set(xmax,ymax,zmax,1.0f);
}

void CBoundingBox::Get(float& r_xmin,float& r_ymin,float& r_zmin,float& r_xmax,float& r_ymax,float& r_zmax) const
{
	r_xmin = xmin ;
	r_ymin = ymin ;
	r_zmin = zmin ;

	r_xmax = xmax ;
	r_ymax = ymax ;
	r_zmax = zmax ;
}

void CBoundingBox::Add(const CBoundingBox * const r_box)
{
	xmin = MIN(xmin,r_box->xmin);
	ymin = MIN(ymin,r_box->ymin);
	zmin = MIN(zmin,r_box->zmin);

	xmax = MAX(xmax,r_box->xmax);
	ymax = MAX(ymax,r_box->ymax);
	zmax = MAX(zmax,r_box->zmax);
}


void CBoundingBox::GetCenter(CGenericVector<float>& r_center) const
{
	r_center.X((xmin+xmax)*0.5f);
	r_center.Y((ymin+ymax)*0.5f);
	r_center.Z((zmin+zmax)*0.5f);
	r_center.H(1.0f);
}

void CBoundingBox::GetCenter(float& r_x,float& r_y,float& r_z) const
{
	r_x=(xmin+xmax)*0.5f;
	r_y=(ymin+ymax)*0.5f;
	r_z=(zmin+zmax)*0.5f;
}

void CBoundingBox::ExtendTo(float r_x,float r_y,float r_z)
{
	xmin = MIN(xmin,r_x);
	ymin = MIN(ymin,r_y);
	zmin = MIN(zmin,r_z);

	xmax = MAX(xmax,r_x);
	ymax = MAX(ymax,r_y);
	zmax = MAX(zmax,r_z);
}

CBoundingBox CBoundingBox::operator+(const CBoundingBox& r_box)
{
	CBoundingBox b;

	b.xmin = MIN(xmin,r_box.xmin);
	b.ymin = MIN(ymin,r_box.ymin);
	b.zmin = MIN(zmin,r_box.zmin);

	b.xmax = MAX(xmax,r_box.xmax);
	b.ymax = MAX(ymax,r_box.ymax);
	b.zmax = MAX(zmax,r_box.zmax);

	return b;
}

CBoundingBox& CBoundingBox::operator+=(const CBoundingBox& r_box)
{
	xmin = MIN(xmin,r_box.xmin);
	ymin = MIN(ymin,r_box.ymin);
	zmin = MIN(zmin,r_box.zmin);

	xmax = MAX(xmax,r_box.xmax);
	ymax = MAX(ymax,r_box.ymax);
	zmax = MAX(zmax,r_box.zmax);

	return *this;
}

CBoundingBox& CBoundingBox ::operator=(const CBoundingBox& r_box)
{
	xmin = r_box.xmin;
	ymin = r_box.ymin;
	zmin = r_box.zmin;

	xmax = r_box.xmax;
	ymax = r_box.ymax;
	zmax = r_box.zmax;

	return *this;
}

void CBoundingBox::Scale(float sx,float sy,float sz)
{
	float cx = (xmin+xmax)*0.5f;
	float cy = (ymin+ymax)*0.5f;
	float cz = (zmin+zmax)*0.5f;

	xmin = (xmin - cx) * sx + cx;
	ymin = (ymin - cy) * sy + cy;
	zmin = (zmin - cz) * sz + cz;

	xmax = (xmax - cx) * sx + cx;
	ymax = (ymax - cy) * sy + cy;
	zmax = (zmax - cz) * sz + cz;
}

void CBoundingBox::Translate(float tx,float ty,float tz)
{
	xmin+=tx;ymin+=ty;zmin+=tz;
	xmax+=tx;ymax+=ty;zmax+=tz;
}

bool CBoundingBox::operator==(const CBoundingBox& r_box)
{
	return (
		(xmin == r_box.xmin) &&
		(ymin == r_box.ymin) &&
		(zmin == r_box.zmin) &&
		(xmax == r_box.xmax) &&
		(ymax == r_box.ymax) &&
		(zmax == r_box.zmax) );
}

CBoundingBox CBoundingBox::Intersect(CBoundingBox& r_box ) const
{
	CBoundingBox b;

	b.xmin = MAX(xmin,r_box.xmin);
	b.xmax = MIN(xmax,r_box.xmax);
	if (b.xmin>b.xmax)
		return CBoundingBox();	// nul bounding box

	b.ymin = MAX(ymin,r_box.ymin);
	b.ymax = MIN(ymax,r_box.ymax);
	if (b.ymin>b.ymax)
		return CBoundingBox();

	b.zmin = MAX(zmin,r_box.zmin);
	b.zmax = MIN(zmax,r_box.zmax);
	if (b.zmin>b.zmax)
		return CBoundingBox();

	return b;
}

void CBoundingBox::Intersect(CBoundingBox& r_box )
{
	xmin = MAX(xmin,r_box.xmin);
	xmax = MIN(xmax,r_box.xmax);
	if (xmin>xmax)
		xmin = xmax = 0;

	ymin = MAX(ymin,r_box.ymin);
	ymax = MIN(ymax,r_box.ymax);
	if (ymin>ymax)
		ymin = ymax = 0;

	zmin = MAX(zmin,r_box.zmin);
	zmax = MIN(zmax,r_box.zmax);
	if (zmin>zmax)
		zmin = zmax = 0;
}

bool CBoundingBox::IsInBox(CGenericVector<float>& r_point) const
{
	return (
		(xmin<=r_point.X()) &&
		(ymin<=r_point.Y()) &&
		(zmin<=r_point.Z()) &&
		(xmax>=r_point.X()) &&
		(ymax>=r_point.Y()) &&
		(zmax>=r_point.Z()) );
}

bool CBoundingBox::IsInBox(float x,float y,float z) const
{
	return (
		(xmin<=x) &&
		(ymin<=y) &&
		(zmin<=z) &&
		(xmax>=x) &&
		(ymax>=y) &&
		(zmax>=z) );
}


float CBoundingBox::Intersect(CGenericRay &ray) const
{	
	float tmin = (float)(SMALL_REAL);
	float tmax = (float)(HUGE_REAL);

	float t1;
	float t2;

	float *o = ray.origin.vector();
	float *d = ray.direction.vector();

	nbIntersect++;

	// x interval
	if (*(d) == 0.0)
	{
		if ((*(o) < xmin) || (*(o) > xmax))
			return HUGE_REAL;
	}
	else
	{
		t1 = (xmin - *(o)) / *(d);
		t2 = (xmax - *(o)) / *(d);

		// pas d'intersection si le cube est derriere l'origine
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return HUGE_REAL;

		if (t2 < t1)
		{
			if (t2 > tmin) tmin = t2;
			if (t1 < tmax) tmax = t1;
		}
		else
		{
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
		}
	}

	o++;
	d++;

	// y interval
	if (*(d) == 0.0f)
	{
		if ((*(o) < ymin) || (*(o) > ymax))
			return HUGE_REAL;
	}
	else
	{
		t1 = (ymin - *(o)) / *(d);
		t2 = (ymax - *(o)) / *(d);
		
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return HUGE_REAL;

		if (t2 < t1)
		{
			if (t2 > tmin) tmin = t2;
			if (t1 < tmax) tmax = t1;
		}
		else
		{
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
		}
	}

	o++;
	d++;

	// z interval
	if (*(d) == 0.0f)
	{
		if ((*(o) < zmin) || (*(o) > zmax))
			return HUGE_REAL;
	}
	else
	{
		t1 = (zmin - *(o)) / *(d);
		t2 = (zmax - *(o)) / *(d);
		
		// pas d'intersection si le cube est derriere l'origine
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return HUGE_REAL;

		if (t2 < t1)
		{
			if (t2 > tmin) tmin = t2;
			if (t1 < tmax) tmax = t1;
		}
		else
		{
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
		}
	}

	if (tmin <= tmax)
		return tmin;
	else
		return HUGE_REAL;
}