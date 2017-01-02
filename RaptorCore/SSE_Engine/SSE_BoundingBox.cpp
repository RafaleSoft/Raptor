// SSE_BoundingBox.cpp: implementation of the SSE_BoundingBox class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
	#include "SSE_BoundingBox.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

RAPTOR_NAMESPACE

#ifdef RAPTOR_SSE_CODE_GENERATION


	#define VMIN_OFFSET	4
	#define VMAX_OFFSET	20
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CSSE_BoundingBox::CSSE_BoundingBox():
		CBoundingBox()
	{
		// Check offsets
		if (
			(((unsigned char*)(&xmin) - (unsigned char*)(this)) != VMIN_OFFSET) ||
			(((unsigned char*)(&xmax) - (unsigned char*)(this)) != VMAX_OFFSET)
			)
		{
			RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),"Raptor was not properly generated. Check CSSE_BoundingBox offsets");
		}
	}

	CSSE_BoundingBox::CSSE_BoundingBox(const CSSE_BoundingBox &r_box):
		CBoundingBox(r_box)
	{
	}

	CSSE_BoundingBox::~CSSE_BoundingBox()
	{
	}

	void CSSE_BoundingBox::add(const CSSE_BoundingBox & r_box)
	{
		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);

		_mm_storeu_ps(&xmin, _mm_min_ps(_mm_loadu_ps(&r_box.xmin), min));
		_mm_storeu_ps(&xmax, _mm_max_ps(_mm_loadu_ps(&r_box.xmax), max));
	}

	CSSE_BoundingBox CSSE_BoundingBox::operator+(const CSSE_BoundingBox& r_box)
	{
		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);

		CSSE_BoundingBox b;

		_mm_storeu_ps(&b.xmin, _mm_min_ps(_mm_loadu_ps(&r_box.xmin), min));
		_mm_storeu_ps(&b.xmax, _mm_max_ps(_mm_loadu_ps(&r_box.xmax), max));

		return b;
	}

	CSSE_BoundingBox& CSSE_BoundingBox::operator+=(const CSSE_BoundingBox& r_box)
	{
		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);

		_mm_storeu_ps(&xmin, _mm_min_ps(_mm_loadu_ps(&r_box.xmin), min));
		_mm_storeu_ps(&xmax, _mm_max_ps(_mm_loadu_ps(&r_box.xmax), max));

		return *this;
	}

	CSSE_BoundingBox& CSSE_BoundingBox ::operator=(const CSSE_BoundingBox& r_box)
	{
		_mm_storeu_ps(&xmin, _mm_loadu_ps(&r_box.xmin));
		_mm_storeu_ps(&xmax, _mm_loadu_ps(&r_box.xmax));

		return *this;
	}

	void CSSE_BoundingBox::scale(float sx,float sy,float sz)
	{
		__m128 scale = _mm_set_ps(1.0f, sz, sy, sx);
		__m128 half = { 0.5f, 0.5f, 0.5f, 0.5f };

		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);

		// c = 0.5 * (min + max)
		__m128 c = _mm_mul_ps(half,_mm_add_ps(min,max));

		// min = scale * (min - c) + c
		_mm_storeu_ps(&xmin, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(min, c), scale), c));

		// max = scale * (max - c) + c
		_mm_storeu_ps(&xmax, _mm_add_ps(_mm_mul_ps(_mm_sub_ps(max, c), scale), c));
	}

	void CSSE_BoundingBox::translate(float tx,float ty,float tz)
	{
		xmin+=tx;ymin+=ty;zmin+=tz;
		xmax+=tx;ymax+=ty;zmax+=tz;
	}

	bool CSSE_BoundingBox::operator==(const CSSE_BoundingBox& r_box)
	{
		return (
			(xmin == r_box.xmin) &&
			(ymin == r_box.ymin) &&
			(zmin == r_box.zmin) &&
			(xmax == r_box.xmax) &&
			(ymax == r_box.ymax) &&
			(zmax == r_box.zmax) );
	}

	void CSSE_BoundingBox::intersect(const CSSE_BoundingBox& r_box)
	{
		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);

		min = _mm_max_ps(_mm_loadu_ps(&r_box.xmin), min);
		max = _mm_min_ps(_mm_loadu_ps(&r_box.xmax), max);
		__m128 cmp = _mm_cmple_ps(min, max);

		_mm_storeu_ps(&xmin, _mm_and_ps(cmp, min));
		_mm_storeu_ps(&xmax, _mm_and_ps(cmp, max));
	}

	bool CSSE_BoundingBox::isInBox(CGenericVector<float>& r_point) const
	{
		return (
			(xmin<=r_point.X()) &&
			(ymin<=r_point.Y()) &&
			(zmin<=r_point.Z()) &&
			(xmax>=r_point.X()) &&
			(ymax>=r_point.Y()) &&
			(zmax>=r_point.Z()) );
	}

	float CSSE_BoundingBox::intersect(GL_COORD_VERTEX &origin,GL_COORD_VERTEX &direction) const
	{	
		float tmin;

		__asm
		{
			mov edi,this
			mov esi,this
			sse_loadupsofs(XMM3_EDI,VMAX_OFFSET)	//	xmm3 = Vmax
			sse_loadupsofs(XMM2_ESI,VMIN_OFFSET)	//	xmm2 = Vmin

			mov edi,direction				
			mov esi,origin
			
			mov eax,[edi]	//	test if a 0.0 direction component can intersect the Bbox
		
			sse_loadups(XMM1_EDI)	//	xmm1 = direction
			sse_loadups(XMM0_ESI)	//	xmm0 = origin

			and eax,[edi+4]			
			and eax,[edi+8]			
			
			jnz dointersect			//	no direction is 0.0, compute intersection
			
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

	dointersect:
			sse_loadaps(XMM4_XMM0)
			sse_loadaps(XMM5_XMM1)

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

			sse_shufps(XMM3_XMM3,0xaa)		//	it's	max v [2]
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
			sse_storess(XMM2_ESI)
			mov eax,0x49742400			//	eax = HUGE_REAL
			test dword ptr [esi],0x080000000		//	return HUGE_REAL if tmin < 0
			jnz huge_return
	
			sse_comiss(XMM2_XMM3)
			jc return_tmin
			jz return_tmin
	huge_return:
			mov [esi],eax
	return_tmin:
			fld [esi]

		}

		return ;	
	}

	void CSSE_BoundingBox::get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m) const
	{
		//	On transpose la matrice pour travailler en colonne
		// ( moins d'operations )
		GL_MATRIX M = m;
		
		float t = M[1]; M[1] = M[4]; M[4] = t;
		t = M[2]; M[2] = M[8] ; M[8] = t;
		t = M[3]; M[3] = M[12] ; M[12] = t;
		t = M[6]; M[6] = M[9] ; M[9] = t;
		t = M[7]; M[7] = M[13] ; M[13] = t;
		t = M[11]; M[11] = M[14] ; M[14] = t;

		GL_COORD_VERTEX vxmin(xmin,xmin,xmin,xmin);
		GL_COORD_VERTEX vxmax(xmax,xmax,xmax,xmax);
		GL_COORD_VERTEX vymin(ymin,ymin,ymin,ymin);
		GL_COORD_VERTEX vymax(ymax,ymax,ymax,ymax);
		GL_COORD_VERTEX vzmin(zmin,zmin,zmin,zmin);
		GL_COORD_VERTEX vzmax(zmax,zmax,zmax,zmax);

		__asm
		{
			lea esi,vxmin
			lea edi,M

			sse_loadups(XMM2_ESI)			// xmm2 = vxmin
			sse_loadups(XMM0_EDI)			// xmm0 = 1ère ligne [x0..x3]
			
			sse_mulps(XMM2_XMM0)
			lea esi,vxmax
			sse_loadaps(XMM6_XMM2)			// xmm2 = backup(xmm6)
			sse_loadups(XMM7_ESI)			// xmm3 = vxmax
			sse_mulps(XMM7_XMM0)

			sse_minps(XMM6_XMM7)			// xmm6 = min(xmin)
			sse_maxps(XMM7_XMM2)			// xmm7 = max(xmax)
			
			sse_loadaps(XMM4_XMM6)			// xmm4 = min result
			sse_loadaps(XMM5_XMM7)			// xmm5 = max result

			///

			lea esi,vymin
			sse_loadups(XMM2_ESI)			// xmm2 = vymin
			sse_loadupsofs(XMM0_EDI,16)		// xmm0 = 2ème ligne [y0..y3]

			sse_mulps(XMM2_XMM0)
			lea esi,vymax
			sse_loadaps(XMM6_XMM2)			// xmm2 = backup(xmm6)
			sse_loadups(XMM7_ESI)			// xmm3 = vymax
			sse_mulps(XMM7_XMM0)

			sse_minps(XMM6_XMM7)			// xmm6 = min(ymin)
			sse_maxps(XMM7_XMM2)			// xmm7 = max(ymax)
			
			sse_addps(XMM4_XMM6)			// xmm4 = min result
			sse_addps(XMM5_XMM7)			// xmm5 = max result

			///

			lea esi,vzmin
			sse_loadups(XMM2_ESI)			// xmm2 = vzmin
			sse_loadupsofs(XMM0_EDI,32)		// xmm0 = 3ème ligne [z0..z3]

			sse_mulps(XMM2_XMM0)
			lea esi,vzmax
			sse_loadaps(XMM6_XMM2)			// xmm2 = backup(xmm6)
			sse_loadups(XMM7_ESI)			// xmm3 = vzmax
			sse_mulps(XMM7_XMM0)

			sse_minps(XMM6_XMM7)			// xmm6 = min(zmin)
			sse_maxps(XMM7_XMM2)			// xmm7 = max(zmax)
			
			sse_addps(XMM4_XMM6)			// xmm4 = min result
			sse_addps(XMM5_XMM7)			// xmm5 = max result

			///

			sse_loadupsofs(XMM0_EDI,48)		// xmm0 = 3ème ligne [w0..w3]
			sse_addps(XMM4_XMM0)			// xmm4 = min result
			sse_addps(XMM5_XMM0)			// xmm5 = max result

			mov esi,r_min
			mov edi,r_max
			sse_storeups(XMM4_ESI)
			sse_storeups(XMM5_EDI)
		}
	}
#endif	// RAPTOR_SSE_CODE_GENERATION

