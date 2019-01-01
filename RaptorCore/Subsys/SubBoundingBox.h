// SubBoundingBox.h: interface for the CSubBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBBOUNDINGBOX_H__F2591551_5CBE_4533_89C7_E2E4C996B390__INCLUDED_)
#define AFX_SUBBOUNDINGBOX_H__F2591551_5CBE_4533_89C7_E2E4C996B390__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CBoundingBox;

typedef unsigned char SubBBoxLevel;
typedef unsigned char SubBBoxData;
typedef unsigned int	SubBBoxDepth;

#define DEPTH_0		1
#define DEPTH_1		8
#define	DEPTH_2		64
#define	DEPTH_3		512
#define	DEPTH_4		4096
#define DEPTH_5		32768

class CSubBoundingBox  
{
public:
	typedef enum
	{
		DOWN_LEFT_BACK,
		DOWN_LEFT_FRONT,
		DOWN_RIGHT_BACK,
		DOWN_RIGHT_FRONT,
		UP_LEFT_BACK,
		UP_LEFT_FRONT,
		UP_RIGHT_BACK,
		UP_RIGHT_FRONT
	} SUB_BBOX_BITS;

	typedef enum
	{
		Z_OFFSET = 1,
		X_OFFSET = 2,
		Y_OFFSET = 4,
	} SUB_BBOX_OFFSETS;

	

public:
	CSubBoundingBox(SubBBoxDepth size = DEPTH_1);
	

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	virtual ~CSubBoundingBox();
	CSubBoundingBox& RAPTOR_FASTCALL subBBox(SubBBoxLevel subLevel);
	SubBBoxLevel RAPTOR_FASTCALL subLevel(CSubBoundingBox &box);
	void RAPTOR_FASTCALL fillBBox(SubBBoxLevel level);
#else
	virtual ~CSubBoundingBox()
	{
		if (m_tree != NULL)
			delete [] m_tree;
	}
	CSubBoundingBox& RAPTOR_FASTCALL subBBox(SubBBoxLevel subLevel)
	{
		subBBoxBuffer.m_size = m_size >> 3;
		subBBoxBuffer.m_tree = m_tree + subLevel * subBBoxBuffer.m_size;
		return subBBoxBuffer;
	}

	SubBBoxLevel RAPTOR_FASTCALL subLevel(CSubBoundingBox &box)
	{
		unsigned int subpos = ~((m_size >> 3) - 1);
		
		unsigned int pos = ((box.m_tree - m_tree) & subpos) >> 3;

		return (SubBBoxLevel)(pos);
	}

	void RAPTOR_FASTCALL fillBBox(SubBBoxLevel level)
	{
		if (m_size == DEPTH_0)
			*m_tree |= (0x1 << level);
	}
#endif

	//	 BEWARE, this method leaves allocated
	//	out of reach !!!!!!
	SubBBoxData *clear(void) { SubBBoxData* tmp = m_tree; m_tree = NULL; return tmp;};
	bool isFilled(int x,int y,int z);
	bool fill(int x,int y,int z);
	SubBBoxDepth getSize(void) const { return m_size; };

private:
	SubBBoxDepth	m_size;
	SubBBoxData		*m_tree;

	void getBBox(int &x,int &y,int &z);

	static CSubBoundingBox subBBoxBuffer;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SUBBOUNDINGBOX_H__F2591551_5CBE_4533_89C7_E2E4C996B390__INCLUDED_)


