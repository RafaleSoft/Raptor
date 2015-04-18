
#ifndef __FTEngine_h
#define __FTEngine_h

#include <ft2build.h>
#include <freetype.h>

/////////////////////////////////////////////////////////////////////////////

class FTEngine
{
public:
	const FT_Library& getEngine() const { return library; }
	static FTEngine& getStaticEngine();

private:
	FTEngine();
	virtual ~FTEngine();

	bool init();

	static FTEngine* static_engine;
	FT_Library library;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTEngine_h
