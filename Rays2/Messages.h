#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include "time.h"		// time_t ( JOBSTRUCT )

/////////////////////////////////////////////////////////////////////////
//	Constants
#define PORTBASE	2048
#define PACKET_SIZE	2000
#define MAX_STR_LEN	64
#define MAX_PARAMS	16

#define JOBBASE 0x00000000
#define OBJBASE 0x00000100
#define IMGBASE 0x00001000
#define	ACKBASE 0x00010000
#define DMNBASE 0x00100000
#define MSGBASE	0x10000000

/////////////////////////////////////////////////////////////////////////
// client job io

#define JOB_ID			(JOBBASE	+	0x1)

//	JOB_START
//		in						out
//	data 0 = clientID		jobID
//	data 1 = width				*
//	data 2 = height				*
//	data 3 = port				*
//	data 4 = IP addr			*
#define JOB_START		(JOBBASE	+	0x2)

//	JOB_REQUEST	( job result is sent through msg.size and data )
//		in						out
//	data 0 = jobID				*
//	data 1 = request ID			*
//	data 2 =	*				*
//	data 3 = port				*
//	data 4 = IP addr			*
#define JOB_REQUEST		(JOBBASE	+	0x3)

//	JOB_STATUS
//		in						out
//	data 0 = jobID				*
//	data 1 =	*			start time
//	data 2 =	*			estimated finish time
//	data 3 = port			nb available job results ( IDs returned in data )
//	data 4 = IP addr		job %
#define JOB_STATUS		(JOBBASE	+	0x4)
#define JOB_BREAK		(JOBBASE	+	0x5)
#define JOB_RESUME		(JOBBASE	+	0x6)
#define JOB_BATCH		(JOBBASE	+	0x7)

//	JOB_PERCENT
//		in						out
//	data 0 = clientID			*
//	data 1 =	*				*
//	data 2 =	*				*
//	data 3 =	*				*
//	data 4 = job %				*
#define JOB_PERCENT		(JOBBASE	+	0x8)

#define JOB_DELETE		(JOBBASE	+	0x9)

// work unit job io
//	JOB_WUNIT
//		in						out
//	data 0 = clientID			*
//	data 1 =	*				*
//	data 2 =	*				*
//	data 3 = port				*
//	data 4 = IP addr			*
#define JOB_WUNIT		(JOBBASE	+	0xa)

#define JOB_BACKUP		(JOBBASE	+	0xb)
#define JOB_UNBACKUP	(JOBBASE	+	0xc)

//	JOB_RUN
//		in						out
//	data 0 = jobID				*
//	data 1 = startScanLine		*
//	data 2 = endScanLine		*
//	data 3 =	*				*
//	data 4 =	*				*
#define JOB_RUN			(JOBBASE	+	0xd)

//	JOB_STOP
//		in						out
//	data 0 = clientID			*
//	data 1 = startScanLine		*
//	data 2 = endScanLine		*
//	data 3 =	*				*
//	data 4 =	*				*
#define JOB_STOP		(JOBBASE	+	0xe)

// image io
//	IMG_REQUEST
//		in					out(OBJ_TEXTURE)	out(OBJ_PLUGIN)
//	data 0 = request type		width				OBJ_PLUGIN
//	data 1 =	*				height				*
//	data 2 =	*				*					*
//	data 3 = port				port				*
//	data 4 = IP addr			IP addr				*
#define	IMG_REQUEST		(IMGBASE	+	0x1)

//	IMG_DATA
//		in						out
//	data 0 = jobID			image_width
//	data 1 =	*			image_height
//	data 2 =	*				*
//	data 3 =	*				*
//	data 4 =	*			IMG_DATA
#define	IMG_DATA		(IMGBASE	+	0x2)
#define IMG_INFO		(IMGBASE	+	0x3)


// objects io

//	OBJ_DATA
//		in						out
//	data 0 = jobID				*
//	data 1 = objType			*
//	data 2 =	*				*
//	data 3 =	*				*
//	data 4 =	*				*
#define	OBJ_DATA		(OBJBASE	+	0x1)

#define OBJ_SIZE		(OBJBASE	+	0x2)
#define OBJ_TYPE		(OBJBASE	+	0x3)
#define OBJ_NOBJECTS	(OBJBASE	+	0x4)
#define OBJ_CAMERA		(OBJBASE	+	0x5)
#define OBJ_LIGHT		(OBJBASE	+	0x6)
#define OBJ_SPHERE		(OBJBASE	+	0x7)
#define OBJ_GEOMETRY	(OBJBASE	+	0x8)

