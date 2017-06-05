// ViewPoint.cpp: implementation of the CViewPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "ViewPoint.h"
#endif

#if !defined(AFX_3DPATH_H__6AD45CFB_C7F6_4F7B_BFF6_932A812A770E__INCLUDED_)
	#include "3DPath.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif


#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif


RAPTOR_NAMESPACE

static CViewPoint::CViewPointClassID viewId;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CViewPoint::CViewPoint(const std::string& name):
	CPersistence(viewId,name),
	currentPath(-1),timePos(0),startTime(0),endTime(1),
	m_lfAlpha(0.0),m_lfBeta(0.0),m_lfGamma(0.0),m_lfLength(0.0),
	m_bContinus(true),m_bLoop(true),
	model(CViewPoint::PERSPECTIVE)
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

CViewPoint::~CViewPoint()
{
}

void CViewPoint::setPosition(float x,float y,float z,VIEW_POINT_POSITION p)
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

void CViewPoint::setViewVolume(float left, float right,
							   float bottom, float up,
							   float n, float f,
							   VIEW_POINT_MODEL m)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((right == left) ||
		(bottom == up) ||
		(near == far))
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

void CViewPoint::getViewVolume(	float &left,float &right,
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

void CViewPoint::getFrustum(CGenericMatrix<float, 4>& frustum) const
{
	frustum.Zero();

	float l = viewVolume[0];
	float r = viewVolume[1];
	float b = viewVolume[2];
	float t = viewVolume[3];
	float n = viewVolume[4];
	float f = viewVolume[5];

	if (CViewPoint::PERSPECTIVE == model)
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
	else if (CViewPoint::ORTHOGRAPHIC == model)
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

CGenericVector<float> CViewPoint::getPosition(VIEW_POINT_POSITION p) const
{
    switch(p)
    {
        case EYE:
        {
			/*
            CGenericMatrix<float> T;
	        CGenericMatrix<float> R;

	        //	first compute the actual transform
	        CGenericVector<float> ax = Rotate;
	        C3DEngine::Get3DEngine()->generateRotation(m_lfAlpha,!ax,R);
	        T.Ident();
	        T[3] = -Origin.X();
	        T[7] = -Origin.Y();
	        T[11] = -Origin.Z();

	        R *= T;

	        T[3] = Position.X();
	        T[7] = Position.Y();
	        T[11] = Position.Z();

	        T *= R;

	        //	Now solve to find Origin translation
	        //	T.x = O
	        CGenericVector<float> b(T[3],T[7],T[11],1.0f);
	        C3DEngine::Get3DEngine()->solve(b,R,ax);

	        return ax;
			*/
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
void CViewPoint::translate(float tx,float ty,float tz)
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

void CViewPoint::scale(float sx,float sy,float sz)
{
	Scale.X() *= sx;
	Scale.Y() *= sy;
	Scale.Z() *= sz;
}

void CViewPoint::rotationZ(float rz)
{
	m_lfGamma += rz;
}

void CViewPoint::rotationX(float rx)
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

void CViewPoint::rotationY(float ry)
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
void CViewPoint::recomputeViewPoint(void)
{
	CGenericVector<float> v = Target - Origin;
	
	m_lfLength = v.Normalize();

    m_lfAlpha = TO_DEGREE(atan2(v.X(),v.Z())) + 180.0;
    m_lfBeta = -TO_DEGREE(atan2(sqrt(v.X()*v.X()+v.Z()*v.Z()),v.Y())) + 90.0;
}


void CViewPoint::glRender(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScalef(Scale.X(),Scale.Y(),Scale.Z());

	glRotatef(m_lfGamma,0.0f,0.0f,1.0f);
	glRotatef(-m_lfBeta,1.0f,0.0f,0.0f);
	glRotatef(-m_lfAlpha,0.0f,1.0f,0.0f);

	glTranslatef(-Origin.X(),-Origin.Y(),-Origin.Z());

	C3DEngine::Get3DEngine()->glConfigureEngine(this);

	CATCH_GL_ERROR
}

void CViewPoint::glRenderViewPointModel(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch(model)
	{
		case ORTHOGRAPHIC:
			glOrtho(viewVolume[0],viewVolume[1],
					viewVolume[2],viewVolume[3],
					viewVolume[4],viewVolume[5]);
			break;
		case PERSPECTIVE:
			glFrustum(	viewVolume[0],viewVolume[1],
						viewVolume[2],viewVolume[3],
						viewVolume[4],viewVolume[5]);
	}

	glMatrixMode(GL_MODELVIEW);

	//glConfigureEngine(this);

	CATCH_GL_ERROR
}


void CViewPoint::setTimeInterval(float tMin, float tMax)
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

void CViewPoint::setTimePos(float tpos)
{ 
	if ((tpos>endTime)||(tpos<startTime))
		timePos = startTime;
	else
		timePos = tpos; 
}

void CViewPoint::trackObject(CObject3D *object, VIEW_POINT_POSITION p)
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
 
void CViewPoint::trackObject(CLight *object, VIEW_POINT_POSITION p)
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

void RAPTOR_FASTCALL CViewPoint::deltaTime(float dt)
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


void CViewPoint::addPath(C3DPath *eyePositionPath,C3DPath *targetPath)
{
	if ((eyePositionPath != NULL)&&(targetPath != NULL))
	{
		eyePositionPaths.push_back(eyePositionPath);
		targetPaths.push_back(targetPath);
	}
}

void CViewPoint::setCurrentPath(int nPath,bool continus,bool loop)
{
	if (nPath < (int)(eyePositionPaths.size()))
		currentPath = nPath;

	m_bContinus = continus;
	m_bLoop = loop;
}

//////////////////////////////////////////////////////////////////////
// Serialisation
//////////////////////////////////////////////////////////////////////
bool CViewPoint::exportObject(CRaptorIO& o) 
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

void CViewPoint::ImportPaths(CRaptorIO& io) 
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

bool CViewPoint::importObject(CRaptorIO& io) 
{ 
    string name;
	io >> name; 
	string data = io.getValueName();

    float tmin = -1.0f;
    float tmax = -1.0f;
    int nPath = 0;
    bool continus = true;
    bool loop = true;

	while (!data.empty())
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
            setPosition(coord.x,coord.y,coord.z,CViewPoint::EYE);
		}
		else if (data == "TargetPosition")
		{
			GL_COORD_VERTEX coord;
			io >> coord;
            setPosition(coord.x,coord.y,coord.z,CViewPoint::TARGET);
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




