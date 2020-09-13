/***************************************************************************/
/*                                                                         */
/*  ViewPoint.cpp                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/



#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "IViewPoint.h"
#endif
#if !defined(AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_)
	#include "3DPath.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif


RAPTOR_NAMESPACE

static IViewPoint::IViewPointClassID viewId;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IViewPoint::IViewPoint(const std::string& name) :
	CPersistence(viewId,name),
	currentPath(-1),timePos(0),startTime(0),endTime(1),
	m_lfAlpha(0.0),m_lfBeta(0.0),m_lfGamma(0.0),m_lfLength(0.0),
	m_bContinus(true),m_bLoop(true),
	model(IViewPoint::PERSPECTIVE)
{
    //  initialise positions
	Origin.Set(0.0f,0.0f,0.0f,1.0f);
	Target.Set(0.0f,0.0f,-1.0f,1.0f);
	Scale.One();

    //  Initialise view 
	viewVolume[0] = -1.33f;
	viewVolume[1] = 1.33f;
	viewVolume[2] = -1.0f;
	viewVolume[3] = 1.0f;
	viewVolume[4] = 1.0f;
	viewVolume[5] = 200.0f;

    // initialise trackers.
    pEyeTrack = NULL;
    pEyeTrack2 = NULL;
    pTargetTrack = NULL;
    pTargetTrack2 = NULL;
}

IViewPoint::~IViewPoint()
{
	//!	Avoid Engine continue to use this object beeing deleted.
	animate(false);
}

void IViewPoint::setPosition(float x, float y, float z, VIEW_POINT_POSITION p)
{
    switch(p)
    {
        case EYE:
	        Origin.Set(x,y,z,1.0f);
            break;
        case TARGET:
            Target.Set(x,y,z,1.0f);
            break;
    }

    recomputeViewPoint();
}

void IViewPoint::setViewVolume(float left, float right,
							   float bottom, float up,
							   float n, float f,
							   VIEW_POINT_MODEL m)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((right == left) ||
		(bottom == up) ||
		(n == f))
	{
		Raptor::GetErrorManager()->generateRaptorError(C3DEngine::C3DEngineClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_ERROR,
													   "Raptor View Point cannot resolve null view volume");
	}
	else
#endif
	{
		viewVolume[0] = left;
		viewVolume[1] = right;
		viewVolume[2] = bottom;
		viewVolume[3] = up;
		viewVolume[4] = n;
		viewVolume[5] = f;
		model = m;
	}
}

void IViewPoint::getViewVolume(float &left, float &right,
								float &bottom, float &up,
								float &n, float &f,
								VIEW_POINT_MODEL &m) const
{
    left = viewVolume[0];
	right = viewVolume[1];
	bottom = viewVolume[2];
	up = viewVolume[3];
	n = viewVolume[4];
	f = viewVolume[5];
	m = model;
}

void IViewPoint::getFrustum(CGenericMatrix<float, 4>& frustum) const
{
	frustum.Zero();

	float l = viewVolume[0];
	float r = viewVolume[1];
	float b = viewVolume[2];
	float t = viewVolume[3];
	float n = viewVolume[4];
	float f = viewVolume[5];

	if (IViewPoint::PERSPECTIVE == model)
	{
		//	2.n / (r-l)		0		(r+l) / (r-l)		0
		//		0		2.n / (t-b)	(t+b) / (t-b)		0
		//		0			0		-(f+n) / (f-n)	-2.f.n / (f-n)
		//		0			0			-1				0

		frustum[0] = 2 * n / (r - l);
		frustum[2] = (r + l) / (r - l);

		frustum[5] = 2 * n / (t - b);
		frustum[6] = (t + b) / (t - b);

		frustum[10] = -(f + n) / (f - n);
		frustum[11] = -2 * f * n / (f - n);

		frustum[14] = -1;
	}
	else if (IViewPoint::ORTHOGRAPHIC == model)
	{
		//	2 / (r-l)		0			0			-(r+l) / (r-l)
		//		0		2 / (t-b)		0			-(t+b) / (t-b)
		//		0			0		-2 / (f-n)		-(f+n) / (f-n)
		//		0			0			0				1
		
		frustum[0] = 2 / (r - l);
		frustum[3] = -(r + l) / (r - l);

		frustum[5] = 2 / (t - b);
		frustum[7] = -(t + b) / (t - b);

		frustum[10] = -2 / (f - n); 
		frustum[11] = -(f + n) / (f - n);

		frustum[15] = 1;
	}
	else
	{
		// Unreachable code
	}
}

void IViewPoint::getTransposeFrustum(CGenericMatrix<float, 4>& frustum) const
{
	frustum.Zero();

	float l = viewVolume[0];
	float r = viewVolume[1];
	float b = viewVolume[2];
	float t = viewVolume[3];
	float n = viewVolume[4];
	float f = viewVolume[5];

	if (IViewPoint::PERSPECTIVE == model)
	{
		//	2.n / (r-l)			0				0				0
		//		0			2.n / (t-b)			0				0
		//	(r+l) / (r-l)	(t+b) / (t-b)	-(f+n) / (f-n)  	-1
		//		0				0			-2.f.n / (f-n)		0

		frustum[0] = 2 * n / (r - l);

		frustum[5] = 2 * n / (t - b);
		
		frustum[8] = (r + l) / (r - l);
		frustum[9] = (t + b) / (t - b);
		frustum[10] = -(f + n) / (f - n);
		frustum[11] = -1;

		frustum[14] = -2 * f * n / (f - n);
	}
	else if (IViewPoint::ORTHOGRAPHIC == model)
	{
		//	2 / (r-l)			0				0				0
		//		0			2 / (t-b)			0				0
		//		0				0			-2 / (f-n)			0
		//	-(r+l) / (r-l)	-(t+b) / (t-b)	-(f+n) / (f-n)		1

		frustum[0] = 2 / (r - l);

		frustum[5] = 2 / (t - b);
		
		frustum[10] = -2 / (f - n);

		frustum[12] = -(r + l) / (r - l);
		frustum[13] = -(t + b) / (t - b);
		frustum[14] = -(f + n) / (f - n);
		frustum[15] = 1;
	}
	else
	{
		// Unreachable code
	}
}

CGenericVector<float> IViewPoint::getPosition(VIEW_POINT_POSITION p) const
{
    switch(p)
    {
        case EYE:
        {
			return Origin;
            break;
        }
        case TARGET:
        {
            return Target;
            break;
        }
        //! This case is never reached
        default:
            return CGenericVector<float>(0,0,0,0);
    }
}

//////////////////////////////////////////////////////////////////////
// Transforms
//////////////////////////////////////////////////////////////////////
void IViewPoint::recomputeTransform()
{
	Transform.Ident();
	Transform.matrix()[0] = Scale.X();
	Transform.matrix()[5] = Scale.Y();
	Transform.matrix()[10] = Scale.Z();
}

void IViewPoint::translate(float tx, float ty, float tz)
{
	CGenericVector<float>	z_axis = Origin - Target;
	z_axis.Normalize();

	CGenericVector<float> x_axis;
	CGenericVector<float> y_axis;
	C3DEngine::Get3DEngine()->normals(z_axis,x_axis,y_axis);

	//	x_axis y component must be 0.
	//	keep y_axis y component positive ...
	if (y_axis.Y() < 0)
	{
		y_axis = !y_axis;
		x_axis = !x_axis;
	}

	CGenericVector<float>	delta = x_axis*tx + y_axis*ty + z_axis*tz;
	delta.H(0.0f);

	Origin += delta;
	Target += delta;
}

void IViewPoint::scale(float sx, float sy, float sz)
{
	Scale.X() *= sx;
	Scale.Y() *= sy;
	Scale.Z() *= sz;
}

void IViewPoint::rotationZ(float rz)
{
	m_lfGamma += rz;
}

void IViewPoint::rotationX(float rx)
{
	m_lfBeta -= rx;
	if (m_lfBeta > 360.0)
		m_lfBeta -= 360.0;
	else if (m_lfBeta < 0.0)
		m_lfBeta += 360.0;

	CGenericVector<float> v = Target - Origin;

	v.Y()  = m_lfLength * sin(TO_RADIAN(m_lfBeta));
	float d2 = m_lfLength*m_lfLength - (v.Y()*v.Y());
	float d = -sqrt(d2);
	
	v.Z() = d * cos(TO_RADIAN(m_lfAlpha));
	v.X() = d * sin(TO_RADIAN(m_lfAlpha));

	Target = v + Origin;
}

void IViewPoint::rotationY(float ry)
{
	m_lfAlpha -= ry;
	if (m_lfAlpha > 360.0)
		m_lfAlpha -= 360.0;
	else if (m_lfAlpha < 0.0)
		m_lfAlpha += 360.0;

	CGenericVector<float> v = Target - Origin;

	float d2 = m_lfLength*m_lfLength - (v.Y()*v.Y());
	float d = -sqrt(d2);

	v.Z() = d * cos(TO_RADIAN(m_lfAlpha));
	v.X() = d * sin(TO_RADIAN(m_lfAlpha));

	Target = v + Origin;
}


//////////////////////////////////////////////////////////////////////
// Rendering
//////////////////////////////////////////////////////////////////////
void IViewPoint::recomputeViewPoint(void)
{
	CGenericVector<float> v = Target - Origin;
	
	m_lfLength = v.Normalize();

    m_lfAlpha = TO_DEGREE(atan2(v.X(),v.Z())) + 180.0;
    m_lfBeta = -TO_DEGREE(atan2(sqrt(v.X()*v.X()+v.Z()*v.Z()),v.Y())) + 90.0;
}

void IViewPoint::setTimeInterval(float tMin, float tMax)
{
	if ((tMin<tMax)&&(tMin>=0))
	{
		startTime = tMin;
		endTime = tMax;
	}
	else
	{
		startTime = 0.0f;
		endTime = 1.0f;
	}
}

void IViewPoint::setTimePos(float tpos)
{ 
	if ((tpos>endTime)||(tpos<startTime))
		timePos = startTime;
	else
		timePos = tpos; 
}

void IViewPoint::trackObject(CObject3D *object, VIEW_POINT_POSITION p)
{
    switch(p)
    {
        case EYE:
            pEyeTrack = object;
            pEyeTrack2 = NULL;
            break;
        case TARGET:
            pTargetTrack = object;
            pTargetTrack2 = NULL;
            break;
    }
}
 
void IViewPoint::trackObject(CLight *object, VIEW_POINT_POSITION p)
{
    switch(p)
    {
        case EYE:
            pEyeTrack2 = object;
            pEyeTrack = NULL;
            break;
        case TARGET:
            pTargetTrack2 = object;
            pTargetTrack = NULL;
            break;
    }
}

void RAPTOR_FASTCALL IViewPoint::deltaTime(float dt)
{
	timePos += dt;

    if (pEyeTrack != NULL)
    {
        GL_COORD_VERTEX center;
        pEyeTrack->getCenter(center);
        setPosition(center.x,center.y,center.z,EYE);
    }
    else if (pEyeTrack2 != NULL)
    {
        GL_COORD_VERTEX center = pEyeTrack2->getLightPosition();
        setPosition(center.x,center.y,center.z,EYE);
    }
    
    if (pTargetTrack != NULL)
    {
        GL_COORD_VERTEX center;
        pTargetTrack->getCenter(center);
        setPosition(center.x,center.y,center.z,TARGET);
    }
    else if (pTargetTrack2 != NULL)
    {
        GL_COORD_VERTEX center = pTargetTrack2->getLightPosition();
        setPosition(center.x,center.y,center.z,TARGET);
    }
    

	if (currentPath>=0)
	{
		if (timePos>endTime)
		{
			if (m_bLoop)
				timePos = startTime;
			else 
				timePos = endTime;

			if (m_bContinus)
			{
				currentPath++;
				if (m_bLoop)
				{
					if (currentPath >= (int)(eyePositionPaths.size()))
						currentPath = 0;
				}
				else
				{
					if (currentPath < (int)(eyePositionPaths.size()))
						timePos = startTime;
					else
						currentPath--;
				}
			}
		}

		float uPos = (timePos-startTime)/(endTime-startTime);

		Origin = eyePositionPaths[currentPath]->eval(uPos);
		Target = targetPaths[currentPath]->eval(uPos);

		recomputeViewPoint();
	}
}


void IViewPoint::addPath(C3DPath *eyePositionPath, C3DPath *targetPath)
{
	if ((eyePositionPath != NULL)&&(targetPath != NULL))
	{
		eyePositionPaths.push_back(eyePositionPath);
		targetPaths.push_back(targetPath);
	}
}

void IViewPoint::setCurrentPath(int nPath, bool continus, bool loop)
{
	if (nPath < (int)(eyePositionPaths.size()))
		currentPath = nPath;

	m_bContinus = continus;
	m_bLoop = loop;
}

//////////////////////////////////////////////////////////////////////
// Serialisation
//////////////////////////////////////////////////////////////////////
bool IViewPoint::exportObject(CRaptorIO& o)
{ 
	CPersistence::exportObject(o);

	o << Origin.X();
	o << Origin.Y();
	o << Origin.Z();
	o << Origin.H();

	o << Target.X();
	o << Target.Y();
	o << Target.Z();
	o << Target.H();

	return true;
}

void IViewPoint::ImportPaths(CRaptorIO& io)
{ 
    string name;
	io >> name; 
	string data = io.getValueName();

    C3DPath *tPath = NULL;
    C3DPath *ePath = NULL;

    while (!data.empty())
	{
	    if (data == "EyePath")
		{
		    ePath = new C3DPath();
            ePath->importObject(io);
		}
        else if (data == "TargetPath")
		{
		    tPath = new C3DPath();
            tPath->importObject(io);
		}
        else
			io >> name;

		data = io.getValueName();
	}

    io >> name;

    if ((tPath != NULL) && (ePath != NULL))
        addPath(ePath,tPath);
}

bool IViewPoint::importObject(CRaptorIO& io)
{ 
    string name;
	io >> name; 
	string data = io.getValueName();

    float tmin = -1.0f;
    float tmax = -1.0f;
    int nPath = 0;
    bool continus = true;
    bool loop = true;

	while (io.hasMoreValues())
	{
		if (data == "left")
			io >> viewVolume[0];
		else if (data == "right")
			io >> viewVolume[1];
		else if (data == "bottom")
			io >> viewVolume[2];
		else if (data == "top")
			io >> viewVolume[3];
		else if (data == "near")
			io >> viewVolume[4];
		else if (data == "far")
			io >> viewVolume[5];
        else if (data == "tmin")
			io >> tmin;
        else if (data == "tmax")
			io >> tmax;
        else if (data == "path")
			io >> nPath;
        else if (data == "continus")
        {
            int bVal = 0;
			io >> bVal;
            continus = (bVal > 0);
        }
        else if (data == "loop")
        {
            int bVal = 0;
			io >> bVal;
            loop = (bVal > 0);
        }
		else if (data == "model")
		{
			io >> name;
			if (name == "perspective")
				model = PERSPECTIVE;
			else if (name == "orthographic")
				model = ORTHOGRAPHIC;
		}
		else if (data == "EyePosition")
		{
			GL_COORD_VERTEX coord;
			io >> coord;
			setPosition(coord.x, coord.y, coord.z, IViewPoint::EYE);
		}
		else if (data == "TargetPosition")
		{
			GL_COORD_VERTEX coord;
			io >> coord;
			setPosition(coord.x, coord.y, coord.z, IViewPoint::TARGET);
		}
        else if (data == "ViewPath")
            ImportPaths(io);
        else if (data == "name")
			CPersistence::importObject(io);
		else
			io >> name;

		data = io.getValueName();
	}

	io >> name;

    setCurrentPath(nPath,continus,loop);
    if ((tmin >=0) && (tmax > 0))
        setTimeInterval(tmin,tmax);

	return true;
}