//	OBJ_TEXTURE
//		in						out
//	data 0 = width				*
//	data 1 = height 			*
//	data 2 = name length		*
//	data 3 = port				*
//	data 4 = IP addr		OBJ_TEXTURE
#define OBJ_TEXTURE		(OBJBASE	+	0x9)

//	OBJ_PLUGIN
//		in						out
//	data 0 = size				*
//	data 1 = *		 			*
//	data 2 = name length		*
//	data 3 = port				*
//	data 4 = IP addr		OBJ_PLUGIN
#define OBJ_PLUGIN		(OBJBASE	+	0xa)

//	OBJ_FRAME
//		in						out
//	data 0 = jobID				*
//	data 1 = OBJ_FRAME 			*
//	data 2 = nb frames  		*
//	data 3 =	*				*
//	data 4 =	*			OBJ_FRAME
#define OBJ_FRAME		(OBJBASE	+	0xb)


// acknowledgement
#define ACK_NONE		(ACKBASE	+	0x1)
#define ACK_JOB			(ACKBASE	+	0x2)
#define ACK_OBJ			(ACKBASE	+	0x3)
#define ACK_DAT			(ACKBASE	+	0x4)
#define ACK_IMG			(ACKBASE	+	0x5)

//	deamon communication

//	DMN_ACTIVE
//		in						out
//	data 0 =	*				*
//	data 1 =	*				*
//	data 2 =	*				*
//	data 3 =	*				*
//	data 4 =	*			IP addr
#define DMN_ACTIVE		(DMNBASE	+	0x1)
#define DMN_INACTIVE	(DMNBASE	+	0x2)

//	DMN_ACTIVE
//		in						out
//	data 0 =  WUID				WUID
//	data 1 =  nbProcs		hProcess
//	data 2 =  srvIP			hThread
//	data 3 =  srvPort		ProcessId
//	data 4 =  priority		ThreadId
#define DMN_DISPATCHJOB	(DMNBASE	+	0x3)

// messages io
#define MSG_START		(MSGBASE	+	0x1)
#define MSG_END			(MSGBASE	+	0x2)
#define MSG_DATA		(MSGBASE	+	0x3)


#ifndef MSG_STR
#define MSG_STR
	static const char * JOB_STR[] =
	{
		"JOBBASE",
		"JOB_ID",
		"JOB_START",
		"JOB_REQUEST",
		"JOB_STATUS",
		"JOB_BREAK",
		"JOB_RESUME",
		"JOB_BATCH",
		"JOB_PERCENT",
		"JOB_DELETE",
		"JOB_WUNIT",
		"JOB_BACKUP",
		"JOB_UNBACKUP",
		"JOB_RUN",
		"JOB_STOP"
	};
	static const char * OBJ_STR[] =
	{
		"OBJBASE",
		"OBJ_DATA",
		"OBJ_SIZE",
		"OBJ_TYPE",
		"OBJ_NOBJECTS",
		"OBJ_CAMERA",
		"OBJ_LIGHT",
		"OBJ_SPHERE",
		"OBJ_GEOMETRY",
		"OBJ_TEXTURE",
		"OBJ_PLUGIN",
		"OBJ_FRAME"
	};
	static const char * IMG_STR[] =
	{
		"IMGBASE",
		"IMG_REQUEST",
		"IMG_DATA",
		"IMG_INFO"
	};
	static const char * ACK_STR[] =
	{
		"ACKBASE",
		"ACK_NONE",
		"ACK_JOB",
		"ACK_OBJ",
		"ACK_DAT",
		"ACK_IMG"
	};
	static const char * DMN_STR[] =
	{
		"DMNBASE",
		"DMN_ACTIVE",
		"DMN_INACTIVE",
		"DMN_DISPATCHJOB"
	};
#else
	extern const char * JOB_STR[];
	extern const char * OBJ_STR[];
	extern const char * IMG_STR[];
	extern const char * ACK_STR[];
	extern const char * DMN_STR[];
#endif

