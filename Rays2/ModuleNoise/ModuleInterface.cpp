#include "stdafx.h"
#include "ModuleNOISE.h"
#include "..\PlugIn.h"

extern "C" DLLEXPORT int InitModule()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	srand( (unsigned)time( NULL ) );
	theApp.InitNoiseGeneration();

	return 1;
}


extern "C" DLLEXPORT char *GetModuleName()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "ModuleNoise";
}

extern "C" DLLEXPORT char *GetModuleAuthor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Rafale Soft Inc.(c)";
}

extern "C" DLLEXPORT char *GetModuleDescription()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Noise generator plug-in for Rays2";
}

extern "C" DLLEXPORT PLUGIN_TYPE GetModuleType(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return GEOMETRY;
}

extern "C" DLLEXPORT unsigned int GetModuleVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return 0x01000001;	//	1.0.0.1
}

extern "C" DLLEXPORT PARAMETER_DATA *GetParameters(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	PARAMETER_DATA *pData = new PARAMETER_DATA;

	pData->nbParameters = 3;
	pData->parameters = new PARAMETER_DATA::parameter_t[3];

	pData->parameters[0].kind = USER_PARAMETER;
	pData->parameters[0].type = FLOAT32_VALUE;
	pData->parameters[0].name = new char[12];
	sprintf(pData->parameters[0].name,"%s","Noise Scale");

	pData->parameters[1].kind = USER_PARAMETER;
	pData->parameters[1].type = FLOAT32_VALUE;
	pData->parameters[1].name = new char[15];
	sprintf(pData->parameters[1].name,"%s","Base Frequency");

	pData->parameters[2].kind = USER_PARAMETER;
	pData->parameters[2].type = FLOAT32_VALUE;
	pData->parameters[2].name = new char[10];
	sprintf(pData->parameters[2].name,"%s","Harmonics");

	return pData;
}

extern "C" DLLEXPORT int SetParameters(PARAMETER_DATA* params)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (params == NULL)
		return 0;

	int res = 1;
	for (int i=0;i<params->nbParameters;i++)
	{
		PARAMETER_DATA::parameter_t	&param = params->parameters[i];

		switch(param.kind)
		{
			case IMAGE_WIDTH:
				if (param.type == UINT32_VALUE)
				break;
			case IMAGE_HEIGHT:
				if (param.type == UINT32_VALUE)
				break;
			case IMAGE_COLOR:
				if (param.type == UINT8_POINTER)
				break;
			case IMAGE_DEPTH:
				if (param.type == FLOAT32_POINTER)
				break;
			case CAMERA_FOCALE:
				if (param.type == FLOAT32_VALUE)
				break;
			case CAMERA_APERTURE:
				if (param.type == FLOAT32_VALUE)
				break;
			case CAMERA_OBJECTPLANE:
				if (param.type == FLOAT32_VALUE)
				break;
			default:
				//AfxMessageBox("Unknown parameter");
				res = 0;
				break;
		}
	}

	return res;
}

extern "C" DLLEXPORT int Normal(float* point,float* normal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ((point != NULL) && (normal != NULL))
	{
		CGenericVector<float> pos(point);
		CGenericVector<float> n = theApp.V_Turb(pos,1.0f,3.0f);
		memcpy(normal,n.vector(),4*sizeof(float));
		return 1;
	}
	else
		return 0;
}

