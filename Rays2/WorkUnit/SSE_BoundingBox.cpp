// SSE_BoundingBox.cpp: implementation of the SSE_BoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SSE_BoundingBox.h"
#include "Raytracer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SSE_CBoundingBox::SSE_CBoundingBox()
{
	xmin = ymin = zmin = HUGE_REAL;
	xmax = ymax = zmax = -HUGE_REAL;
	hmin = hmax = 1.0f;
}

SSE_CBoundingBox::SSE_CBoundingBox(const SSE_CBoundingBox &r_box)
{
	xmin = r_box.xmin;
	ymin = r_box.ymin;
	zmin = r_box.zmin;

	xmax = r_box.xmax;
	ymax = r_box.ymax;
	zmax = r_box.zmax;
}

SSE_CBoundingBox::~SSE_CBoundingBox()
{
//	if (bsp != NULL)
//		delete bsp;
}

void SSE_CBoundingBox::Set(CGenericVector<float>& r_min,CGenericVector<float>& r_max)
{
	xmin = r_min.X();
	ymin = r_min.Y();
	zmin = r_min.Z();

	xmax = r_max.X();
	ymax = r_max.Y();
	zmax = r_max.Z();
}

void SSE_CBoundingBox::Set(float r_xmin,float r_ymin,float r_zmin,float r_xmax,float r_ymax,float r_zmax)
{
	xmin = r_xmin;
	ymin = r_ymin;
	zmin = r_zmin;

	xmax = r_xmax;
	ymax = r_ymax;
	zmax = r_zmax;
}

void SSE_CBoundingBox::Get(CGenericVector<float>& r_min,CGenericVector<float>& r_max) const
{
	r_min.Set(xmin,ymin,zmin,1.0f);
	r_max.Set(xmax,ymax,zmax,1.0f);
}

void SSE_CBoundingBox::Get(float& r_xmin,float& r_ymin,float& r_zmin,float& r_xmax,float& r_ymax,float& r_zmax) const
{
	r_xmin = xmin ;
	r_ymin = ymin ;
	r_zmin = zmin ;

	r_xmax = xmax ;
	r_ymax = ymax ;
	r_zmax = zmax ;
}

void SSE_CBoundingBox::Add(const SSE_CBoundingBox * const r_box)
{
	__asm
		{
			mov edi,r_box
			mov esi,this

			sse_loadupsofs(XMM1_EDI,4)	//	xmm1 = r_box->vmin
			sse_loadupsofs(XMM3_ESI,4)	//	xmm3 = this->vmin
			sse_loadupsofs(XMM2_EDI,20)	//	xmm2 = r_box->vmax
			sse_loadupsofs(XMM4_ESI,20)	//	xmm4 = this->vmax

			sse_minps(XMM1_XMM3)
			sse_maxps(XMM2_XMM4)
			sse_storeupsofs(XMM1_ESI,4)
			sse_storeupsofs(XMM2_ESI,20)
		}
}


void SSE_CBoundingBox::GetCenter(CGenericVector<float>& r_center) const
{
	r_center.X((xmin+xmax)*0.5f);
	r_center.Y((ymin+ymax)*0.5f);
	r_center.Z((zmin+zmax)*0.5f);
	r_center.H(1.0f);
}

void SSE_CBoundingBox::GetCenter(float& r_x,float& r_y,float& r_z) const
{
	r_x = (xmin + xmax) * 0.5f;
	r_y = (ymin + ymax) * 0.5f;
	r_z = (zmin + zmax) * 0.5f;
}

void SSE_CBoundingBox::ExtendTo(float r_x,float r_y,float r_z)
{
	xmin = MIN(xmin,r_x);
	ymin = MIN(ymin,r_y);
	zmin = MIN(zmin,r_z);

	xmax = MAX(xmax,r_x);
	ymax = MAX(ymax,r_y);
	zmax = MAX(zmax,r_z);
}

