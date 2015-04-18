#ifndef __INTERNAL_PROCS_H__
#define __INTERNAL_PROCS_H__


#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
	#include "GLHierarchy/Particle.h"
#endif


RAPTOR_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//	Image modifiers
//
extern void motionFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam);
extern void blowFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam);
extern void spinFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam);
extern void staticFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam);

///////////////////////////////////////////////////////////////////////////////
//	Physics default PFD
//
class CPhysics;
extern void defaultPfd(	CPhysics *physics, CGenericVector<float> &accel, CGenericVector<float> &raccel);

RAPTOR_NAMESPACE_END

#endif

