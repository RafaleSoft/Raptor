// 3DPath.h: interface for the C3DPath class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_)
#define AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "3DEngine.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API C3DPath : public CPersistence  
{
public:
	typedef enum
	{
		CGL_INTERPOLATE,
		CGL_CATMULLROMEX,
		CGL_CATMULLROM
	} PATH_EVALUATION;

	typedef enum
	{
		CGL_ORIGINPATH,
		CGL_TARGETPATH
	} PATH_TYPE;

public:
	C3DPath(const std::string& name = "3DPATH");
	virtual ~C3DPath();

	//	Path type generation
	void setPathType(PATH_EVALUATION type);

	//	Specify a knot for the bezier curve :
	//	- x,y,z are the knot 'geometric' knot coordinates
	//	- knot is the knot position, in [0.0f .. 1.0f]
	//	- t denotes the tension of the curve at this knot ( catmull-rom only )
	//	- c denotes the continuity of the curve at this knot ( catmull-rom only )
	bool addKnot(	float x,float y,float z,float knot,
					float t = 0,float c = 0);			// specific parameters ( Catmull Rom Ex )

	//	Given a position between 0.0f and 1.0f, ( u )
	//	eval computes a position on the curve
	CGenericVector<float> &eval(float u);

	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(C3DPathClassID,"Path",CPersistence)


private:
	bool importKnot(CRaptorIO& io,GL_COORD_VERTEX_TAG &v,float &t, float &c);
	void reInitCoeffs(void);


	PATH_EVALUATION					evalType;

	vector<float>					uknots;
	// specific to polynomial
	vector<float>					ucoeffs;
	// tension : specific to Catmull-Rom ex
	vector<float>					tknots;
	// continuity : specific to Catmull-Rom ex
	vector<float>					cknots;
	
	vector< CGenericVector<float> >	knots;

	CGenericVector<float>	result;

	//	temporary data to avoid constructors
	//	( specific to Catmull-Rom & Ex )
	CGenericMatrix<float>	Hermite;

	CGenericVector<float>	Dprev;
	CGenericVector<float>	Dnext;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_)

