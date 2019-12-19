// DefaultBumpmapLoader.cpp: implementation of the CDefaultBumpmapLoader class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
    #include "DefaultBumpmapLoader.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif

#include <math.h>


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultBumpmapLoader::CDefaultBumpmapLoader(float scale)
	:bump_scale(scale)
{

}

CDefaultBumpmapLoader::CDefaultBumpmapLoader(const CDefaultBumpmapLoader& loader)
	: bump_scale(loader.bump_scale)
{
}

CDefaultBumpmapLoader& CDefaultBumpmapLoader::operator=(const CDefaultBumpmapLoader& loader)
{
	bump_scale = loader.bump_scale;
	return *this;
}

CDefaultBumpmapLoader::~CDefaultBumpmapLoader()
{

}

bool CDefaultBumpmapLoader::apply(CImage* const src) const
								  //,const operation_param_t& param) const
{
    if (src == NULL)
		return false;

	//float scale = param.bump_scale;
	float scale = bump_scale;

	int w = src->getWidth();
	int w_4 = w * 4;
	int h = src->getHeight();
    int s = h * w_4;

	unsigned char *texturedata = src->getPixels();

    CHostMemoryManager::Allocator<unsigned char> allocator;
	unsigned char *bumpMap = allocator.allocate(s);

    for (int i=0; i<s; i+=4)
	    *((unsigned int*)(bumpMap + i)) = 0xFFFF7F7F;

	for (int i=0; i<w; i++)
	{
		for (int j=0; j<h; j++)
		{
			unsigned char r;
			unsigned char r2;
			int x=0,y=0,z=768;
			int offset = 4 * (j*w + i);

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
			unsigned char transparency = texturedata[offset + 3];

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

    memcpy(texturedata,bumpMap,s);

    CHostMemoryManager::GetInstance()->garbage(bumpMap);

	return true;
}


