#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#if !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysUtils.h"
#endif

/////////////////////////////////////////////////////////////////////////
//	Constants
#define MAX_STR_LEN	64

#define JOBBASE 0x00000000
#define OBJBASE 0x00000100
#define IMGBASE 0x00001000
#define	ACKBASE 0x00010000
#define DMNBASE 0x00100000
#define SESBASE 0x01000000
#define MSGBASE	0x10000000

/////////////////////////////////////////////////////////////////////////
// client job io

#define JOB_ID			(JOBBASE	+	0x1)

//	JOB_START
//		in						out
//	data 0 = sessionID		  jobID
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
//	data 0 = sessionID			*
//	data 1 =	*				*
//	data 2 =	*				*
//	data 3 =	*				*
//	data 4 = job %				*
#define JOB_PERCENT		(JOBBASE	+	0x8)

#define JOB_DELETE		(JOBBASE	+	0x9)

// work unit job io
//	JOB_WUNIT
//		in						out
//	data 0 = sessionID			*
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
//	data 0 = sessionID			*
//	data 1 = startScanLine		*
//	data 2 = endScanLine		*
//	data 3 =	*				*
//	data 4 =	*				*
#define JOB_STOP		(JOBBASE	+	0xe)

//	JOB_DATA
//		in					out (NONE)
//	data 0 =    *				*
//	data 1 =    *				*
//	data 2 =    *				*
//	data 3 =	*				*
//	data 4 =	*				*
#define JOB_DATA		(JOBBASE	+	0xf)

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

//
//	Deamon communication
//

//	DMN_STATUS
//		in						out
//	data 0 =	*			nbProcs
//	data 1 =	*			nbProcsAvailable
//	data 2 =	*			jobDone
//	data 3 =	*			Nb running jobs
//	data 4 =	*			IP addr
#define DMN_STATUS		(DMNBASE	+	0x1)
//#define DMN_INACTIVE	(DMNBASE	+	0x2)

//	DMN_ACTIVE
//		in						out
//	data 0 =  WUID				WUID
//	data 1 =  nbProcs		hProcess
//	data 2 =  srvIP			hThread
//	data 3 =  srvPort		ProcessId
//	data 4 =  priority		ThreadId
#define DMN_DISPATCHJOB	(DMNBASE	+	0x3)

//
//	Sessions management
//

//	SES_OPEN
//		in				   out (SES_ID)
//	data 0 =    *			sessionID - High dword
//	data 1 =    *    		sessionID - Low dword
//	data 2 =	*				*
//	data 3 =    *				*
//	data 4 =    *   			*
#define SES_OPEN		(SESBASE	+	0x1)

//	SES_CLOSE
//		in					out (NONE)
//	data 0 =    *				*
//	data 1 =    *    			*
//	data 2 =	*				*
//	data 3 =    *				*
//	data 4 =    *   			*
#define SES_CLOSE		(SESBASE	+	0x2)

//	SES_ID
//		in					out (NONE)
//	data 0 = sessionID - High   *
//	data 1 = sessionID - Low    *
//	data 1 =    *    			*
//	data 2 =	*				*
//	data 3 =    *				*
//	data 4 =    *   			*
#define SES_ID			(SESBASE	+	0x3)

/////////////////////////////////////////////////////////////////////////
//	Public structures for Client/Server/WorkUnit communication
typedef uint32_t RAYS_MSG_ID;

typedef struct MSGSTRUCT
{
	//! These fields are already part of RaptorNetwork protocol.
	//RAYS_MSG_ID		msg_crc;		// 32 bits CRC of the message payload
	//RAYS_MSG_ID		msg_size;		// size of following chunck of data
	//!	Rays base messages id.
	RAYS_MSG_ID		msg_id;			// semantic
	RAYS_MSG_ID		msg_data[5];	// sub semantic ID
} msg_struct_t;

typedef MSGSTRUCT* LPMSGSTRUCT;

const unsigned int MSGSIZE = sizeof(MSGSTRUCT);

typedef struct _rays_config_tag
{
	uint32_t	width;
	uint32_t	height;
	uint32_t	variance;
	uint32_t	deflection;
	uint32_t	defraction;
	uint32_t	crease;
	float		focale;
	float		object_plane;
	uint32_t	photon_map;
	char		envtexname[MAX_STR_LEN];
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
	unsigned long	transform;
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
	unsigned long	model;		// bitwise of LIGHT_MODEL
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
	unsigned int		nbCalls;
	rays_plugin_t		*calls;
	unsigned int		nbTransforms;
	rays_transform_t	*transforms;
} rays_frame_t;

#endif	// _MESSAGES_H_
