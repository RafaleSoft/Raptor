#include "stdafx.h"
#include "ModuleDOF.h"
#include "..\PlugIn.h"

extern "C" DLLEXPORT int InitModule()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (theApp.workImage != NULL)
	{
		delete [] theApp.workImage;
		theApp.workImage = NULL;
	}

	if ((theApp.image == NULL) || (theApp.zbuffer == NULL))
		return 0;

	theApp.workImage = new unsigned short[theApp.width * theApp.height * 4];

	return 1;
}


extern "C" DLLEXPORT char *GetModuleName()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "ModuleDOF";
}

extern "C" DLLEXPORT char *GetModuleAuthor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Rafale Soft Inc.(c)";
}

extern "C" DLLEXPORT char *GetModuleDescription()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return "Depth of field plug-in for Rays2";
}

extern "C" DLLEXPORT PLUGIN_TYPE GetModuleType(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AFTER_EFFECT;
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

	pData->nbParameters = 7;
	pData->parameters = new PARAMETER_DATA::parameter_t[7];

	pData->parameters[0].kind = IMAGE_WIDTH;
	pData->parameters[0].type = UINT32_VALUE;
	pData->parameters[0].name = new char[6];
	sprintf(pData->parameters[0].name,"%s","width");

	pData->parameters[1].kind = IMAGE_HEIGHT;
	pData->parameters[1].type = UINT32_VALUE;
	pData->parameters[1].name = new char[7];
	sprintf(pData->parameters[1].name,"%s","height");

	pData->parameters[2].kind = IMAGE_COLOR;
	pData->parameters[1].type = UINT8_POINTER;
	pData->parameters[2].name = new char[11];
	sprintf(pData->parameters[2].name,"%s","imageColor");

	pData->parameters[3].kind = IMAGE_DEPTH;
	pData->parameters[3].type = FLOAT32_POINTER;
	pData->parameters[3].name = new char[11];
	sprintf(pData->parameters[3].name,"%s","imageDepth");

	pData->parameters[4].kind = CAMERA_FOCALE;
	pData->parameters[4].type = FLOAT32_VALUE;
	pData->parameters[4].name = new char[13];
	sprintf(pData->parameters[4].name,"%s","cameraFocale");

	pData->parameters[5].kind = CAMERA_APERTURE;
	pData->parameters[5].type = FLOAT32_VALUE;
	pData->parameters[5].name = new char[15];
	sprintf(pData->parameters[5].name,"%s","cameraAperture");

	pData->parameters[6].kind = CAMERA_OBJECTPLANE;
	pData->parameters[6].type = FLOAT32_VALUE;
	pData->parameters[6].name = new char[18];
	sprintf(pData->parameters[6].name,"%s","cameraObjectPlane");

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
					theApp.width = param.value.uint32Value;
				break;
			case IMAGE_HEIGHT:
				if (param.type == UINT32_VALUE)
					theApp.height = param.value.uint32Value;
				break;
			case IMAGE_COLOR:
				if (param.type == UINT8_POINTER)
					theApp.image = param.value.uint8Pointer;
				break;
			case IMAGE_DEPTH:
				if (param.type == FLOAT32_POINTER)
					theApp.zbuffer = param.value.float32Pointer;
				break;
			case CAMERA_FOCALE:
				if (param.type == FLOAT32_VALUE)
					theApp.focale = param.value.float32Value;
				break;
			case CAMERA_APERTURE:
				if (param.type == FLOAT32_VALUE)
					theApp.aperture = param.value.float32Value;
				break;
			case CAMERA_OBJECTPLANE:
				if (param.type == FLOAT32_VALUE)
					theApp.objectplane = param.value.float32Value;
				break;
			default:
				AfxMessageBox("Unknown parameter");
				res = 0;
				break;
		}
	}

	return res;
}

extern "C" DLLEXPORT int AfterEffect(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (theApp.workImage == NULL)
		return 0;

	theApp.AfterEffect();

	return 1;
}

