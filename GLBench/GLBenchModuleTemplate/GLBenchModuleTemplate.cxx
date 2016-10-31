#ifndef GL_BENCH_MODULE_TEMPLATE_CXX
#define GL_BENCH_MODULE_TEMPLATE_CXX

#include <strstream>
#include "../Interface.h"

#include "Engine/TimeObject.h"
#include "MFCExtension/CWnd/GLWnd.h"

result_bench_t results;

static const char* AUTHOR = "Rafale Soft Inc.(c)";

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
	GLBenchDisplay() : totalTime(0.0f), dt(0.0f)
	{
	};
	virtual ~GLBenchDisplay()
	{
	};

	virtual	void GLInitContext(void) = 0;
	virtual void GLDisplayFunc(void) = 0;

	float totalTime;
	float dt;

private:
};


void BenchStep(unsigned int step, unsigned int size,
	float calibration, GLBenchDisplay *display,
	unsigned int width, unsigned int height)
{
	unsigned int	nb = 0;
	CTimeObject::markTime(0);

	for (unsigned i = 0; i<size; i++)
	{
		display->SendMessage(WM_PAINT);
		nb++;
	}

	float dt = CTimeObject::deltaMarkTime(0);

	results.result_items[step].fps_rate = nb / dt;
	results.result_items[step].driver_overhead = 100.0f * (1.0f - display->totalTime / (dt - calibration));
	double megatexelspersec = nb / double(dt - calibration) * width * height / 1000000.0;
	results.result_items[step].score = megatexelspersec;
	results.result_items[step].fragment_rate = megatexelspersec;
}


#endif
