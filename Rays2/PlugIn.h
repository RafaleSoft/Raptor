#ifndef __PLUG_IN_H__
#define __PLUG_IN_H__

#if !defined(DLLEXPORT)
	#define DLLEXPORT _declspec(dllexport)
#endif
#if !defined(ENTRY)
	#define ENTRY _cdecl
#endif
#if !defined(DLLENTRY)
	#define DLLENTRY extern "C"	
#endif

typedef enum
{
	UNKNOWN_TYPE,
	AFTER_EFFECT,
	GEOMETRY,
	TEXTURE,
	SHADER,
	ENGINE,
	NB_PLUGIN_TYPE
} PLUGIN_TYPE;

#ifndef PLUGIN_TYPE_STR
#define PLUGIN_TYPE_STR
	static const char * PLUGIN_STR[] =
	{
		"Unknown type",
		"After effect",
		"Geometry",
		"Texture",
		"Shader",
		"Engine"
	};
#else
	extern const char * PLUGIN_STR[];
#endif

typedef enum
{
	IMAGE_WIDTH,
	IMAGE_HEIGHT,
	IMAGE_COLOR,
	IMAGE_DEPTH,
	CAMERA_FOCALE,
	CAMERA_APERTURE,
	CAMERA_OBJECTPLANE,
	USER_PARAMETER,
	NB_INTERNAL_PARAMETER
} INTERNAL_PARAMETER;

#ifndef INTERNAL_PARAMETER_STR
#define INTERNAL_PARAMETER_STR
	static const char * PARAMETER_STR[] =
	{
		"Image width",
		"Image height",
		"Image color",
		"Image depth",
		"Camera focale",
		"Camera aperture",
		"Camera object plane",
		"User parameter"
	};
#else
	extern const char * PARAMETER_STR[];
#endif

typedef enum
{
	INT32_VALUE,
	UINT32_VALUE,
	FLOAT32_VALUE,
	UINT8_POINTER,
	INT32_POINTER,
	UINT32_POINTER,
	FLOAT32_POINTER,
	NB_PARAMETER_TYPE
} PARAMETER_TYPE;

#ifndef PARAMETER_TYPE_STR
#define PARAMETER_TYPE_STR
	static const char * PARAMETER_STR2[] =
	{
		"32bits integer value",
		"32bits unsigned integer value",
		"32bits float value",
		"8bits unsigned integer pointer",
		"32bits integer pointer",
		"32bits unsigned integer pointer",
		"32bits float pointer"
	};
#else
	extern const char * PARAMETER_STR2[];
#endif


typedef struct PARAMETER_DATA_t
{
	int	nbParameters;
	struct parameter_t
	{
		PARAMETER_TYPE type;
		INTERNAL_PARAMETER kind;
		char* name;
		union
		{
			int	int32Value;
			unsigned int uint32Value;
			float float32Value;
			unsigned char* uint8Pointer;
			int* int32Pointer;
			unsigned int* uint32Pointer;
			float* float32Pointer;
		} value;
	} *parameters;

	PARAMETER_DATA_t()
	{
		nbParameters = 0;
		parameters = NULL;
	}

	~PARAMETER_DATA_t()
	{
		for (int i=0;i<nbParameters;i++)
			delete [] parameters[i].name;
		if (nbParameters > 0)
			delete [] parameters;
	}
} PARAMETER_DATA;


/////////////////////////////////////////////////////////////////////////
//	Plug-in communication protocol
typedef __declspec( dllimport ) char * (* GETMODULEINFO)(void);
typedef __declspec( dllimport ) PLUGIN_TYPE (* GETMODULETYPE)(void);
typedef __declspec( dllimport ) unsigned int (* GETMODULEVERSION)(void);
typedef __declspec( dllimport ) PARAMETER_DATA * (* GETPARAMETERS)(void);
typedef __declspec( dllimport ) int (* SETPARAMETERS)(PARAMETER_DATA* params);
typedef __declspec( dllimport ) int (* INITMODULE)(void);
typedef __declspec( dllimport ) int (* AFTEREFFECT)(void);
typedef __declspec( dllimport ) int (* NORMAL)(float* point,float* normal);

class CPlugin
{
public:
	CPlugin()
	{
		moduleInstance = 0;
		pGetModuleName = 0;
		pGetModuleAuthor = 0;
		pGetModuleDescription = 0;
		pGetModuleType = 0;
		pGetModuleVersion = 0;
		pInitModule = 0;
		pGetParameters = 0;
		pSetParameters = 0;
		pAfterEffect = 0;
		pNormal = 0;
	};
	virtual ~CPlugin() {};

	bool OpenPlugin(const std::string& name);

	HINSTANCE			moduleInstance;
	GETMODULEINFO		pGetModuleName;
	GETMODULEINFO		pGetModuleAuthor;
	GETMODULEINFO		pGetModuleDescription;
	GETMODULETYPE		pGetModuleType;
	GETMODULEVERSION	pGetModuleVersion;
	INITMODULE			pInitModule;
	GETPARAMETERS		pGetParameters;
	SETPARAMETERS		pSetParameters;
	AFTEREFFECT			pAfterEffect;
	NORMAL				pNormal;
};

#endif // !defined(AFX_CAMERA_H__A20284B1_13FC_4604_9859_032740298CC0__INCLUDED_)
