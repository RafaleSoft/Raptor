// PerlinNoise.cpp: implementation of the CPerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERLINNOISE_H__AE39D006_3DDA_4924_B76D_A11A0F2BCFB9__INCLUDED_)
    #include "PerlinNoise.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static const int PERMUTATION_SIZE = 256;

float interpolator(float t)
{
    return t * t * t * ( 10 - t * ( 15 - 6 * t ));
}

__inline float lerp(float t, float a, float b)
{
    return (a + t * (b - a));
}

//////////////////////////////////////////////////////////////////////
//! Here under is the original Ken Perlin's noise implementation.
//! As he improved the visual result of his noise function, we will use
//! the new interpolator and gradients instead. 
//! Maybe we could add a parameter to the class, allowing the use
//! of both algorithms.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);

#define s_curve(t) ( t * t * (3. - 2. * t) )


#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

double noise1(double arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

static void normalize2(float v[2])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

static void init(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

RAPTOR_NAMESPACE_END

//////////////////////////////////////////////////////////////////////


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPerlinNoise::CPerlinNoise(const CVaArray<OPS>& ops)
	:m_ImageOps(ops),m_model(NOISE1),m_textureMirror(false),
	m_amplitude(1.0f),m_baseFrequency(1.0f/128.0f)
{
    m_iPermutation = new unsigned int[PERMUTATION_SIZE+PERMUTATION_SIZE];
    srand(time(0));

    init();

    unsigned int i = 0;
    for (i=0;i<PERMUTATION_SIZE;i++)
        m_iPermutation[i] = i;

    for (i=0;i<PERMUTATION_SIZE;i++)
    {
        unsigned int j = rand() % PERMUTATION_SIZE;
        unsigned int p = m_iPermutation[j];
        m_iPermutation[j] = m_iPermutation[i];
        m_iPermutation[i+PERMUTATION_SIZE] = m_iPermutation[i] = p;
    }

    G[0] = GL_COORD_VERTEX(1.0f, 1.0f, 0.0f, 1.0f);
    G[1] = GL_COORD_VERTEX(-1.0f, 1.0f, 0.0f, 1.0f);
    G[2] = GL_COORD_VERTEX(1.0f, -1.0f, 0.0f, 1.0f);
    G[3] = GL_COORD_VERTEX(-1.0f, -1.0f, 0.0f, 1.0f);

    G[4] = GL_COORD_VERTEX(1.0f, 0.0f, 1.0f, 1.0f);
    G[5] = GL_COORD_VERTEX(-1.0f, 0.0f, 1.0f, 1.0f);
    G[6] = GL_COORD_VERTEX(1.0f, 0.0f, -1.0f, 1.0f);
    G[7] = GL_COORD_VERTEX(-1.0f, 0.0f, -1.0f, 1.0f);

    G[8] = GL_COORD_VERTEX(0.0f, 1.0f, 1.0f, 1.0f);
    G[9] = GL_COORD_VERTEX(0.0f, -1.0f, 1.0f, 1.0f);
    G[10] = GL_COORD_VERTEX(0.0f, 1.0f, -1.0f, 1.0f);
    G[11] = GL_COORD_VERTEX(0.0f, -1.0f, -1.0f, 1.0f);

    G[12] = GL_COORD_VERTEX(1.0f, 1.0f, 0.0f, 1.0f);
    G[13] = GL_COORD_VERTEX(-1.0f, 1.0f, 0.0f, 1.0f);
    G[14] = GL_COORD_VERTEX(0.0f, -1.0f, 1.0f, 1.0f);
    G[15] = GL_COORD_VERTEX(0.0f, -1.0f, -1.0f, 1.0f);
}

CPerlinNoise::~CPerlinNoise()
{
    delete [] m_iPermutation;
}

unsigned int CPerlinNoise::getGenerateWidth(void) const
{
    // TODO
    return 0;
}

unsigned int CPerlinNoise::getGenerateHeight(void) const
{
    // TODO
    return 0;
}

void CPerlinNoise::glGenerate(CTextureObject* t)
{
    if ((t == NULL) || (!m_bEnabled))
        return;

	CImage noise;
	noise.allocatePixels(t->getWidth(), t->getHeight());
	
	unsigned char *data = noise.getPixels();
	generateNoise(data,t->getWidth(),t->getHeight(),t->getDepth(),t->getTransparency());

	CTextureFactory &f = CTextureFactory::getDefaultFactory();

	//CImage::IImageOP::operation_param_t param;
	//param.bump_scale = f.getConfig().getBumpAmplitude();
	//param.transparency = t->getTransparency();
	noise.loadImage(".buffer", m_ImageOps); // , param);
		
	f.glLoadTexture(t, noise);

    CATCH_GL_ERROR
}

void CPerlinNoise::generateNoise(unsigned char *data,
								 unsigned int width,
								 unsigned int height,
								 unsigned int depth,
								 unsigned int transparency)
{
	unsigned int w = width;
	unsigned int h = height;
	unsigned int d = MAX(depth,1);
	if (m_textureMirror)
	{
		w = MAX(w >> 1,1);
		h = MAX(h >> 1,1);
		d = MAX(d >> 1,1);
	}
	
	for (unsigned int l=0;l<d;l++)
	{
		for (unsigned int j=0;j<h;j++)
		{
			for (unsigned int i=0;i<w;i++)
			{
				float n = 0.0f;
				float amplitude = m_amplitude;
				switch (m_model)
				{
					case NOISE1:
					{
						float frequency = m_baseFrequency;
						
						for (unsigned int k=0;k<8;k++)
						{
							n += amplitude * fabs(noise(i * frequency,j * frequency,l * frequency));
							frequency *= 2;
							amplitude *= 0.5f;
						}
						break;
					}
					case NOISE2:
					{
						float frequency = 4.0f * m_baseFrequency;
						for (unsigned int k=0;k<8;k++)
						{
							n += amplitude * (noise(i * frequency,j * frequency,l * frequency));
							frequency *= 2;
							amplitude *= 0.5f;
						}
						break;
					}
					case NOISE3:
					{
						float frequency = 4.0f * m_baseFrequency;
						for (unsigned int k=0;k<8;k++)
						{
							n += amplitude * (noise(i * frequency,j * frequency,l * frequency));
							frequency *= 2;
							amplitude *= 0.5f;
						}
						n = n - 1.0f;
						break;
					}
				}

				n = 128 * (1 - n);
				if (n > 255 ) 
					n = 255;
				else if (n < 0) 
					n = 0;

				unsigned int offset = 4*((l*width + j)*height + i);
				data[offset] = n;
				data[offset+1] = n;
				data[offset+2] = n;
				data[offset+3] = transparency;

				if (m_textureMirror)
				{
					offset = 4*((j+1)*width - i);
					data[offset - 4] = n;
					data[offset - 3] = n;
					data[offset - 2] = n;
					data[offset - 1] = transparency;

					offset = 4*((height-j-1)*width + i);
					data[offset] = n;
					data[offset+1] = n;
					data[offset+2] = n;
					data[offset+3] = transparency;

					offset = 4*((height-j)*width - i);
					data[offset - 4] = n;
					data[offset - 3] = n;
					data[offset - 2] = n;
					data[offset - 1] = transparency;
				}
			}
		}
	}
}

float CPerlinNoise::noise(float x, float y, float z)
{
    float X =  floor(x);
    float Y =  floor(y);
    float Z =  floor(z);

    int i0 = (int)X & 255;
    int j0 = (int)Y & 255;
    int k0 = (int)Z & 255;
    X = x - X;
    Y = y - Y;
    Z = z - Z;

    unsigned int p_i0j0 = m_iPermutation[i0] + j0;
    unsigned int p_i1j0 = m_iPermutation[i0+1] + j0;

    unsigned int p_i0j0k0 = m_iPermutation[p_i0j0] + k0;
    unsigned int p_i1j0k0 = m_iPermutation[p_i1j0] + k0;

    unsigned int p_i0j1k0 = m_iPermutation[p_i0j0+1] + k0;
    unsigned int p_i1j1k0 = m_iPermutation[p_i1j0+1] + k0;

    float g_i0j0k0 = grad(p_i0j0k0,X,Y,Z);
    float g_i1j0k0 = grad(p_i1j0k0,X-1,Y,Z);
    float g_i0j1k0 = grad(p_i0j1k0,X,Y-1,Z);
    float g_i1j1k0 = grad(p_i1j1k0,X-1,Y-1,Z);

    float g_i0j0k1 = grad(p_i0j0k0+1,X,Y,Z-1);
    float g_i1j0k1 = grad(p_i1j0k0+1,X-1,Y,Z-1);
    float g_i0j1k1 = grad(p_i0j1k0+1,X,Y-1,Z-1);
    float g_i1j1k1 = grad(p_i1j1k0+1,X-1,Y-1,Z-1);

    float u = interpolator(X);
    float v = interpolator(Y);
    float w = interpolator(Z);

    float X1 = lerp(u,g_i0j0k0,g_i1j0k0);
    float X2 = lerp(u,g_i0j1k0,g_i1j1k0);
    float X3 = lerp(u,g_i0j0k1,g_i1j0k1);
    float X4 = lerp(u,g_i0j1k1,g_i1j1k1);

    return lerp(w,lerp(v,X1,X2),lerp(v,X3,X4));
}

float CPerlinNoise::grad(unsigned int hash, float x, float y, float z)
{
    GL_COORD_VERTEX &g = G[m_iPermutation[hash] & 15];
    return g.x * x + g.y * y + g.z * z;
}
/*
double noise(double x, double y, double z) {
      int X = (int)Math.floor(x) & 255,                  // FIND UNIT CUBE THAT
          Y = (int)Math.floor(y) & 255,                  // CONTAINS POINT.
          Z = (int)Math.floor(z) & 255;
      x -= Math.floor(x);                                // FIND RELATIVE X,Y,Z
      y -= Math.floor(y);                                // OF POINT IN CUBE.
      z -= Math.floor(z);
      double u = fade(x),                                // COMPUTE FADE CURVES
             v = fade(y),                                // FOR EACH OF X,Y,Z.
             w = fade(z);
      int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                     grad(p[BA  ], x-1, y  , z   )), // BLENDED
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                     grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                                     grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
   }
   static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
   static double lerp(double t, double a, double b) { return a + t * (b - a); }
   static double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
             v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
*/

