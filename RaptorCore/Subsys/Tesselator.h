// Tesselator.h: interface for the CTesselator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESSELATOR_H__8C05321B_5915_4B8D_BB59_A097BAB19844__INCLUDED_)
#define AFX_TESSELATOR_H__8C05321B_5915_4B8D_BB59_A097BAB19844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVector4f;

class CTesselator  
{
public:
	CTesselator();
	virtual ~CTesselator();


	int triangulate(const CVector4f* points,
					int nCount);

	void tessNormal(float nx,float ny,float nz)
	{	normal.Set(nx,ny,nz,1.0f);	};

	void tessBeginContour(void);

	int * getTesselation(void) const { return m_triangleIndexes; };


protected:
	int         *m_nIndex;
	int			*m_triangleIndexes;
	int			m_nTriangle;

	CVector4f	m_e0;
	CVector4f	m_e1;
	CVector4f	m_N;

	float       m_A      ;  // 2 area
	
	enum  
	{  
		convex      =  1,
		degenerate  =  0,
		concave     = -1
	};


	int   removeVertex( int j,
						int nVertex );

	bool  isAnyPointInside(	const CVector4f* points,
							int i,int j,int k,
							int nVertex);// const;

	bool  isPointInside(const CVector4f& point,
						const CVector4f& q2);// const;

	int   triangleArea(	const CVector4f* points,
						int i,int j,int k);

	virtual  void  addFace(	const CVector4f* points,
							int i,int j,int k);


private:
	CVector4f normal;
	CVector4f ntmp;
	CVector4f pmq2;

	int		m_nMaxPoints;

public:
	void	allocIndex(int nCount);
};

#endif // !defined(AFX_TESSELATOR_H__8C05321B_5915_4B8D_BB59_A097BAB19844__INCLUDED_)
