#include "StdAfx.h"
#include "DTEDReader.h"

#include "System/Raptor.h"

RAPTOR_NAMESPACE


CDTEDReader::CDTEDReader(void)
{
}

CDTEDReader::~CDTEDReader(void)
{
}

unsigned short* CDTEDReader::dted_read(unsigned int &size_x,unsigned int &size_y,const char *infile)
{
	unsigned char w = 0;
	unsigned char h = 0;
	unsigned short *img = NULL;
	FILE *in = NULL;
	size_t readSize = 0;

	if (NULL == infile)
		return NULL;

	in = fopen(infile,"rb");
	if (NULL == in)
		return NULL;

	if (1 != fread(&w,1,1,in))
	{
		fclose(in);
		return NULL;
	}
	if (1 != fread(&h,1,1,in))
	{
		fclose(in);
		return NULL;
	}

	img = (unsigned short*)malloc(w*h*2);

	readSize = fread(img,2,w*h,in);
	if (w*h != readSize)
	{
		fclose(in);
		free(img);
		return NULL;
	}

	fclose(in);

	size_x = w;
	size_y = h;

	return img;
}


unsigned short* CDTEDReader::dted_resize(unsigned short *image,unsigned int &size_x,unsigned int &size_y,unsigned int n)
{
	unsigned short* img = (unsigned short*)malloc(sizeof(unsigned short)*n*n);

	float stepx = ((float)size_x) / ((float)n);
	float stepy = ((float)size_y) / ((float)n);

	for (unsigned int j=0;j<n;j++)
	{
		for (unsigned int i=0;i<n;i++)
		{
			unsigned int col = i*stepx;
			unsigned int row = j*stepy;
			unsigned int src_pos = col + row*size_x;
			unsigned short alt = image[src_pos];
			unsigned short alt2 = (col < size_x-1 ? image[src_pos+1] : alt);
			unsigned short alt3 = (row < size_y-1 ? image[src_pos+size_x] : alt);
			unsigned short alt4 = ((col < size_x-1) && (row < size_y-1) ? image[src_pos+size_x+1] : alt);
			img[i+j*n] = (alt + alt2 + alt3 + alt4) >> 2;
		}
	}

	size_x = n;
	size_y = n;
	return img;
}

