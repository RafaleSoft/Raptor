// BoundingBox.cpp: implementation of the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#include "Subsys/SubBoundingBox.h"

#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "BoundingBox.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4355)		// use of this pointer in constructor
#pragma warning (disable : 4711)	// automatic inline expansion warning
CBoundingBox::CBoundingBox()
:bsp(NULL)
{
	xmin=ymin=zmin=0;
	xmax=ymax=zmax=0;
	hmin = hmax = 1.0f;
}

CBoundingBox::CBoundingBox(const CBoundingBox &r_box)
:bsp(NULL)
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
	if (bsp != NULL)
		delete bsp;
}

void CBoundingBox::set(const GL_COORD_VERTEX& r_min,const GL_COORD_VERTEX& r_max)
{
	xmin = r_min.x;
	ymin = r_min.y;
	zmin = r_min.z;

	xmax = r_max.x;
	ymax = r_max.y;
	zmax = r_max.z;
}

void CBoundingBox::set(float r_xmin,float r_ymin,float r_zmin,float r_xmax,float r_ymax,float r_zmax)
{
	xmin = r_xmin;
	ymin = r_ymin;
	zmin = r_zmin;

	xmax = r_xmax;
	ymax = r_ymax;
	zmax = r_zmax;
}

void CBoundingBox::get(GL_COORD_VERTEX& r_min,GL_COORD_VERTEX& r_max) const
{
	r_min.x = xmin;
	r_min.y = ymin;
	r_min.z = zmin;
	r_min.h = 1.0f;

	r_max.x = xmax;
	r_max.y = ymax;
	r_max.z = zmax;
	r_max.h = 1.0f;
}

void CBoundingBox::get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m) const
{
	CGenericVector<float>	pmin;
	CGenericVector<float>	pmax;

	//	x component
	pmin.X(xmin*m[0]);	pmax.X(xmax*m[0]);
	if (pmin.X() < pmax.X()) { 	r_min.x = pmin.X(); r_max.x = pmax.X(); }
	else { 	r_min.x = pmax.X(); r_max.x = pmin.X(); }

	pmin.Y(xmin*m[4]);	pmax.Y(xmax*m[4]);
	if (pmin.Y() < pmax.Y()) { 	r_min.y = pmin.Y(); r_max.y = pmax.Y(); }
	else { 	r_min.y = pmax.Y(); r_max.y = pmin.Y(); }

	pmin.Z(xmin*m[8]);	pmax.Z(xmax*m[8]);
	if (pmin.Z() < pmax.Z()) { 	r_min.z = pmin.Z(); r_max.z = pmax.Z(); }
	else { 	r_min.z = pmax.Z(); r_max.z = pmin.Z(); }

	pmin.H(xmin*m[12]);	pmax.H(xmax*m[12]);
	if (pmin.H() < pmax.H()) { 	r_min.h = pmin.H(); r_max.h = pmax.H(); }
	else { 	r_min.h = pmax.H(); r_max.h = pmin.H(); }

	//	y component
	pmin.X(ymin*m[1]);	pmax.X(ymax*m[1]);
	if (pmin.X() < pmax.X()) { 	r_min.x += pmin.X(); r_max.x += pmax.X(); }
	else { 	r_min.x += pmax.X(); r_max.x += pmin.X(); }

	pmin.Y(ymin*m[5]);	pmax.Y(ymax*m[5]);
	if (pmin.Y() < pmax.Y()) { 	r_min.y += pmin.Y(); r_max.y += pmax.Y(); }
	else { 	r_min.y += pmax.Y(); r_max.y += pmin.Y(); }

	pmin.Z(ymin*m[9]);	pmax.Z(ymax*m[9]);
	if (pmin.Z() < pmax.Z()) { 	r_min.z += pmin.Z(); r_max.z += pmax.Z(); }
	else { 	r_min.z += pmax.Z(); r_max.z += pmin.Z(); }

	pmin.H(ymin*m[13]);	pmax.H(ymax*m[13]);
	if (pmin.H() < pmax.H()) { 	r_min.h += pmin.H(); r_max.h += pmax.H(); }
	else { 	r_min.h += pmax.H(); r_max.h += pmin.H(); }

	//	z component
	pmin.X(zmin*m[2]);	pmax.X(zmax*m[2]);
	if (pmin.X() < pmax.X()) { 	r_min.x += pmin.X(); r_max.x += pmax.X(); }
	else { 	r_min.x += pmax.X(); r_max.x += pmin.X(); }

	pmin.Y(zmin*m[6]);	pmax.Y(zmax*m[6]);
	if (pmin.Y() < pmax.Y()) { 	r_min.y += pmin.Y(); r_max.y += pmax.Y(); }
	else { 	r_min.y += pmax.Y(); r_max.y += pmin.Y(); }

	pmin.Z(zmin*m[10]);	pmax.Z(zmax*m[10]);
	if (pmin.Z() < pmax.Z()) { 	r_min.z += pmin.Z(); r_max.z += pmax.Z(); }
	else { 	r_min.z += pmax.Z(); r_max.z += pmin.Z(); }

	pmin.H(zmin*m[14]);	pmax.H(zmax*m[14]);
	if (pmin.H() < pmax.H()) { 	r_min.h += pmin.H(); r_max.h += pmax.H(); }
	else { 	r_min.h += pmax.H(); r_max.h += pmin.H(); }


	r_min.x += m[3];	r_max.x += m[3];
	r_min.y += m[7];	r_max.y += m[7];
	r_min.z += m[11];	r_max.z += m[11];
	r_min.h += m[15];	r_max.h += m[15];	
}

