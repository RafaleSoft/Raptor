// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BUILDERNATIVE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BUILDERNATIVE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BUILDERNATIVE_EXPORTS
	#define BUILDERNATIVE_API __declspec(dllexport)
#else
	#define BUILDERNATIVE_API __declspec(dllimport)
#endif

class CGLBuilder;


// This class is exported from the BuilderNative.dll
class BUILDERNATIVE_API CBuilderNative
{
public:
	CBuilderNative(void);
	~CBuilderNative(void);

	//!	Initialise all builder extensions.
	void glvkInitBuilder(HDC dc, HGLRC glhrc);


private:
	//!	Helper method to initialise a default GL context.
	int glCreateContext(HDC& hdc, HGLRC& glhrc);

	//!	The actual code builder.
	CGLBuilder	*builder;
};

