#ifndef GL_BENCH_MODULE_TEMPLATE_CXX
#define GL_BENCH_MODULE_TEMPLATE_CXX

#include <strstream>
#include "../Interface.h"

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
		results.rate += results.result_items[i].rate;
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
		results.result_items[i].rate = 0;
		results.result_items[i].score = 0;
	}
}



#endif