SSE_CBoundingBox SSE_CBoundingBox::operator+(const SSE_CBoundingBox& r_box)
{
	SSE_CBoundingBox b;

	b.xmin = MIN(xmin,r_box.xmin);
	b.ymin = MIN(ymin,r_box.ymin);
	b.zmin = MIN(zmin,r_box.zmin);

	b.xmax = MAX(xmax,r_box.xmax);
	b.ymax = MAX(ymax,r_box.ymax);
	b.zmax = MAX(zmax,r_box.zmax);

	return b;
}

SSE_CBoundingBox& SSE_CBoundingBox::operator+=(const SSE_CBoundingBox& r_box)
{
	xmin = MIN(xmin,r_box.xmin);
	ymin = MIN(ymin,r_box.ymin);
	zmin = MIN(zmin,r_box.zmin);

	xmax = MAX(xmax,r_box.xmax);
	ymax = MAX(ymax,r_box.ymax);
	zmax = MAX(zmax,r_box.zmax);

	return *this;
}

SSE_CBoundingBox& SSE_CBoundingBox ::operator=(const SSE_CBoundingBox& r_box)
{
	xmin = r_box.xmin;
	ymin = r_box.ymin;
	zmin = r_box.zmin;

	xmax = r_box.xmax;
	ymax = r_box.ymax;
	zmax = r_box.zmax;

	return *this;
}

void SSE_CBoundingBox::Scale(float sx,float sy,float sz)
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

void SSE_CBoundingBox::Translate(float tx,float ty,float tz)
{
	xmin+=tx;ymin+=ty;zmin+=tz;
	xmax+=tx;ymax+=ty;zmax+=tz;
}

bool SSE_CBoundingBox::operator==(const SSE_CBoundingBox& r_box)
{
	return (
		(xmin == r_box.xmin) &&
		(ymin == r_box.ymin) &&
		(zmin == r_box.zmin) &&
		(xmax == r_box.xmax) &&
		(ymax == r_box.ymax) &&
		(zmax == r_box.zmax) );
}

SSE_CBoundingBox SSE_CBoundingBox::Intersect(SSE_CBoundingBox& r_box ) const
{
	SSE_CBoundingBox b;

	b.xmin = MAX(xmin,r_box.xmin);
	b.xmax = MIN(xmax,r_box.xmax);
	if (b.xmin>b.xmax)
		return SSE_CBoundingBox();	// nul bounding box

	b.ymin = MAX(ymin,r_box.ymin);
	b.ymax = MIN(ymax,r_box.ymax);
	if (b.ymin>b.ymax)
		return SSE_CBoundingBox();

	b.zmin = MAX(zmin,r_box.zmin);
	b.zmax = MIN(zmax,r_box.zmax);
	if (b.zmin>b.zmax)
		return SSE_CBoundingBox();

	return b;
}

void SSE_CBoundingBox::Intersect(SSE_CBoundingBox& r_box )
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

bool SSE_CBoundingBox::IsInBox(CGenericVector<float>& r_point) const
{
	return (
		(xmin<=r_point.X()) &&
		(ymin<=r_point.Y()) &&
		(zmin<=r_point.Z()) &&
		(xmax>=r_point.X()) &&
		(ymax>=r_point.Y()) &&
		(zmax>=r_point.Z()) );
}

bool SSE_CBoundingBox::IsInBox(float x,float y,float z) const
{
	return (
		(xmin<=x) &&
		(ymin<=y) &&
		(zmin<=z) &&
		(xmax>=x) &&
		(ymax>=y) &&
		(zmax>=z) );
}



