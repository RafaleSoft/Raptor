// DefaultBumpmapLoader.cpp: implementation of the CDefaultBumpmapLoader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
    #include "DefaultBumpmapLoader.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif

#include <math.h>


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultBumpmapLoader::CDefaultBumpmapLoader()
{

}

CDefaultBumpmapLoader::~CDefaultBumpmapLoader()
{

}

bool CDefaultBumpmapLoader::apply(CTextureObject* const src,
								  unsigned int,
								  unsigned int,
								  unsigned int,
								  const CTextureFactoryConfig& config) const
{
    if (src == NULL)
		return false;

    float scale = config.getBumpAmplitude();
	unsigned int transparency = src->getTransparency();

	int w = src->getWidth();
	int w_4 = w * 4;
	int h = src->getHeight();
    int s = h * w_4;
	int d = MAX(1,src->getDepth());
	int v = s * d;

	unsigned char *texturedata = src->getTexels();

    CMemory::Allocator<unsigned char> allocator;
	unsigned char *bumpMap = allocator.allocate(v);

    for (int i=0; i<v; i+=4)
	    *((unsigned int*)(bumpMap + i)) = 0xFFFF7F7F;

	for (int k=0; k<d; k++)
	{
		for (int i=0; i<w; i++)
		{
			for (int j=0; j<h; j++)
			{
				unsigned char r;
				unsigned char r2;
				int x=0,y=0,z=768;
				int offset = 4 * ((k*h + j)*w + i);

				int left = offset-4;
				int right = offset+4;
				int up = offset-w_4;
				int down = offset+w_4;
				int upleft = offset-w_4-4;
				int downleft = offset+w_4-4;
				int upright = offset-w_4+4;
				int downright = offset+w_4+4;

				if (j==0)
				{   up += s;    upleft += s;    upright += s;   }
				else if (j == (h-1))
				{   down -= s;  downleft -= s;  downright -= s; }
				

				if (i==0)
				{   left += w_4;    upleft += w_4;  downleft += w_4;    }
				else if (i == (w-1))
				{   right -= w_4;   upright -= w_4; downright -= w_4;   }

				r = texturedata[offset];

				r2 = texturedata[left]; 
				if (r2<r) x-=r-r2; else if (r2>r) x+=r2-r;

				r2 = texturedata[right]; 
				if (r2<r) x+=r-r2; else if (r2>r) x-=r2-r;

				r2 = texturedata[up]; 
				if (r2<r) y-=r-r2; else if (r2>r) y+=r2-r;

				r2 = texturedata[down]; 
				if (r2<r) y+=r-r2; else if (r2>r) y-=r2-r;

				r2 = texturedata[upleft]; 
				if (r2<r) {x-=(r-r2)/2;y-=(r-r2)/2;} else if (r2>r) {x+=(r2-r)/2;y+=(r2-r)/2;}

				r2 = texturedata[downleft]; 
				if (r2<r) {x-=(r-r2)/2;y+=(r-r2)/2;} else if (r2>r) {x+=(r2-r)/2;y-=(r2-r)/2;}

				r2 = texturedata[upright]; 
				if (r2<r) {x+=(r-r2)/2;y-=(r-r2)/2;} else if (r2>r) {x-=(r2-r)/2;y+=(r2-r)/2;}

				r2 = texturedata[downright]; 
				if (r2<r) {x+=(r-r2)/2;y+=(r-r2)/2;} else if (r2>r) {x-=(r2-r)/2;y-=(r2-r)/2;}

				x *= scale; y *= scale;
				if (abs(x)>abs(y)) z-=abs(x); else z-=abs(y);

				float n = 127.0f / (float)(sqrt((float)(x*x + y*y + z*z)));
				bumpMap[offset++] = (unsigned char)(x * n + 128);
				bumpMap[offset++] = (unsigned char)(y * n + 128);
				bumpMap[offset++] = (unsigned char)(z * n + 128);
				bumpMap[offset++] = transparency;
			}
		}
	}

    memcpy(texturedata,bumpMap,v);

    CMemory::GetInstance()->garbage(bumpMap);

	return true;
}


