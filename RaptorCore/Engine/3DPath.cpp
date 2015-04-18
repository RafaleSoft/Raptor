// 3DPath.cpp: implementation of the C3DPath class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_)
	#include "3DPath.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif


//////////////////////////////////////////////////////////////////////
// Const & statics
//////////////////////////////////////////////////////////////////////
RAPTOR_NAMESPACE_BEGIN

static const float HERMITE[16] = 
{
	2	,	-2	,	1	,	1,
	-3	,	3	,	-2	,	-1,
	0	,	0	,	1	,	0,
	1	,	0	,	0	,	0
};

static C3DPath::C3DPathClassID pathId;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DPath::C3DPath(const std::string& name)
	:CPersistence(pathId,name),
	evalType(CGL_INTERPOLATE)
{
	Hermite = HERMITE;
}

C3DPath::~C3DPath()
{

}


//////////////////////////////////////////////////////////////////////
// Path operations
//////////////////////////////////////////////////////////////////////
void C3DPath::setPathType(PATH_EVALUATION type)
{
	switch(type)
	{
		case CGL_INTERPOLATE:
		case CGL_CATMULLROMEX:
		case CGL_CATMULLROM:
			evalType = type;
			break;
		default:
			evalType = CGL_INTERPOLATE;
	}
}


bool C3DPath::addKnot(float x,float y,float z,float knot,float t,float c)
{
	if ((knot<0)||(knot>1.0f))
		return false;

	vector<float>::iterator uknots_pos = uknots.begin();
	vector< CGenericVector<float> >::iterator knots_pos = knots.begin();
	vector<float>::iterator tknots_pos = tknots.begin();
	vector<float>::iterator cknots_pos = cknots.begin();

	while ((uknots_pos != uknots.end()) && (knot > *uknots_pos))
	{
		uknots_pos++;
		knots_pos++;
		tknots_pos++;
		cknots_pos++;
	}

	if ((uknots_pos != uknots.end()) && (knot == *uknots_pos))
		return false;
	else
	{
		CGenericVector<float> val(x,y,z,1.0f);

		uknots.insert(uknots_pos,knot);
		knots.insert(knots_pos,val);

		//	limit tension to reasonable values
		if (t<-1.0f)
			tknots.insert(tknots_pos,-1.0f);
		else if (t>1.0f)
			tknots.insert(tknots_pos,1.0f);
		else
			tknots.insert(tknots_pos,t);

		//	limit continuity to reasonable values
		if (c<-1.0f)
			cknots.insert(cknots_pos,-1.0f);
		else if (t>1.0f)
			cknots.insert(cknots_pos,1.0f);
		else
			cknots.insert(cknots_pos,c);

		reInitCoeffs();

		return true;
	}
}

void C3DPath::reInitCoeffs(void)
{
	ucoeffs.clear();

	int max = uknots.size();

	for (int i=0;i<max;i++)
	{
		float coeff = 1;
		for (int j=0;j<max;j++)
		{
			if (i != j)
				coeff *= (uknots[i]-uknots[j]);
		}
		ucoeffs.push_back(1.0f/coeff);
	}
}

