// ContourAttributes.h: interface for the CContourAttributes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_)
#define AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "System/CGLTypes.h"


RAPTOR_NAMESPACE_BEGIN


class CContourAttributes  
{
public:
	CContourAttributes();
	virtual ~CContourAttributes();


    virtual void extrude(const GL_COORD_VERTEX &pos,float extrusion = 100.0f);

	//
	//	Attributes specific to backface/edge algorithm
	//	An edge links two points of the geometry, and
	//	is shared by two adjacent faces, front and back
	typedef struct edge_t
	{
		int point[2];
		int front;
		int back;

		edge_t()
		{ point[0] = point[1] = front = back = -1; }
	} edge;

	//
	//	A line is a simple link within two points
	typedef struct line_t
	{
		unsigned int from;
		unsigned int to;
		line_t(unsigned int f=0, unsigned int t=0)
		{ from = f; to = t; }
	} line;

	//
	//	A specific operator to order edges.
	// This operator is part of the algorithm that constructs the edge list of a geometry.
	// This operator orders edges with their two points, to give a strict order
	struct myLess
	{
		bool operator()(const line& p1, const line& p2) const
		{
			unsigned int min1,min2,max1,max2;
			if (p1.from < p1.to)
			{
				min1 = p1.from;
				max1 = p1.to;
			}
			else
			{
				min1 = p1.to;
				max1 = p1.from;
			}
			if (p2.from < p2.to)
			{
				min2 = p2.from;
				max2 = p2.to;
			}
			else
			{
				min2 = p2.to;
				max2 = p2.from;
			}

			if (min1 < min2)
				return true;
			else if (min1 == min2)
				return (max1 < max2);
			else
				return false;
		}
	};

	// The ordered list of edges
	map<line,edge,myLess> edges;
	typedef map<line,edge,myLess>::const_iterator EDGE_ITR;
	//	The list of edges, for faster processing
	CContourAttributes::edge	*edgeList;

	//	The 'face' normals of the geometry
	GL_COORD_VERTEX	*pContourNormals;

	//	The list of lines that form the contour
	unsigned int contourSize;
	CContourAttributes::line *pContourEdges;

	//	The volume built from the contour, a direction and an extrusion value
	// ( the volume can extend to infinity )
	GL_COORD_VERTEX	*pContourVolume;

    unsigned short  *pContourFaces;

	//	The arrays here under are indexes that point
	//	to 'real' coordinates in the contour voume above.
	unsigned int contourVolumeSize;
	unsigned int darkCapSize;
	unsigned int lightCapSize;

	unsigned int *volume;
	unsigned int *darkcap;
	unsigned int *lightcap;

	//	An array that identifies the backfaces of the geometry
	bool		 *backfaces;

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_CONTOURATTRIBUTES_H__6ADB3B0A_FE00_4385_B7EC_3A239298653D__INCLUDED_)

