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


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CSSE_BoundingBox::CSSE_BoundingBox():
		CBoundingBox()
	{
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
		__m128 min = _mm_loadu_ps(&xmin);
		__m128 max = _mm_loadu_ps(&xmax);
		__m128 org = _mm_loadu_ps(origin);
		__m128 dir = _mm_loadu_ps(direction);

		__m128 out_box = _mm_cmplt_ps(org, min);
		out_box = _mm_or_ps(_mm_cmpgt_ps(org, max), out_box);
		__m128 no_dir = _mm_cmpeq_ps(dir, _mm_setzero_ps());
		out_box = _mm_and_ps(out_box, no_dir);
		int reject = _mm_movemask_ps(out_box);
		
		// Si la direction est nulle et l'origine hors de l'axe boite, on sort.
		if (0 != (reject & 7))
			return FLT_MAX;

		// t1 = (min - origin) / direction;
		// t2 = (max - origin) / direction;
		__m128 t1 = _mm_div_ps(_mm_sub_ps(min, org), dir);
		__m128 t2 = _mm_div_ps(_mm_sub_ps(max, org), dir);

		// pas d'intersection si le cube est derriere l'origine i.e. if ((t1 < 0.0f) && (t2 < 0.0f))
		// et uniquement sur les composantes non éjectées ci-dessus (!reject)
		__m128 behind = _mm_cmplt_ps(t1, _mm_setzero_ps());
		behind = _mm_and_ps(behind, _mm_cmplt_ps(t2, _mm_setzero_ps()));
		reject = ~_mm_movemask_ps(no_dir);
		if (0 != (reject & _mm_movemask_ps(behind) & 7))
			return FLT_MAX;

		// Find max of min values && min of max values
		min = _mm_min_ps(t1, t2);
		max = _mm_max_ps(t1, t2);

		float tmin = FLT_EPSILON;
		float tmax = FLT_MAX;
		if (reject & 1)
		{
			tmin = MAX(min.m128_f32[0], tmin);
			tmax = MIN(max.m128_f32[0], tmax);
		}
		if (reject & 2)
		{
			tmin = MAX(min.m128_f32[1], tmin);
			tmax = MIN(max.m128_f32[1], tmax);
		}
		if (reject & 4)
		{
			tmin = MAX(min.m128_f32[2], tmin);
			tmax = MIN(max.m128_f32[2], tmax);
		}

		if (tmin <= tmax)
			return tmin;
		else
			return FLT_MAX;
	}

	void CSSE_BoundingBox::get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &M) const
	{
		__m128 vmin = _mm_load_ps1(&xmin);
		__m128 vmax = _mm_load_ps1(&xmax);
		__m128 mat = _mm_loadu_ps(&M[0]);
		__m128 min = _mm_mul_ps(mat, vmin);
		__m128 max = _mm_mul_ps(mat, vmax);
		__m128 dotmin = _mm_min_ps(min, max);
		__m128 dotmax = _mm_max_ps(min, max);

		vmin = _mm_load_ps1(&ymin);
		vmax = _mm_load_ps1(&ymax);
		mat = _mm_loadu_ps(&M[4]);
		min = _mm_mul_ps(mat, vmin);
		max = _mm_mul_ps(mat, vmax);
		dotmin = _mm_add_ps(dotmin,_mm_min_ps(min,max));
		dotmax = _mm_add_ps(dotmax,_mm_max_ps(min,max));

		vmin = _mm_load_ps1(&zmin);
		vmax = _mm_load_ps1(&zmax);
		mat = _mm_loadu_ps(&M[8]);
		min = _mm_mul_ps(mat, vmin);
		max = _mm_mul_ps(mat, vmax);
		dotmin = _mm_add_ps(dotmin, _mm_min_ps(min, max));
		dotmax = _mm_add_ps(dotmax, _mm_max_ps(min, max));

		mat = _mm_loadu_ps(&M[12]);
		dotmin = _mm_add_ps(dotmin, mat);
		dotmax = _mm_add_ps(dotmax, mat);

		_mm_storeu_ps(r_min.operator float *(), dotmin);
		_mm_storeu_ps(r_max.operator float *(), dotmax);
	}
#endif	// RAPTOR_SSE_CODE_GENERATION

