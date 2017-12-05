
#include "FTEngine.h"


/////////////////////////////////////////////////////////////////////////////

// static
FTEngine* FTEngine::static_engine;

/////////////////////////////////////////////////////////////////////////////

FTEngine::FTEngine()
	:library(0)
{
}

/////////////////////////////////////////////////////////////////////////////

FTEngine::~FTEngine()
{
	FT_Error error = FT_Done_FreeType( library );
	if ( error )
	{
	  //... an error occurred during library initialization ...
	}
}

/////////////////////////////////////////////////////////////////////////////

bool FTEngine::init()
{
	if (library != 0)
		return true;

	FT_Error error = FT_Init_FreeType( &library );
	if ( FT_Err_Ok != error )
	{
	  //... an error occurred during library initialization ...
	}

	return (FT_Err_Ok == error);
}

/////////////////////////////////////////////////////////////////////////////

// static
FTEngine& FTEngine::getStaticEngine()
{
	if( static_engine == 0 )
	{
		static_engine= new FTEngine;
		if( ! static_engine->init() )
		{
			//... an error occurred during library initialization ...
		}
	}

	return *static_engine;
}

/////////////////////////////////////////////////////////////////////////////
