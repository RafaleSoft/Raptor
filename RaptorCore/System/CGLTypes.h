#ifndef __CGLTYPES_HPP__
#define __CGLTYPES_HPP__

//!	Sub-systems headers

#ifndef __SIMD_H__
	#include "SimdLib/SIMD.H"
#endif
#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_COLOR_H__3770AC59_0D0E_49EF_99C8_037268A33CE4__INCLUDED_)
	#include "System/Color.h"
#endif


RAPTOR_NAMESPACE_BEGIN

#ifndef __RAPTOR_VERSION_H__
	#include "System/Version.h"
#endif

//!
//!	Generic opaque pointers.
//!
//! A valid handle must not be NULL, i.e.
//! both the handle and the hClass attributes 
//! are striclty positive. hClass of handles are
//! divided in two parts:
//! - Raptor private handles, created and managed by Raptor
//!	- User handles, that Raptor can use but that identify client classes.
typedef struct RAPTOR_HANDLE
{
	unsigned int hClass;
	unsigned int handle;

	RAPTOR_HANDLE():hClass(0),handle(0) {}
    RAPTOR_HANDLE(unsigned int c,void* p):hClass(c),handle((unsigned int)p) {}
} RAPTOR_HANDLE;
typedef RAPTOR_HANDLE*	LP_RAPTOR_HANDLE;

//! Classes of handles are in 2 categories : Raptor handles and client handles.
#define	RAPTOR_HANDLE_CLASS		0x00000000
#define CLIENT_HANDLE_CLASS		0x00010000

#define DEVICE_CONTEXT_CLASS    0x00010000
#define WINDOW_CLASS            0x00010001
#define DIB_CLASS               0x00010002



//!
//!	Base window buffers configuration
//!
//!	These constants are passed to the graphic context factory
//!	using CGL_CRATE_STRUCT.display_mode. The display mode is
//!	set by combining these constants with a bitwise OR.
//!	Do not use them in the frame_mode parameter.
#define CGL_NULL			    0x00000000
#define CGL_RGB				    0x00000001
#define CGL_RGBA			    0x00000002
#define CGL_SINGLE			    0x00000004
#define CGL_DOUBLE			    0x00000008
#define CGL_DOUBLE_SWAPCOPY		0x00100008
#define CGL_DOUBLE_SWAPEXCHANGE 0x00200008
#define CGL_DOUBLE_SWAPUNDEF    0x00400008
#define CGL_DEPTH_16		    0x00000010
#define CGL_DEPTH_24		    0x00000020
#define CGL_DEPTH_32		    0x00000030 // CGL_DEPTH_16 | CGL_DEPTH_24
#define CGL_DEPTH			    CGL_DEPTH_24
#define CGL_STENCIL			    0x00000040
#define CGL_ANTIALIAS		    0x00000080
#define CGL_ANTIALIAS_2X	    0x10000080
#define CGL_ANTIALIAS_4X	    0x30000080
#define CGL_ANTIALIAS_5X	    0x40000080
#define CGL_ANTIALIAS_6X	    0x50000080
#define CGL_ANTIALIAS_8X	    0x70000080
#define CGL_ANTIALIAS_16X	    0xF0000080
#define CGL_SOFTWARE		    0x00000100
#define CGL_GENERIC			    0x00000200
#define CGL_HARDWARE		    0x00000400
#define CGL_OVERLAY			    0x00000800
#define CGL_RENDER_TEXTURE	    0x00001000
#define CGL_RENDER_2DTEXTURE	CGL_RENDER_TEXTURE
#define CGL_RENDER_DEPTHTEXTURE	0x01001000
#define CGL_RENDER_CUBETEXTURE	0x02001000
#define CGL_ACCUM			    0x00002000
#define CGL_RENDER_FILTERED	    0x00004000
#define CGL_RENDER_BUFFER	    0x00008000
#define CGL_FLOAT               0x00010002 // float format is only used with rgba
#define CGL_FLOAT_16			0x00030002
#define CGL_FLOAT_32			0x00050002


//////////////////////////////////////////////////////////////////////
//	Texture objects
#define	CGL_AUTO_MIPMAPPED		0x00002000
#define CGL_USER_MIPMAPPED		0x00004000

#define CGL_CUBEMAP_PX			0x0000C010
#define CGL_CUBEMAP_NX			0x0000C020
#define CGL_CUBEMAP_PY			0x0000C030
#define CGL_CUBEMAP_NY			0x0000C040
#define CGL_CUBEMAP_PZ			0x0000C050
#define CGL_CUBEMAP_NZ			0x0000C060
#define CGL_NORMAL_MAP			0x01000000
#define CGL_CREATE_NORMAL_MAP	0x03000000


//////////////////////////////////////////////////////////////////////
//	Mathematical defines
//
#include <math.h>		//	Quite obvious
#include <limits.h>		//	Data type min/max values and settings
#include <float.h>		//	idem for fpu
//
#if !defined(RANDOM)	// returns a random value between -0.5 and 0.5
	#define RANDOM (((float)(rand()-(RAND_MAX/2)))/RAND_MAX)
