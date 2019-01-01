// SubBoundingBox.cpp: implementation of the CSubBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#include "SubBoundingBox.h"


RAPTOR_NAMESPACE_BEGIN

#pragma warning (disable : 4711)	// inline constructor 
CSubBoundingBox CSubBoundingBox::subBBoxBuffer;

#define INDEX1	2
#define INDEX2	4
#define INDEX3	8
#define INDEX4	16
#define INDEX5	32

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubBoundingBox::CSubBoundingBox(SubBBoxDepth size):
m_tree(NULL)
{
	if (size<1)
		m_size = 0;
	else
		m_size = size;

	if (m_size>0)
	{
		m_tree = new SubBBoxData[m_size];
		memset(m_tree,0,m_size);
	}
}

#ifdef RAPTOR_DEBUG_MODE_GENERATION
    CSubBoundingBox::~CSubBoundingBox()
    {
        if (m_tree != NULL)
	        delete [] m_tree;
    }

	CSubBoundingBox& RAPTOR_FASTCALL CSubBoundingBox::subBBox(SubBBoxLevel subLevel)
	{
		subBBoxBuffer.m_size = m_size >> 3;
		subBBoxBuffer.m_tree = m_tree + subLevel * subBBoxBuffer.m_size;
		return subBBoxBuffer;
	}

	SubBBoxLevel RAPTOR_FASTCALL CSubBoundingBox::subLevel(CSubBoundingBox &box)
	{
		unsigned int subpos = ~((m_size >> 3) - 1);
		
		unsigned int pos = ((box.m_tree - m_tree) & subpos) >> 3;

		return (SubBBoxLevel)(pos);
	}

	void RAPTOR_FASTCALL CSubBoundingBox::fillBBox(SubBBoxLevel level)
	{
		if (m_size == DEPTH_0)
			*m_tree |= (0x1 << level);
	}
#endif

void CSubBoundingBox::getBBox(int &x,int &y,int &z)
{
	subBBoxBuffer.m_size = m_size;
	subBBoxBuffer.m_tree = m_tree;

	int index = 0;
	int depth = m_size;

	switch(depth)
	{
		case DEPTH_1: index = INDEX1; break;
		case DEPTH_2: index = INDEX2; break;
		case DEPTH_3: index = INDEX3; break;
		case DEPTH_4: index = INDEX4; break;
		case DEPTH_5: index = INDEX5; break;
	}

	while(depth>DEPTH_0)
	{
		int level = 0;
		if (y>=index) { level += Y_OFFSET; y -= index; }
		if (x>=index) { level += X_OFFSET; x -= index; }
		if (z>=index) { level += Z_OFFSET; z -= index; }

		depth = depth >> 3;
		index = index >> 1;

		subBBoxBuffer.m_tree = subBBoxBuffer.m_tree + level * depth;
	}
}

bool CSubBoundingBox::isFilled(int x,int y,int z)
{
	bool status = false;

	int level = X_OFFSET + Y_OFFSET + Z_OFFSET;
	int xx = x;
	int yy = y;
	int zz = z;

	getBBox(xx,yy,zz);

	if (yy == 0) level -= Y_OFFSET;
	if (xx == 0) level -= X_OFFSET;
	if (zz == 0) level -= Z_OFFSET;

	status = (*subBBoxBuffer.m_tree & (char)(0x1 << level)) != 0;
	
	return status;
}

bool CSubBoundingBox::fill(int x,int y,int z)
{
	bool status = false;

	int level = X_OFFSET + Y_OFFSET + Z_OFFSET;
	int xx = x;
	int yy = y;
	int zz = z;

	getBBox(xx,yy,zz);

	if (yy == 0) level -= Y_OFFSET;
	if (xx == 0) level -= X_OFFSET;
	if (zz == 0) level -= Z_OFFSET;

	status = (*subBBoxBuffer.m_tree & (char)(0x1 << level)) != 0;
	*subBBoxBuffer.m_tree |= (char)(0x1 << level);
	
	return status;
}