float SSE_CBoundingBox::Intersect(CGenericRay &ray) const
{	
	float tmin;
	
	__asm
	{
		mov edi,this
		mov esi,this
		sse_loadupsofs(XMM3_EDI,20)	//	xmm3 = Vmax
		sse_loadupsofs(XMM2_ESI,4)	//	xmm2 = Vmin

		mov edi,ray				
		mov esi,ray
		add edi,DIRECTION_OFFSET+4		//	edi = &ray.direction.vector
		add esi,ORIGIN_OFFSET+4			//	esi = &ray.origin.vector

		mov eax,[edi]			//	test if a 0.0 direction component can intersect the Bbox
	
		sse_loadups(XMM1_EDI)	//	xmm1 = direction
		sse_loadups(XMM0_ESI)	//	xmm0 = origin

		and eax,[edi+4]			
		and eax,[edi+8]			
		
		jnz intersect			//	no direction is 0.0, compute intersection
		
		sse_loadaps(XMM4_XMM0)
		sse_loadaps(XMM5_XMM0)
		sse_cmpgeps(XMM4_XMM2)
		sse_cmpleps(XMM5_XMM3)
		sse_andps(XMM4_XMM5)
		sse_orps(XMM4_XMM1)

		sse_movmskps(XMM0_XMM4)
		lea esi,tmin
		test eax,7

		mov eax,0x49742400			//	eax = HUGE_REAL
		jz	huge_return

intersect:
		sse_loadaps(XMM4_XMM0)	// xmm4 = origin
		sse_loadaps(XMM5_XMM1)	// xmm5 = direction

		sse_subps(XMM2_XMM0)
		sse_subps(XMM3_XMM4)

		sse_divps(XMM2_XMM1)	//	xmm2 = (vmin - o ) / d
		sse_divps(XMM3_XMM5)	//	xmm3 = (vmax - o ) / d

		sse_loadaps(XMM4_XMM2)
		sse_loadaps(XMM5_XMM3)

		sse_minps(XMM2_XMM5)	//	xmm2 = min v
		sse_maxps(XMM3_XMM4)	//	xmm3 = max v

		sse_loadaps(XMM5_XMM2)
		sse_loadaps(XMM4_XMM3)

		sse_shufps(XMM5_XMM5,SSE_R4_R2_R1_R3)
		sse_shufps(XMM4_XMM4,SSE_R4_R2_R1_R3)
		
		sse_cmpgtps(XMM5_XMM2)
		sse_cmpltps(XMM4_XMM3)
		lea esi,tmin
		sse_movmskps(XMM0_XMM5)

		//	find max of min v
		mov ah,al

		and al,5
		cmp al,1
		jne no_c

		sse_shufps(XMM2_XMM2,0xaa)		//	it's	min v [2]
		jmp testmax
no_c:
		and ah,6
		cmp ah,4
		jne no_b

		sse_shufps(XMM2_XMM2,0x55)		//	it's	min v [1]
		jmp testmax
no_b:

//		sse_shufps(XMM2_XMM2,0X00)		//	it's	min v [0] => don't shuffle
		
testmax:
		sse_movmskps(XMM0_XMM4)

		//	find min of max v
		mov ah,al

		and al,5
		cmp al,1
		jne no_c2

		sse_shufps(XMM3_XMM3,0Xaa)		//	it's	max v [2]
		jmp nointersect
no_c2:
		and ah,6
		cmp ah,4
		jne no_b2

		sse_shufps(XMM3_XMM3,0x55)		//	it's	max v [1]
		jmp nointersect
no_b2:
//		sse_shufps(XMM3_XMM3,0x00)		//	it's	max v [0] => don't shuffle
		
nointersect:
		sse_storess(XMM3_ESI)
		mov eax,0x49742400			//	eax = HUGE_REAL

		test dword ptr [esi],0x080000000		//	return HUGE_REAL if tmax < 0
		jnz huge_return

		sse_storess(XMM2_ESI)
		sse_comiss(XMM2_XMM3)
		jbe return_tmin				//	return tmin if tmin <= tmax
huge_return:
		
		mov [esi],eax
return_tmin:
		fld [esi]

	}

	return ;	
}
