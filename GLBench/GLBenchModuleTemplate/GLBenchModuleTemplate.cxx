/***************************************************************************/
/*                                                                         */
/*  GLBenchModuleTemplate.cxx                                              */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/



#ifndef GL_BENCH_MODULE_TEMPLATE_CXX
#define GL_BENCH_MODULE_TEMPLATE_CXX

#include <strstream>
#include "../Interface.h"

#include "Engine/TimeObject.h"
#include "MFCExtension/CWnd/GLWnd.h"

result_bench_t results;

static const char* AUTHOR = "Rafale Soft Inc.(c)";
static const unsigned int CALIBRATE_SIZE = 5000;

extern "C" GLBENCH_API const char* GetModuleName()
{
	return NAME;
}

extern "C" GLBENCH_API const char* GetModuleAuthor()
{
	return AUTHOR;
}

extern "C" GLBENCH_API const char* GetModuleDescription()
{
	return DESCRIPTION;
}

extern "C" GLBENCH_API lp_result_bench_t GetBenchResult()
{
	results.score = 0;
	results.rate = 0.0f;

	for (unsigned int i=0;i<results.nbItems;i++)
	{
		results.score += results.result_items[i].score;
		results.rate += results.result_items[i].fps_rate;
	}

	results.score /= results.nbItems;
	results.rate /= results.nbItems;

	return &results;
}


extern "C" GLBENCH_API void InitModule()
{
	results.name = NAME;
	results.author = AUTHOR;
	results.description = DESCRIPTION;
	results.nbItems = NB_RESULTS;
	results.result_items = new result_item_t[results.nbItems];

	for (unsigned int i=0;i<results.nbItems;i++)
	{
		results.result_items[i].description = RESULT_DESCRIPTION[i];
		results.result_items[i].fps_rate = 0;
		results.result_items[i].fragment_rate = 0;
		results.result_items[i].score = 0;
		results.result_items[i].driver_overhead = 0;
	}
}

class GLBenchDisplay : public CGLWnd
{
public:
	GLBenchDisplay()
	:totalTime(0.0f), dt(0.0f), width(0), height(0),
		doCalibrate(false), calibration(0.0f)
	{
	};
	virtual ~GLBenchDisplay()
	{
	};

	unsigned int getWidth(void) const { return width; };
	unsigned int getHeight(void) const { return height; };

	void glCalibrate(unsigned int width, unsigned int height);
	float getCalibration(void) const { return calibration; };

	void resetTotalTime(void) { totalTime = 0.0f; };
	float getTotalTime(void) const { return totalTime; };
		

protected:
	float totalTime;
	float dt;
	unsigned int width;
	unsigned int height;

private:
	virtual void glDraw(void) = 0;

	virtual	void GLInitContext(void) = 0;
	virtual void GLDisplayFunc(void);

	bool doCalibrate;
	float calibration;
};


void GLBenchDisplay::GLDisplayFunc(void)
{
	if (!doCalibrate)
		glDraw();
}

void GLBenchDisplay::glCalibrate(unsigned int w, unsigned int h)
{
	totalTime = 0.0f;
	width = w;
	height = h;
	doCalibrate = true;

	//
	//	Calibration pre-heat
	//
	int i = 0;
	unsigned int nb = 0;
	for (i = 0; i<CALIBRATE_SIZE; i++)
	{
		SendMessage(WM_PAINT);
		nb++;
	}

	//
	//	Calibrate with Swap buffers
	//
	totalTime = 0.0f;
	CTimeObject::markTime(this);

	for (i = 0; i<CALIBRATE_SIZE; i++)
	{
		SendMessage(WM_PAINT);
		nb++;
	}

	calibration = CTimeObject::deltaMarkTime(this);
	
	double megatexelspersec = (nb / calibration) * width * height / 1000000.0;
	results.result_items[0].fps_rate = nb / calibration;
	results.result_items[0].score = megatexelspersec;
	results.result_items[0].fragment_rate = megatexelspersec;

	doCalibrate = false;
}


void BenchStep(	unsigned int step, unsigned int loop_size, GLBenchDisplay *display)
{
	unsigned int	nb = 0;
	display->resetTotalTime();
	CTimeObject::markTime(0);

	for (unsigned i = 0; i<loop_size; i++)
	{
		display->SendMessage(WM_PAINT);
		nb++;
	}

	float dt = CTimeObject::deltaMarkTime(0);
	float calibration = (display->getCalibration() * loop_size) / CALIBRATE_SIZE;

	results.result_items[step].fps_rate = nb / dt;
	results.result_items[step].driver_overhead = 100.0f * (1.0f - display->getTotalTime() / (dt - calibration));
	double megatexelspersec = nb / double(dt - calibration) * display->getWidth() * display->getHeight() / 1000000.0;
	results.result_items[step].score = megatexelspersec;
	results.result_items[step].fragment_rate = megatexelspersec;
}


#endif