#define ADD_MSG_STRING(msg,msgid) \
	{\
	if(msgid < OBJBASE)\
		msg+=CString(JOB_STR[msgid-JOBBASE]);\
	else if(msgid < IMGBASE)\
		msg+=CString(OBJ_STR[msgid-OBJBASE]);\
	else if(msgid < ACKBASE)\
		msg+=CString(IMG_STR[msgid-IMGBASE]);\
	else if(msgid < DMNBASE)\
		msg+=CString(ACK_STR[msgid-ACKBASE]);\
	else if(msgid < MSGBASE)\
		msg+=CString(DMN_STR[msgid-DMNBASE]);\
	}


/////////////////////////////////////////////////////////////////////////
//	Public structures for Client/Server/WorkUnit communication

typedef unsigned int RAYS_MSG_ID;

typedef struct MSGSTRUCT
{
	RAYS_MSG_ID		msg_header;		
	RAYS_MSG_ID		msg_id;			// semantic
	RAYS_MSG_ID		msg_size;		// size of following chunck of data
	RAYS_MSG_ID		msg_data[5];	// sub semantic ID
	RAYS_MSG_ID		msg_tail;
} msg_struct_t;

typedef MSGSTRUCT* LPMSGSTRUCT;

const unsigned int MSGSIZE = sizeof(MSGSTRUCT);

typedef struct _rays_config_tag
{
	UINT	width;
	UINT	height;
	UINT	variance;
	UINT	deflection;
	UINT	defraction; 
	UINT	crease;
	float	focale;
	float	object_plane;
	UINT	photon_map;
	char envtexname[MAX_STR_LEN];
} rays_config_t;

typedef struct _rays_plugin_tag
{
	char	name[MAX_STR_LEN];
	int		nbParams;
	float	params[16];
} rays_plugin_t;

typedef struct _rays_coord_tag
{
	float x;
	float y;
	float z;
	float h;
} rays_coord_t;

enum TRANSFORM_TYPE
{
	NO_TRANSFORM,
	SCALE,
	TRANSLATE,
	ROTATE
};

typedef struct _rays_transform_tag
{
	char			target[MAX_STR_LEN];
	DWORD			transform;
	rays_coord_t	coeffs;
} rays_transform_t;

typedef struct _rays_vertex_tag
{
	rays_coord_t	position;
	rays_coord_t	texel;
} rays_vertex_t;

//	each model is defined in a single bit
enum LIGHT_MODEL
{
	OMNI = 0,
	SPOT = 1,
	SPOT_COS = 2,
	SPOT_EXP = 4,
	PHOTON_MAP = 8,
};

typedef struct _rays_light_tag
{
	rays_coord_t	position;
	rays_coord_t	color;
	float			intensity;
	rays_coord_t	direction;
	float			cutoff;
	DWORD			model;		// bitwise of LIGHT_MODEL
} rays_light_t;


enum SHADING_OPTIONS
{
	NONE,
	SEPARATE_SPECULAR,
	NB_OPTIONS
};

enum TEXTURE_TYPE
{
	MAP,
	PROCEDURE,
	NB_TYPE
};

typedef struct _rays_shading_tag
{
	float			ambient;
	float			diffuse;
	float			specular;
	int				exponent;
	float			reflection;
	float			refraction;
	float			indice;
	SHADING_OPTIONS	options;
} rays_shading_t;

typedef struct _rays_texture_tag
{
	TEXTURE_TYPE	type;
	rays_plugin_t	texture;
} rays_texture_t;

typedef struct _rays_triangle_tag
{
	int		p1;
	int		p2;
	int		p3;
} rays_triangle_t;

typedef struct _rays_objectbase_tag
{
	char			name[MAX_STR_LEN];
	rays_shading_t	shading;
	rays_texture_t	texture;
	rays_texture_t	normal;
	rays_coord_t	color;
} rays_objectbase_t;

typedef struct _rays_sphere_tag
{
	rays_objectbase_t	base;
	rays_coord_t		center;
	float				radius;
} rays_sphere_t;

typedef struct _rays_composition_tag		//	the geometry and the mesh should
{									//	follow this structure header
	rays_objectbase_t	base;
	int					geometrySize;
	rays_vertex_t		*geometry;
	int					meshSize;
	rays_triangle_t		*mesh;
} rays_composition_header_t;


typedef struct _rays_frame_tag
{
	int					nbCalls;
	rays_plugin_t		*calls;
	int					nbTransforms;
	rays_transform_t	*transforms;
} rays_frame_t;

#endif	// _MESSAGES_H_