#endif
#if !defined(PI)
	#define PI 3.1415926535897932384626433832795
#endif
#if !defined(TWO_PI)
	#define TWO_PI 6.283185307179586476925286766559
#endif
#if !defined(ABS)
	#define ABS(x) (x)>0?(x):-(x)
#endif
#if !defined(RAD)
	#define TO_RADIAN(x) ((x) * 0.017453292519943295769236907684886)	//x*PI/180.0f
#endif
#if !defined(DEG)
	#define TO_DEGREE(x) ((x) * 57.295779513082320876798154814105)	//x*180.0f/PI
#endif
#if !defined(SGN)
	#define SGN(x) (x)>=0?(1):(-1)
#endif
#if !defined(MIN)
	#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#if !defined(MAX)
	#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#if !defined(ONE_OVER_256)
	#define ONE_OVER_256 0.00390625
	#define ONE_OVER_256_F 0.00390625f
#endif
//
//	own data structures for fast access
//	( faster than classes, data can also
//	be tightly packed into registers )
//	These structure types match opengl data storage
//	for vertex and matrix and can be safely cast
//	in function calls such as glMultMatrixf(...)
typedef struct GL_COORD_VERTEX_TAG
{
	float x;	//	3D coordinates
	float y;
	float z;
	float h;	// homogenous component

	bool operator==(const GL_COORD_VERTEX_TAG &r_v)
	{
		return (*((int*)&x) == *((int*)&r_v.x) &&
				*((int*)&y) == *((int*)&r_v.y) &&
				*((int*)&z) == *((int*)&r_v.z));
	}
	bool operator==(float val)
	{
		return (*((int*)&x) == *((int*)&val) &&
				*((int*)&y) == *((int*)&val) &&
				*((int*)&z) == *((int*)&val));
	}

	const GL_COORD_VERTEX_TAG& operator=(const GL_COORD_VERTEX_TAG& r_v)
	{
		x = r_v.x;
		y = r_v.y;
		z = r_v.z;
		h = r_v.h;
		return *this;
	}

	operator float*() { return (float*)(this); }
	operator const float*() const { return (const float*)this; };

	GL_COORD_VERTEX_TAG(float _x = 0.0f,float _y = 0.0f,float _z = 0.0f,float _h = 1.0f)
		:x(_x),y(_y),z(_z),h(_h)
	{
	}
} GL_COORD_VERTEX;
typedef GL_COORD_VERTEX *LP_GL_COORD_VERTEX;

typedef struct GL_HIRES_COORD_VERTEX_TAG
{
	double x;	//	3D coordinates
	double y;
	double z;
	double h;	// homogenous component

	const GL_HIRES_COORD_VERTEX_TAG& operator=(const GL_HIRES_COORD_VERTEX_TAG& r_v)
	{
		x = r_v.x;
		y = r_v.y;
		z = r_v.z;
		h = r_v.h;
		return *this;
	}

	operator double*() { return (double*)(this); }
	operator const double*() const { return (const double*)this; };

	GL_HIRES_COORD_VERTEX_TAG(double _x = 0.0f,double _y = 0.0f,double _z = 0.0f,double _h = 1.0f)
		:x(_x),y(_y),z(_z),h(_h)
	{
	}
} GL_HIRES_COORD_VERTEX;
typedef GL_HIRES_COORD_VERTEX *LP_GL_HIRES_COORD_VERTEX;

typedef struct GL_TEX_VERTEX_TAG
{
	float u;	// texture coordinates
	float v;

	bool operator==(const GL_TEX_VERTEX_TAG &r_v)
	{
		return (*((int*)&u) == *((int*)&r_v.u) &&
				*((int*)&v) == *((int*)&r_v.v));
	}
	bool operator==(float val)
	{
		return (*((int*)&u) == *((int*)&val) &&
				*((int*)&v) == *((int*)&val));
	}

	operator float*() { return (float*)(this); }
	operator const float*() const { return (const float*)this; };

	GL_TEX_VERTEX_TAG(float _u = 0.0f,float _v = 0.0f)
		:u(_u),v(_v)
	{
	}
} GL_TEX_VERTEX;
typedef GL_TEX_VERTEX *LP_GL_TEX_VERTEX;

//	t => translation vector
//	R => orientation matrix
//	H => homogenous vector
//	x' = M*x => x' =  ( R*x + x.h*t ) / H*x
typedef struct  GL_MATRIX_TAG
{											
	GL_COORD_VERTEX rowx;//	rx.x rx.y rx.z t.x
	GL_COORD_VERTEX rowy;//	ry.x ry.y ry.z t.y
	GL_COORD_VERTEX rowz;//	rz.x rz.y rz.z t.z
	GL_COORD_VERTEX rowh;//	h.x h.y h.z h.h

	operator float*() { return (float*)(this); }
	operator const float*() const { return (const float*)this; };

} GL_MATRIX;
typedef GL_MATRIX *LP_GL_MATRIX;