CGenericVector<float> &C3DPath::eval(float u)
{
	result.Set(0.0f,0.0f,0.0f,1.0f);

	int max = uknots.size();

	if (evalType == CGL_INTERPOLATE)
	{
		for (int i=0;i<max;i++)
		{
			float coeff = 1;
			for (int j=0;j<max;j++)
			{
				if (i != j)
					coeff *= (u-uknots[j]);
			}

			coeff*=ucoeffs[i];
			result += ( knots[i] * coeff);
		}
	}
	else if (evalType == CGL_CATMULLROM)
	{
		int pos = 0;
		while ((pos < max-1)&&(u > uknots[pos]))
			pos++;

		if (pos == 0)
			result = knots[0];
		else if (u >= uknots[pos])
			result = knots[pos];
		else
		{
			//	valid points are knots[pos-1];knots[pos]
			CGenericVector<float>	S;

			//	calcul de s
			float s = (u - uknots[pos-1])/(uknots[pos] - uknots[pos-1]);
			S.H(1.0f);
			S.Z(s);
			s *= s;
			S.Y(s);
			s *= S.Z();
			S.X(s);

			// calcul des fonctions d'hermite
			S = S * Hermite;
			Dprev = Dnext = result;
			if ((pos > 1) && (pos < max))
				Dprev = (knots[pos] - knots[pos-2]) * 0.5f;
			if ((pos > 0) && (pos < max - 1))
				Dnext = (knots[pos+1] - knots[pos-1]) * 0.5f;

			// calcul du resultat
			result += (knots[pos-1] * S.X());
			result += (knots[pos] * S.Y());
			result += (Dprev * S.Z());
			result += (Dnext * S.H());	

			result.H(1.0f);
		}
	}
	else if (evalType == CGL_CATMULLROMEX)
	{
		int pos = 0;
		while ((pos < max-1)&&(u > uknots[pos]))
			pos++;

		if (pos == 0)
			result = knots[0];
		else if (u >= uknots[pos])
			result = knots[pos];
		else
		{
			//	valid points are knots[pos-1];knots[pos]
			CGenericVector<float>	S;

			//	calcul de s
			float s = (u - uknots[pos-1])/(uknots[pos] - uknots[pos-1]);
			S.H(1.0f);
			S.Z(s);
			s *= s;
			S.Y(s);
			s *= S.Z();
			S.X(s);

			// calcul des fonctions d'hermite
			S = S * Hermite;
			Dprev = Dnext = result;
			
			if ((pos > 1) && (pos < max))
            {
				Dprev = (knots[pos-1] - knots[pos-2]) * (1-tknots[pos-1]) * (1+cknots[pos-1]) * 0.5f +
						      (knots[pos] - knots[pos-1]) * (1-tknots[pos-1]) * (1-cknots[pos-1]) * 0.5f;
            }

			if ((pos > 0) && (pos < max - 1))
            {
				Dnext = (knots[pos] - knots[pos-1]) * (1-tknots[pos]) * (1-cknots[pos]) * 0.5f +
						     (knots[pos+1] - knots[pos]) * (1-tknots[pos]) * (1+cknots[pos]) * 0.5f;
            }
			/*
			if ((pos > 1) && (pos < max))
				Dprev = (1-tknots[pos-1]) * 0.5f * (knots[pos] - knots[pos-2]);
			if ((pos > 0) && (pos < max - 1))
				Dnext = (1-tknots[pos]) * 0.5f * (knots[pos+1] - knots[pos-1]);
			*/
			// calcul du resultat
			result += (knots[pos-1] * S.X());
			result += (knots[pos] * S.Y());
			result += (Dprev * S.Z());
			result += (Dnext * S.H());	

			result.H(1.0f);
		}
	}
	return result;
}


//////////////////////////////////////////////////////////////////////
// Serialisation
//////////////////////////////////////////////////////////////////////
bool C3DPath::exportObject(CRaptorIO& o)
{ 
	CPersistence::exportObject(o);
	
	int max = evalType;
	o << max;

	max = uknots.size();
	o << max;

    int i=0;

	for (i=0;i<max;i++)
		o << uknots[i];
	for (i=0;i<max;i++)
		o << tknots[i];
	for (i=0;i<max;i++)
		o << cknots[i];

	for (i=0;i<max;i++)
	{
		o << knots[i].X();
		o << knots[i].Y();
		o << knots[i].Z();
		o << knots[i].H();
	}

	return false;
}

bool C3DPath::importObject(CRaptorIO& io) 
{
    string name;

	io >> name; 
	string data = io.getValueName();

	while (!data.empty())
	{
		if (data == "model")
		{
			io >> name;
			if (name == "interpolate")
				evalType = CGL_INTERPOLATE;
			else if (name == "catmullromex")
				evalType = CGL_CATMULLROMEX;
            else if (name == "catmullrom")
				evalType = CGL_CATMULLROM;
            else
				evalType = CGL_INTERPOLATE;
		}
        else if (data == "Knot")
        {
            GL_COORD_VERTEX knot;
            float tension=0;
            float continuity=0;
            importKnot(io,knot,tension,continuity);
            addKnot(knot.x,knot.y,knot.z,knot.h,tension,continuity);
        }
        else if (data == "name")
			CPersistence::importObject(io);
		else
			io >> name;

		data = io.getValueName();
	}
	io >> name;

    reInitCoeffs();

	return true;
}

bool C3DPath::importKnot(CRaptorIO& io,GL_COORD_VERTEX_TAG &v,float &t, float &c)  
{ 
    string name;
	io >> name;
    
    string data = io.getValueName();
    while (!data.empty())
    {
		if (data == "x")
    		io >> v.x;
		else if (data == "y")
    		io >> v.y;
		else if (data == "z")
    		io >> v.z;
		else if (data == "h")
    		io >> v.h;
        else if (data == "t")
    		io >> t;
        else if (data == "c")
    		io >> c;
		else
			io >> name;

		data = io.getValueName();
    }

	// skip to next value
	io >> name;

	return true; 
}


