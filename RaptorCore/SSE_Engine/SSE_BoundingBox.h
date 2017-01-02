// SSE_BoundingBox.h: interface for the SSE_BoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)
#define AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_BOUNDINGBOX_H__DB24F01C_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/BoundingBox.h"
#endif


RAPTOR_NAMESPACE_BEGIN


#ifdef RAPTOR_SSE_CODE_GENERATION

	class RAPTOR_API CSSE_BoundingBox : public CBoundingBox
	{
	public:
		CSSE_BoundingBox();
		CSSE_BoundingBox(const CSSE_BoundingBox &r_box);	
		virtual ~CSSE_BoundingBox();

		//	Adding a bounding extends this to the minimum bounding box
		//	that contains this and r_box
		virtual void add(const CSSE_BoundingBox & r_box);

		CSSE_BoundingBox operator+(const CSSE_BoundingBox& r_box);
		CSSE_BoundingBox& operator=(const CSSE_BoundingBox& r_box);
		CSSE_BoundingBox& operator+=(const CSSE_BoundingBox& r_box);
		bool operator==(const CSSE_BoundingBox& r_box);

		//	Returns the min & max corners of the projected BBox
		virtual void get(GL_COORD_VERTEX &r_min,GL_COORD_VERTEX &r_max,const GL_MATRIX &m) const;

		//	Intersect generates the minimum BBox containing
		//	the intersection of this and r_box
		void intersect(const CSSE_BoundingBox& r_box);

		virtual void scale(float sx,float sy,float sz);
		virtual void translate(float tx,float ty,float tz);
		
		virtual bool isInBox(CGenericVector<float>& r_point) const;


		virtual float intersect(GL_COORD_VERTEX &origin,GL_COORD_VERTEX &direction) const;
	};

#endif //RAPTOR_SSE_CODE_GENERATION

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SSE_BOUNDINGBOX_H__DD1B683A_C345_4399_B1CF_4059B5C3A744__INCLUDED_)