#define ZERO_COORD_VERTEX(v) { v.x=0.0f;v.y=0.0f;v.z=0.0f;v.h=1.0f; }
#define NULL_COORD_VERTEX(v) { v.x=0.0f;v.y=0.0f;v.z=0.0f;v.h=0.0f; }
#define NULL_TEX_VERTEX(t) { t.u=0.0f;t.v=0.0f; }

#define NULL_VERTEX(v) \
	{ NULL_COORD_VERTEX(v.coords) \
	NULL_TEX_VERTEX(v.texcoords) \
	v.w = 0.0f; \
	v.f = 0.0f; }

#define ZERO_VERTEX(v) \
	{ ZERO_COORD_VERTEX(v.coords) \
	NULL_TEX_VERTEX(v.texcoords) \
	v.w = 0.0f; \
	v.f = 0.0f; }

#define NULL_MATRIX(m) \
	{ NULL_COORD_VERTEX(m.rowx)\
	NULL_COORD_VERTEX(m.rowy)\
	NULL_COORD_VERTEX(m.rowz)\
	NULL_COORD_VERTEX(m.rowh) }

#define ZERO_MATRIX(m) \
	{ NULL_COORD_VERTEX(m.rowx)\
	NULL_COORD_VERTEX(m.rowy)\
	NULL_COORD_VERTEX(m.rowz)\
	ZERO_COORD_VERTEX(m.rowh) }

#define IDENT_MATRIX(m) \
	{ ZERO_MATRIX(m)\
	m.rowx.x = m.rowy.y = m.rowz.z = 1.0; }

#define RX_MATRIX(m,a)\
	{ IDENT_MATRIX(m)\
	m.rowz.z = m.rowy.y = (float)cos(TO_RADIAN(a));\
	m.rowy.z = -(float)sin(TO_RADIAN(a));\
	m.rowz.y = (float)sin(TO_RADIAN(a)); }

#define RY_MATRIX(m,a)\
	{ IDENT_MATRIX(m)\
	m.rowz.z = m.rowx.x = (float)cos(TO_RADIAN(a));\
	m.rowx.z = -(float)sin(TO_RADIAN(a));\
	m.rowz.x = (float)sin(TO_RADIAN(a)); }

#define RZ_MATRIX(m,a)\
	{ IDENT_MATRIX(m)\
	m.rowy.y = m.rowx.x = (float)cos(TO_RADIAN(a));\
	m.rowx.y = -(float)sin(TO_RADIAN(a));\
	m.rowy.x = (float)sin(TO_RADIAN(a)); }


typedef struct GL_VERTEX_DATA_TAG
{
	GL_COORD_VERTEX	vertex;
	GL_COORD_VERTEX	normal;
    GL_TEX_VERTEX	texCoord0;
	CColor::RGBA	color;
	CColor::RGBA	secondaryColor;
	GL_COORD_VERTEX	tangent;
	GL_COORD_VERTEX	binormal;
    float			fog;
    float			weight;
} GL_VERTEX_DATA;
typedef GL_VERTEX_DATA *LP_GL_VERTEX_DATA;


//
//	Helper functions for easier compatibility
//
void glGetTransposeFloatv(int e,GL_MATRIX &m);
void glGetTransposeFloatv(int e,CGenericMatrix<float> &m);


//
//	Critical sections management for Raptor classes
//
class RAPTOR_API CRaptorLock;

class RAPTOR_API CRaptorMutex  
{
public:
	CRaptorMutex();
	virtual ~CRaptorMutex();

	void	lock () const;
    void	unlock () const;

private:
	CRaptorMutex (const CRaptorMutex& M);
	void operator = (const CRaptorMutex& M);

#if defined(_WIN32) || defined(_WIN64)
	mutable CRITICAL_SECTION _mutex;
#elif defined(LINUX)
	mutable pthread_mutex_t _mutex;
#endif
};

class RAPTOR_API CRaptorLock
{
public:
    CRaptorLock(const CRaptorMutex& m)
      :_mutex (m),_locked (true)
    { _mutex.lock(); }
    
    ~CRaptorLock()
    { if (_locked) _mutex.unlock(); }
    
    void acquire(void)
    { _mutex.lock(); _locked = true; }
    
    void release(void)
    { _locked = false; _mutex.unlock(); }
    
    bool locked() { return _locked; }

private:
    const CRaptorMutex&	_mutex;
    bool		_locked;
};

class RAPTOR_API CRaptorSemaphore
{
public:
	CRaptorSemaphore(int value=0);
	virtual ~CRaptorSemaphore();

	void	P() const;
    void	V() const;

private:
	CRaptorSemaphore (const CRaptorSemaphore& M);
	void operator = (const CRaptorSemaphore& M);

#if defined(_WIN32) || defined(_WIN64)
	mutable HANDLE _sem;
#elif defined(LINUX)
	mutable sem_t _sem;
#endif
};

RAPTOR_NAMESPACE_END

#endif

