#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include "time.h"		// time_t ( JOBSTRUCT )

/////////////////////////////////////////////////////////////////////////
//	Constants
#define PORTBASE 2048

#define JOBBASE 0x00000000
#define OBJBASE 0x00000100
#define IMGBASE 0x00001000
#define	ACKBASE 0x00010000
#define MSGBASE	0x10000000

// client job io
#define JOB_ID			(JOBBASE	+	0x1)
#define JOB_START		(JOBBASE	+	0x2)
#define JOB_REQUEST		(JOBBASE	+	0x3)
#define JOB_PERCENT		(JOBBASE	+	0x4)
#define JOB_BREAK		(JOBBASE	+	0x5)
#define JOB_RESUME		(JOBBASE	+	0x6)
#define JOB_BATCH		(JOBBASE	+	0x7)

// work unit job io
#define JOB_WUNIT		(JOBBASE	+	0x11)
#define JOB_BACKUP		(JOBBASE	+	0x12)
#define JOB_UNBACKUP	(JOBBASE	+	0x13)
#define JOB_RUN			(JOBBASE	+	0x14)
#define JOB_STOP		(JOBBASE	+	0x15)

// image io
#define	IMG_REQUEST		(IMGBASE	+	0x1)
#define	IMG_DATA		(IMGBASE	+	0x2)
#define IMG_PRINT		(IMGBASE	+	0x3)
#define IMG_SIZE_X		(IMGBASE	+	0x4)
#define IMG_SIZE_Y		(IMGBASE	+	0x5)

// objects io
#define	OBJ_DATA		(OBJBASE	+	0x1)
#define OBJ_SIZE		(OBJBASE	+	0x2)
#define OBJ_TYPE		(OBJBASE	+	0x3)
#define OBJ_NOBJECTS	(OBJBASE	+	0x4)

// acknowledgement
#define ACK_NONE		(ACKBASE	+	0x1)
#define ACK_JOB			(ACKBASE	+	0x2)
#define ACK_OBJ			(ACKBASE	+	0x3)
#define ACK_DAT			(ACKBASE	+	0x4)
#define ACK_IMG			(ACKBASE	+	0x5)

// messages io
#define MSG_START		(MSGBASE	+	0x1)
#define MSG_END			(MSGBASE	+	0x2)


/////////////////////////////////////////////////////////////////////////
//	Structures

typedef struct MSGSTRUCT
{
	int	msg_header;		
	int	msg_id;			// semantic
	int	msg_size;		// size of following chunck of data
	int	msg_data[5];	// sub semantic ID
	int	msg_tail;
} msg_struct_t;

typedef MSGSTRUCT* LPMSGSTRUCT;

typedef struct WORKUNITSTRUCT
{
	int					workUnitID;
	PROCESS_INFORMATION	pi;
	STARTUPINFO			si;
} work_unit_struct_t;

typedef WORKUNITSTRUCT* LPWORKUNITSTRUCT;

typedef struct JOBSTRUCT
{
	int					jobID;
	int					clientID;
	int					nbWorkUnits;
	LPWORKUNITSTRUCT	*workUnits;
	CDC*				globalImageBuffer;
	time_t				jobStart;
	int					jobWidth;
	int					jobHeight;
} job_struct_t;

typedef JOBSTRUCT* LPJOBSTRUCT;

#endif	// _MESSAGES_H_