void CBoundingBox::getP(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m,const GL_MATRIX &mm) const
{
	CGenericVector<float>	Max;
	CGenericVector<float>	Min;
	CGenericVector<float>	p(xmin,ymin,zmin,1.0f);
	CGenericMatrix<float>	transform;

	// Init datas
	memcpy(transform.matrix(),m,16*sizeof(float));

	p *= transform;  p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmin + mm.rowz.y * ymin + mm.rowz.z * zmin + mm.rowz.h;
	Max = Min  = p;

	// Compute transformed vertex
	p.Set(xmin,ymin,zmax,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmin + mm.rowz.y * ymin + mm.rowz.z * zmax + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmax,ymin,zmax,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmax + mm.rowz.y * ymin + mm.rowz.z * zmax + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmax,ymin,zmin,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmax + mm.rowz.y * ymin + mm.rowz.z * zmin + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmin,ymax,zmin,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmin + mm.rowz.y * ymax + mm.rowz.z * zmin + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmin,ymax,zmax,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmin + mm.rowz.y * ymax + mm.rowz.z * zmax + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmax,ymax,zmax,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmax + mm.rowz.y * ymax + mm.rowz.z * zmax + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	p.Set(xmax,ymax,zmin,1.0);
	p *= transform; p.X() /= p.H();p.Y() /= p.H();p.Z() /= p.H();
	p.Z() = mm.rowz.x * xmax + mm.rowz.y * ymax + mm.rowz.z * zmin + mm.rowz.h;
	if (p.X() < Min.X()) { Min.X(p.X()); } else if (p.X() > Max.X()) { Max.X(p.X()); }
	if (p.Y() < Min.Y()) { Min.Y(p.Y()); } else if (p.Y() > Max.Y()) { Max.Y(p.Y()); }
	if (p.Z() < Min.Z()) { Min.Z(p.Z()); } else if (p.Z() > Max.Z()) { Max.Z(p.Z()); }

	memcpy(r_min,Min.vector(),4*sizeof(float));
	memcpy(r_max,Max.vector(),4*sizeof(float));
}

void CBoundingBox::get(float& r_xmin,float& r_ymin,float& r_zmin,float& r_xmax,float& r_ymax,float& r_zmax) const
{
	r_xmin = xmin ;
	r_ymin = ymin ;
	r_zmin = zmin ;

	r_xmax = xmax ;
	r_ymax = ymax ;
	r_zmax = zmax ;
}

float CBoundingBox::getDiameter(void) const
{
	return (float)(sqrt(	((xmax - xmin) * (xmax - xmin)) +
							((ymax - ymin) * (ymax - ymin)) +
							((zmax - zmin) * (zmax - zmin))
						));
}

void CBoundingBox::getSphere(GL_COORD_VERTEX &r_center,float &radius) const
{
	r_center.x = 0.5f * (xmax + xmin);
	r_center.y = 0.5f * (ymax + ymin);
	r_center.z = 0.5f * (zmax + zmin);
	r_center.h = 1.0f;

	radius = 0.5f * (float)(sqrt(	((xmax - xmin) * (xmax - xmin)) +
									((ymax - ymin) * (ymax - ymin)) +
									((zmax - zmin) * (zmax - zmin))
								));
}

void CBoundingBox::add(const CBoundingBox & r_box)
{
	xmin = MIN(xmin,r_box.xmin);
	ymin = MIN(ymin,r_box.ymin);
	zmin = MIN(zmin,r_box.zmin);

	xmax = MAX(xmax,r_box.xmax);
	ymax = MAX(ymax,r_box.ymax);
	zmax = MAX(zmax,r_box.zmax);
}


void CBoundingBox::getCenter(GL_COORD_VERTEX &r_center) const
{
	r_center.x=(xmin+xmax)*0.5f;
	r_center.y=(ymin+ymax)*0.5f;
	r_center.z=(zmin+zmax)*0.5f;
}

void CBoundingBox::getCenter(float& r_x,float& r_y,float& r_z) const
{
	r_x=(xmin+xmax)*0.5f;
	r_y=(ymin+ymax)*0.5f;
	r_z=(zmin+zmax)*0.5f;
}

void CBoundingBox::extendTo(float r_x,float r_y,float r_z)
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

void CBoundingBox::scale(float sx,float sy,float sz)
{
	GL_COORD_VERTEX c;
	c.x=(xmin+xmax)*0.5f;
	c.y=(ymin+ymax)*0.5f;
	c.z=(zmin+zmax)*0.5f;

	xmin = (xmin-c.x)*sx+c.x;
	ymin = (ymin-c.y)*sy+c.y;
	zmin = (zmin-c.z)*sz+c.z;

	xmax = (xmax-c.x)*sx+c.x;
	ymax = (ymax-c.y)*sy+c.y;
	zmax = (zmax-c.z)*sz+c.z;
}

void CBoundingBox::translate(float tx,float ty,float tz)
{
	xmin+=tx;ymin+=ty;zmin+=tz;
	xmax+=tx;ymax+=ty;zmax+=tz;
}

void CBoundingBox::translateAbsolute(float tx,float ty,float tz)
{
	float delta = tx - (xmin+xmax)*0.5f;
	xmin += delta;
	xmax += delta;

	delta = ty - (ymin+ymax)*0.5f;
	ymin += delta;
	ymax += delta;

	delta = tz - (zmin+zmax)*0.5f;
	zmin += delta;
	zmax += delta;
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

CBoundingBox CBoundingBox::intersect(const CBoundingBox& r_box ) const
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

void CBoundingBox::intersect(const CBoundingBox& r_box )
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

bool CBoundingBox::isInBox(GL_COORD_VERTEX& r_point) const
{
	return (
		(xmin<=r_point.x) &&
		(ymin<=r_point.y) &&
		(zmin<=r_point.z) &&
		(xmax>=r_point.x) &&
		(ymax>=r_point.y) &&
		(zmax>=r_point.z) );
}

bool CBoundingBox::isInBox(float x,float y,float z) const
{
	return (
		(xmin<=x) &&
		(ymin<=y) &&
		(zmin<=z) &&
		(xmax>=x) &&
		(ymax>=y) &&
		(zmax>=z) );
}


bool CBoundingBox::intersect(const CBoundingBox &r_box,GL_MATRIX &) const
{
	return (
		(xmax >= r_box.xmin) && (r_box.xmax >= xmin) &&
		(ymax >= r_box.ymin) && (r_box.ymax >= ymin) &&
		(zmax >= r_box.zmin) && (r_box.zmax >= zmin) );
}


float CBoundingBox::intersect(GL_COORD_VERTEX &origin,GL_COORD_VERTEX &direction) const
{	
	float tmin = (float)(FLT_EPSILON);
	float tmax = (float)(FLT_MAX);

	float t1;
	float t2;

	// x interval
	if (direction.x == 0.0)
	{
		if ((origin.x < xmin) || (origin.x > xmax))
			return FLT_MAX;
	}
	else
	{
		t1 = (xmin - origin.x) / direction.x;
		t2 = (xmax - origin.x) / direction.x;

		// pas d'intersection si le cube est derriere l'origine
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return FLT_MAX;

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

	// y interval
	if (direction.y == 0.0f)
	{
		if ((origin.y < ymin) || (origin.y > ymax))
			return FLT_MAX;
	}
	else
	{
		t1 = (ymin - origin.y) / direction.y;
		t2 = (ymax - origin.y) / direction.y;
		
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return FLT_MAX;

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

	// z interval
	if (direction.z == 0.0f)
	{
		if ((origin.z < zmin) || (origin.z > zmax))
			return FLT_MAX;
	}
	else
	{
		t1 = (zmin - origin.z) / direction.z;
		t2 = (zmax - origin.z) / direction.z;
		
		// pas d'intersection si le cube est derriere l'origine
		//if ((t1 < 0.0f) && (t2 < 0.0f))
		if ((*((int*)&t1) & 0x080000000) && (*((int*)&t2) & 0x080000000))
			return FLT_MAX;

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
		return FLT_MAX;
}


//
//
//	Sub bounding boxes
//
//
bool CBoundingBox::createBSP(SubBBoxDepth depth) 
{
	if ((depth == DEPTH_0) ||
		(depth == DEPTH_1) ||
		(depth == DEPTH_2) ||
		(depth == DEPTH_3) ||
		(depth == DEPTH_4) ||
		(depth == DEPTH_5))
		bsp = new CSubBoundingBox(depth);

	return (bsp != NULL);
}

bool CBoundingBox::fillBSP(GL_COORD_VERTEX &v1,GL_COORD_VERTEX &v2,GL_COORD_VERTEX &v3)
{
	if (bsp == NULL)
		return false;
	else
	{
		float dx = xmax - xmin;
		float dy = ymax - ymin;
		float dz = zmax - zmin;

		float div = 1.0f;
		switch(bsp->getSize())
		{
			case DEPTH_1: div = div/2; break;
			case DEPTH_2: div = div/4; break;
			case DEPTH_3: div = div/8; break;
			case DEPTH_4: div = div/16; break;
			case DEPTH_5: div = div/32; break;
		}

		dx *= div;
		dy *= div;
		dz *= div;

		return true;
	}
}

bool CBoundingBox::addToBSP(GL_COORD_VERTEX& vertex)
{
	if (bsp == NULL)
		return false;
	else
	{
		float minx = xmin;
		float miny = ymin;
		float minz = zmin;
		
		float maxx = xmax;
		float maxy = ymax;
		float maxz = zmax;

		float cx = 0.5f * (minx + maxx);
		float cy = 0.5f * (miny + maxy);
		float cz = 0.5f * (minz + maxz);

		CSubBoundingBox	curbsp = *bsp;
		SubBBoxLevel	level = 0;

		while (curbsp.getSize() > DEPTH_0)
		{
			level = 0;

			if (vertex.y > cy) { level += CSubBoundingBox::Y_OFFSET; miny = cy; } else maxy = cy;
			if (vertex.x > cx) { level += CSubBoundingBox::X_OFFSET; minx = cx; } else maxx = cx;
			if (vertex.z > cz) { level += CSubBoundingBox::Z_OFFSET; minz = cz; } else maxz = cz;

			curbsp = curbsp.subBBox(level);

			cx = 0.5f * (minx + maxx);
			cy = 0.5f * (miny + maxy);
			cz = 0.5f * (minz + maxz);
		}

		level = 0;
		if (vertex.y > cy) level += CSubBoundingBox::Y_OFFSET;
		if (vertex.x > cx) level += CSubBoundingBox::X_OFFSET;
		if (vertex.z > cz) level += CSubBoundingBox::Z_OFFSET;

		curbsp.fillBBox(level);
		curbsp.clear();

		return true;
	}
}


float CBoundingBox::renderBSP(void) const
{
	if (bsp == NULL)
		return 0.0f;
	else
	{
		float dx = xmax - xmin;
		float dy = ymax - ymin;
		float dz = zmax - zmin;
		int step = 2;

		float div = 1.0f;
		switch(bsp->getSize())
		{
			case DEPTH_1: step *= 2; break;
			case DEPTH_2: step *= 4; break;
			case DEPTH_3: step *= 8; break;
			case DEPTH_4: step *= 16; break;
			case DEPTH_5: step *= 32; break;
		}

		dx *= (div / step);
		dy *= (div / step);
		dz *= (div / step);

		float posx = xmin;

		glColor4f(1.0f,0.0f,0.0f,0.5f);
		glDisable(GL_CULL_FACE);
		glBegin(GL_QUADS);

		for (int i=0;i<step;i++)
		{
			float posy = ymin;

			for (int j=0;j<step;j++)
			{
				float posz = zmin;

				for (int k=0;k<step;k++)
				{
					if (bsp->isFilled(i,j,k))
					{
						glVertex3f(posx,posy,posz);	glVertex3f(posx+dx,posy,posz);glVertex3f(posx+dx,posy+dy,posz);glVertex3f(posx,posy+dy,posz);
						glVertex3f(posx,posy,posz+dz);	glVertex3f(posx+dx,posy,posz+dz);glVertex3f(posx+dx,posy+dy,posz+dz);glVertex3f(posx,posy+dy,posz+dz);
						glVertex3f(posx,posy+dy,posz);	glVertex3f(posx,posy+dy,posz+dz);glVertex3f(posx+dx,posy+dy,posz+dz);glVertex3f(posx+dx,posy+dy,posz);
						glVertex3f(posx,posy,posz);	glVertex3f(posx,posy,posz+dz);glVertex3f(posx+dx,posy,posz+dz);glVertex3f(posx+dx,posy,posz);
						glVertex3f(posx,posy,posz);	glVertex3f(posx,posy,posz+dz);glVertex3f(posx,posy+dy,posz+dz);glVertex3f(posx,posy+dy,posz);
						glVertex3f(posx+dx,posy+dy,posz);	glVertex3f(posx+dx,posy+dy,posz+dz);glVertex3f(posx+dx,posy,posz+dz);glVertex3f(posx+dx,posy,posz);
					}

					posz += dz;
				}

				posy += dy;
			}

			posx += dx;
		}

		glEnd();
	}

	return 0.0f;
}

