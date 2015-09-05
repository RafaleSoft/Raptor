// ModuleDOF.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ModuleDOF.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CModuleDOFApp, CWinApp)
	//{{AFX_MSG_MAP(CModuleDOFApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#include <math.h>			//	ceil()
#ifndef M_PI
	#define M_PI            3.1415926535
#endif

#define COLOR_FACTOR		32

/////////////////////////////////////////////////////////////////////////////
// CModuleDOFApp construction

CModuleDOFApp::CModuleDOFApp()
{
	width = 0;
	height = 0;
	image = NULL;
	zbuffer = NULL;
	workImage = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CModuleDOFApp object
CModuleDOFApp theApp;


void CModuleDOFApp::proceedPixel(float diameter,unsigned int index)
{
	int x = index % width;
	int y = index / width;

	float c = 0.5f * diameter;
	int max = ceil(c);
	int min = floor(c);
	float k = 4.0f / ( diameter * diameter * M_PI);

	unsigned int offset = 4 * index;
	unsigned short r = image[offset];
	unsigned short g = image[offset+1];
	unsigned short b = image[offset+2];
	unsigned short a = 255;

	int imin = -max;
	int imax = max;
	int jmin = -max;
	int jmax = max;

	if ((x - max) < 0)
		imin = -x;
	else if ((x + max) >= width)
		imax = width - x - 1;
	if ((y - max) < 0)
		jmin = -y;
	else if ((y + max) >= height)
		jmax = height - y - 1;

	for (int i=imin;i<=imax;i++)
	{
		for (int j=jmin;j<=jmax;j++)
		{
			float d = sqrt(i * i + j * j);

			offset = 4*(index + j*width + i);

			if (d < min)
			{
				workImage[offset] += COLOR_FACTOR * k * r;
				workImage[offset+1] += COLOR_FACTOR * k * g;
				workImage[offset+2] += COLOR_FACTOR * k * b;
				workImage[offset+3] += COLOR_FACTOR * k * a;
			}
			else 
			{
				workImage[offset] += (c - min) * COLOR_FACTOR * k * r;
				workImage[offset+1] += (c - min) * COLOR_FACTOR * k * g;
				workImage[offset+2] += (c - min) * COLOR_FACTOR * k * b;
				workImage[offset+3] += (c - min) * COLOR_FACTOR * k * a;
			}
		}
	}
}

void CModuleDOFApp::AfterEffect(void)
{
	float *circles = new float[width*height];

	float a = theApp.aperture;
	float k = theApp.width * focale / ( focale - objectplane );

	for (unsigned int i=0;i<width*height;i++)
	{
		float z = zbuffer[i];
		float val = k;

		if ((z > 0.01f) || (z < -0.01f))
			val = k * (z - objectplane) / z;	

		if (val < 0)
			val = -val;
		
		circles[i] = val;
	}

	unsigned int w = width * 4;

	for (i=0;i<width*height;i++)
	{
		float c = circles[i];
		int j = 4*i;

		if (c > 1.0f)
		{
			proceedPixel(c,i);
		}
		else
		{
			workImage[j] += COLOR_FACTOR * image[j];
			workImage[j+1] += COLOR_FACTOR * image[j+1];
			workImage[j+2] += COLOR_FACTOR * image[j+2];
			workImage[j+3] += 255 * COLOR_FACTOR;
		}
	}

	for (i=0;i<w*height;i+=4)
	{
		float fact = 255.0f / workImage[i+3];
		if (fact > 1.0f)
			fact = 1.0f;

		image[i] = fact * workImage[i];
		image[i+1] = fact * workImage[i+1];
		image[i+2] = fact * workImage[i+2];
		image[i+3] = 255;
	}

/*
	CFile zbuffer("cbuffer.dat",CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive);
	CString str;
	for (unsigned int j=0;j<height;j++)
	{
		for (unsigned int i=0;i<width;i++)
		{
			str.Format("%f ",circles[i+j*width]);
			zbuffer.Write(LPCTSTR(str),str.GetLength());
		}
		str.Format("\n");
		zbuffer.Write(LPCTSTR(str),str.GetLength());
	}
	zbuffer.Close();
*/
}
