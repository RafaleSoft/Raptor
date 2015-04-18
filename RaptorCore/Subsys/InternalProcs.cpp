#include "Subsys/CodeGeneration.h"


#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_PHYSICS_H__B42ABB89_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "Engine/Physics.h"
#endif
#if !defined(AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_)
    #include "GLHierarchy/PerlinNoise.h"
#endif


RAPTOR_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////
//
//	Stock Modifier functions
//
//
void motionFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam)
{
#ifdef RAPTOR_SSE_CODE_GENERATION
	__asm
	{
		mov edx,height
		mov ecx,width

		mov esi,src
		mov edi,dst

		pxor mm6,mm6	// 0 a register to prepare word expansion
		mov eax,width
		mov ebx,dwParam
		shl eax,2
		movd mm5,ebx	// fading parameter ( substracted to result )
		add esi,eax		// to perform motion : source pixels are
						// one line below destination
		punpcklbw mm5,mm6
line:
		// read pixels
		//	... on current line ...
		movd mm1,[esi-4]
		movd mm0,[esi+4]
		punpcklbw mm1,mm6
		punpcklbw mm0,mm6
		movq mm7,mm1
		add esi,eax
		paddsw mm7,mm0

		// ... on line below ...
		movd mm1,[esi]
		movd mm0,[esi-4]
		punpcklbw mm1,mm6
		punpcklbw mm0,mm6
		paddsw mm7,mm1
		movd mm1,[esi+4]
		paddsw mm7,mm0
		sub esi,eax
		punpcklbw mm1,mm6
		sub esi,eax
		paddsw mm7,mm1
		
		// ... on line over ...
		movd mm0,[esi]
		movd mm1,[esi+4]
		punpcklbw mm0,mm6
		punpcklbw mm1,mm6
		paddsw mm7,mm0
		movd mm0,[esi-4]
		paddsw mm7,mm1
		punpcklbw mm0,mm6
		paddsw mm7,mm0

		// compute fading
		add esi,eax
		psrlw mm7,3
		movd mm0,[esi]		
		punpcklbw mm0,mm6
		paddsw mm7,mm0
		psrlw mm7,1
		psubsw mm7,mm5

		// store pixel
		packuswb mm7,mm6
		movd [edi],mm7

		add esi,4		// next src/dst pixel
		add edi,4

		dec ecx
		jnz line

		mov ecx,width
		dec edx
		jnz line

		emms
	}
#endif
}


void blowFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam)
{
	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;
	unsigned char *finaloffset = 0;

	int xinc = 0;
	int yinc = 0;

	int dx = 0;
	int dy = 0;
	unsigned int width4 = width * 4;

	unsigned int val = 0;

#ifdef RAPTOR_SSE_CODE_GENERATION
	__asm
	{
		pxor mm6,mm6	// 0 a register to prepare word expansion
		mov ebx,dwParam
		movd mm5,ebx	// fading parameter ( substracted to result )	
		punpcklbw mm5,mm6
	}

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			dx = j - (width >> 1);
			dy = i - (height >> 1);

			if (dx < 0)
				xinc = 4;
			else
				xinc = -4;

			if (dy < 0)
				yinc = (width << 2);
			else
				yinc = -(width << 2);

			dx = abs(dx);
			dy = abs(dy);

			if (dx>dy)
				finaloffset = ofssrc+xinc;
			else if (dx<dy)
				finaloffset = ofssrc+yinc;
			else
				finaloffset = ofssrc+xinc+yinc;

			__asm
			{
				mov esi,finaloffset
				mov edi,ofsdst
				mov eax,width4
				// read pixels
				//	... on current line ...
				movd mm1,[esi-4]
				movd mm0,[esi+4]
				punpcklbw mm1,mm6
				punpcklbw mm0,mm6
				movq mm7,mm1
				add esi,eax
				paddsw mm7,mm0

				// ... on line below ...
				movd mm1,[esi]
				movd mm0,[esi-4]
				punpcklbw mm1,mm6
				punpcklbw mm0,mm6
				paddsw mm7,mm1
				movd mm1,[esi+4]
				paddsw mm7,mm0
				sub esi,eax
				punpcklbw mm1,mm6
				sub esi,eax
				paddsw mm7,mm1
				
				// ... on line over ...
				movd mm0,[esi]
				movd mm1,[esi+4]
				punpcklbw mm0,mm6
				punpcklbw mm1,mm6
				paddsw mm7,mm0
				movd mm0,[esi-4]
				paddsw mm7,mm1
				punpcklbw mm0,mm6
				paddsw mm7,mm0

				// compute fading
				add esi,eax
				psrlw mm7,3
				movd mm0,[esi]		
				punpcklbw mm0,mm6
				paddsw mm7,mm0
				psrlw mm7,1
				psubsw mm7,mm5

				// store pixel
				packuswb mm7,mm6
				movd [edi],mm7
			}

			ofsdst += 4;
			ofssrc += 4;
		}
	}
	__asm emms;
#endif

}

void spinFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam)
{
	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;
	unsigned char *finaloffset = 0;

	int xinc = 0;
	int yinc = 0;

	int dx = 0;
	int dy = 0;
	unsigned int width4 = width * 4;

	unsigned int val = 0;

#ifdef RAPTOR_SSE_CODE_GENERATION
	__asm
	{
		pxor mm6,mm6	// 0 a register to prepare word expansion
		mov ebx,dwParam
		movd mm5,ebx	// fading parameter ( substracted to result )	
		punpcklbw mm5,mm6
	}

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			dx = j - (width >> 1);
			dy = i - (height >> 1);

			xinc = 4;
			yinc = (width << 2);

			if (dx > 0)
			{
				if (dy > 0)
					finaloffset = ofssrc-yinc;
				else
					finaloffset = ofssrc-xinc;
			}
			else
			{
				if (dy > 0)
					finaloffset = ofssrc+xinc;
				else
					finaloffset = ofssrc+yinc;
			}

			__asm
			{
				mov esi,finaloffset
				mov edi,ofsdst
				mov eax,width4
				// read pixels
				//	... on current line ...
				movd mm1,[esi-4]
				movd mm0,[esi+4]
				punpcklbw mm1,mm6
				punpcklbw mm0,mm6
				movq mm7,mm1
				add esi,eax
				paddsw mm7,mm0

				// ... on line below ...
				movd mm1,[esi]
				movd mm0,[esi-4]
				punpcklbw mm1,mm6
				punpcklbw mm0,mm6
				paddsw mm7,mm1
				movd mm1,[esi+4]
				paddsw mm7,mm0
				sub esi,eax
				punpcklbw mm1,mm6
				sub esi,eax
				paddsw mm7,mm1
				
				// ... on line over ...
				movd mm0,[esi]
				movd mm1,[esi+4]
				punpcklbw mm0,mm6
				punpcklbw mm1,mm6
				paddsw mm7,mm0
				movd mm0,[esi-4]
				paddsw mm7,mm1
				punpcklbw mm0,mm6
				paddsw mm7,mm0

				// compute fading
				add esi,eax
				psrlw mm7,3
				movd mm0,[esi]		
				punpcklbw mm0,mm6
				paddsw mm7,mm0
				psrlw mm7,1
				psubsw mm7,mm5

				// store pixel
				packuswb mm7,mm6
				movd [edi],mm7
			}

			ofsdst += 4;
			ofssrc += 4;
		}
	}

	__asm emms;
#endif

}


void staticFader(int width,int height,unsigned char *src,unsigned char *dst,unsigned long dwParam)
{
#ifdef RAPTOR_SSE_CODE_GENERATION
	__asm
	{
		mov edx,height
		mov ecx,width

		mov esi,src
		mov edi,dst

		pxor mm6,mm6	// 0 a register to prepare word expansion
		mov eax,width
		mov ebx,dwParam
		shl eax,2
		movd mm5,ebx	// fading parameter ( substracted to result )

		punpcklbw mm5,mm6
line:
		// read pixels
		//	... on current line ...
		movd mm1,[esi-4]
		movd mm0,[esi+4]
		punpcklbw mm1,mm6
		punpcklbw mm0,mm6
		movq mm7,mm1
		add esi,eax
		paddsw mm7,mm0

		// ... on line below ...
		movd mm1,[esi]
		movd mm0,[esi-4]
		punpcklbw mm1,mm6
		punpcklbw mm0,mm6
		paddsw mm7,mm1
		movd mm1,[esi+4]
		paddsw mm7,mm0
		sub esi,eax
		punpcklbw mm1,mm6
		sub esi,eax
		paddsw mm7,mm1
		
		// ... on line over ...
		movd mm0,[esi]
		movd mm1,[esi+4]
		punpcklbw mm0,mm6
		punpcklbw mm1,mm6
		paddsw mm7,mm0
		movd mm0,[esi-4]
		paddsw mm7,mm1
		punpcklbw mm0,mm6
		paddsw mm7,mm0

		// compute fading
		add esi,eax
		psrlw mm7,3
		movd mm0,[esi]		
		punpcklbw mm0,mm6
		paddsw mm7,mm0
		psrlw mm7,1
		psubsw mm7,mm5

		// store pixel
		packuswb mm7,mm6
		movd [edi],mm7

		add esi,4		// next src/dst pixel
		add edi,4

		dec ecx
		jnz line

		mov ecx,width
		dec edx
		jnz line

		emms
	}
#endif
}



/////////////////////////////////////////////////////////////////////////////
//	Export a default physics behaviour : the standard PFD
void defaultPfd(	CPhysics *physics,
					CGenericVector<float> &accel,
					CGenericVector<float> &raccel)
{
	int nb = physics->getNbForces();
	
	if (nb>0)
	{
        CPhysics::CForce *f = physics->getForce(0);


		if (f->vector_attached)
			accel = CPhysics::Rotation * f->vect;
		else
			accel = f->vect;

		if (f->position_attached)
			CPhysics::dV = ( (CPhysics::Rotation * f->pos) - physics->G );
		else
			CPhysics::dV = ( f->pos - physics->G );
		
		raccel = ( CPhysics::dV ^ accel);
		
		if (nb>1)
		for (int i=1;i<nb;i++)
		{	
			f = physics->getForce(i);

			if (f->vector_attached)
				accel += CPhysics::Rotation * f->vect;
			else
				accel += f->vect;

			if (f->position_attached)
				CPhysics::dV = ( (CPhysics::Rotation * f->pos) - physics->G );
			else
				CPhysics::dV = ( f->pos - physics->G );

			if (f->vector_attached)
				raccel += ( CPhysics::dV ^ (CPhysics::Rotation * f->vect) );
			else
				raccel += ( CPhysics::dV ^ f->vect );
		}

		//	compute result :
		//	- acceleration
		//	- angular acceleration
		float one_over_mass = 1/physics->mass;
		raccel.H(0.0f);
		accel.H(0.0f);
		accel *= one_over_mass;
		raccel *= one_over_mass;

		// now, remove the diracs
		for (int i=nb-1;i>=0;i--)
		{
            f = physics->getForce(i);
			if (f->dirac)
			{
				physics->removeForce(f);
			}
		}
	}
	else
	{
		accel.Zero();
		raccel.Zero();
	}
}


//////////////////////////////////////////////////////////////////////

RAPTOR_NAMESPACE_END


