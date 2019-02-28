// RaptorDisplayConfig.cpp: implementation of the CRaptorDisplayConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif

#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "RaptorDisplayConfig.h"
#endif
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
#else
	#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
		#include "System/Raptor.h"
	#endif
	#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
		#include "System/RaptorGLExtensions.h"
	#endif
	#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
		#include "GLHierarchy/ProgramParameters.h"
	#endif
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorDisplayConfig::CRaptorDisplayConfig()
{
	x = 0;
	y = 0;
	width = 640;
	height = 480;
	caption = "Raptor application";
	refresh_rate.sync_to_monitor = false;
	refresh_rate.fps = CGL_MAXREFRESHRATE;
	display_mode = CGL_RGBA | CGL_DEPTH;
	draw_logo = false;
	status_bar = false;
	acceleration = GENERIC;
	antialias = ANTIALIAS_NONE;
	swap_buffer = SWAP_UNDEFINED;
	renderer = NATIVE_GL;
	bind_to_texture = false;
	overlay = false;
	double_buffer = true;
	depth_buffer = true;
	stencil_buffer = false;
	accumulator_buffer = false;

    hintState.perspectiveCorrection = GL_NICEST;
    hintState.fog = GL_DONT_CARE;
    hintState.lineSmooth = GL_DONT_CARE;
    hintState.pointSmooth = GL_DONT_CARE;
    hintState.polygonSmooth = GL_DONT_CARE;

    pixelState.scissorTest = false;
    pixelState.scissorBox[0] = 0;
    pixelState.scissorBox[1] = 0;
    pixelState.scissorBox[2] = 0;
    pixelState.scissorBox[3] = 0;
    pixelState.alphaTest = false;
    pixelState.alphaTestFunc = GL_ALWAYS;
    pixelState.alphaTestRef = 0;
    pixelState.stencilTest = false;
    pixelState.stencilFunc = GL_ALWAYS;
    pixelState.stencilValueMask = 255;
    pixelState.stencilRef = 0;
    pixelState.stencilFail = GL_KEEP;
    pixelState.stencilPassDepthFail = GL_KEEP;
    pixelState.stencilPassDepthPass = GL_KEEP;
    pixelState.depthTest = true;
    pixelState.depthFunc = GL_LESS;
    pixelState.blend = false;
    pixelState.blendSrc = GL_SRC_ALPHA;
    pixelState.blendDest = GL_ONE_MINUS_SRC_ALPHA;
    pixelState.dither = false;
    pixelState.indexLogicOp = 0;
    pixelState.colorLogicOp = 0;
    pixelState.logicOpMode = GL_COPY;
	pixelState.samples = 1;

	framebufferState.accumClearValue = CColor::RGBA(0,0,0,0);
	framebufferState.colorClearValue = CColor::RGBA(0,0,0,0);
    framebufferState.colorWriteMask[0] = GL_TRUE;
    framebufferState.colorWriteMask[1] = GL_TRUE;
    framebufferState.colorWriteMask[2] = GL_TRUE;
    framebufferState.colorWriteMask[3] = GL_TRUE;
	framebufferState.depthClearValue = 1.0f;
	framebufferState.depthWriteMask = GL_TRUE;
	framebufferState.drawBuffer = GL_BACK;
    framebufferState.indexClearValue = 0.0f;
	framebufferState.indexWriteMask = ~0;
	framebufferState.stencilClearValue = 0;
    framebufferState.stencilWriteMask = ~0;

    lightingState.lightingEnable = GL_FALSE;
	lightingState.colorMaterialEnable = GL_FALSE;
	lightingState.colorMaterialParameter = GL_AMBIENT_AND_DIFFUSE;
	lightingState.colorMaterialFace = GL_FRONT_AND_BACK;
	lightingState.materialAmbient = CColor::RGBA(0.2f, 0.2f, 0.2f, 1.0f);
	lightingState.materialDiffuse = CColor::RGBA(0.8f,0.8f,0.8f,1.0f);
	lightingState.materialEmission = CColor::RGBA(0.0f,0.0f,0.0f,1.0f);
	lightingState.materialSpecular = CColor::RGBA(0.0f,0.0f,0.0f,1.0f);
	lightingState.materialShininess = 0.0f;
	lightingState.lightModelAmbient = CColor::RGBA(0.2f, 0.2f, 0.2f, 1.0f);
	lightingState.lightModelLocalViewer = GL_TRUE;     // specific for nice raptor display
	lightingState.lightModelTwoSide = GL_FALSE;
	lightingState.lightModelSeparateSpecular = false;
	for (int i=0;i<8;i++)
	{
		CRaptorDisplayConfig::GL_LIGHT_STATE* gls = &lightingState.light0 + i;

		gls->enable = GL_FALSE;
		gls->ambient = CColor::RGBA(0.0f,0.0f,0.0f,1.0f);
		gls->diffuse = CColor::RGBA(1.0f,1.0f,1.0f,1.0f);
		gls->specular = CColor::RGBA(1.0f,1.0f,1.0f,1.0f);
		gls->position = GL_COORD_VERTEX(0.0f,0.0f,1.0f,1.0f);
		gls->constantAttenuation = 1.0f;
		gls->linearAttenuation = 0.0f;
		gls->quadraticAttenuation = 0.0f;
		gls->spotDirection = GL_COORD_VERTEX(0.0f,0.0f,-1.0f,1.0f);
		gls->spotExponent = 0.0f;
		gls->spotCutOff = 180.0f;
		gls->colorIndexes = CColor::RGBA(0.0f,0.0f,0.0f,1.0f);
	}

	// TODO : complete initialisation with proper values
	memset(&currentState,0,sizeof(GL_CURRENT_STATE));
	memset(&arraysState,0,sizeof(GL_ARRAYS_STATE));
	memset(&transformState,0,sizeof(GL_TRANSFORM_STATE));
	memset(&coloringState,0,sizeof(GL_COLORING_STATE));
	memset(&rasterizeState,0,sizeof(GL_RASTERIZE_STATE));
	memset(&textureState,0,sizeof(GL_TEXTURE_STATE));

	transformState.viewport[0] = 0;
	transformState.viewport[1] = 0;
	transformState.viewport[2] = 640;
	transformState.viewport[3] = 480;
}

CRaptorDisplayConfig::~CRaptorDisplayConfig()
{

}

void CRaptorDisplayConfig::copyBaseConfig(const CRaptorDisplayConfig& config)
{
	x = config.x;
    y = config.y;
    width = config.width;
    height = config.height;
    draw_logo = config.draw_logo;
	status_bar = config.status_bar;
    display_mode = config.display_mode;
    acceleration = config.acceleration;
	antialias = config.antialias;
	swap_buffer = config.swap_buffer;
	renderer = config.renderer;
	bind_to_texture = config.bind_to_texture;
	overlay = config.overlay;
	double_buffer = config.double_buffer;
	depth_buffer = config.depth_buffer;
	stencil_buffer = config.stencil_buffer;
	accumulator_buffer = config.accumulator_buffer;

	refresh_rate = config.refresh_rate;
	caption = config.caption;
}

unsigned int CRaptorDisplayConfig::getNbSamples(void) const
{
	unsigned int samples = 1;
	switch(antialias)
	{
		case ANTIALIAS_2X:
			samples = 2;
			break;
		case ANTIALIAS_4X:
			samples = 4;
			break;
		case ANTIALIAS_5X:
			samples = 5;
			break;
		case ANTIALIAS_6X:
			samples = 6;
			break;
		case ANTIALIAS_8X:
			samples = 8;
			break;
		case ANTIALIAS_16X:
			samples = 16;
			break;
		case ANTIALIAS_32X:
			samples = 32;
			break;
		default:
			samples = 1;
			break;
	}

	return samples;
}

bool CRaptorDisplayConfig::glQueryConfig(unsigned long query)
{
	if (query == GL_CONFIG_STATE_QUERY)
		return true;

	if (query & GL_CURRENT_STATE_QUERY)
	{
		glGetFloatv(GL_CURRENT_COLOR,currentState.currentColor);
		glGetIntegerv(GL_CURRENT_INDEX,&currentState.currentIndex);
		glGetFloatv(GL_CURRENT_TEXTURE_COORDS,currentState.currentTextureCoords);
		glGetFloatv(GL_CURRENT_NORMAL,currentState.currentNormal);
		glGetFloatv(GL_CURRENT_RASTER_POSITION,currentState.currentRasterPosition);
		glGetFloatv(GL_CURRENT_RASTER_DISTANCE,&currentState.currentRasterDistance);
		glGetFloatv(GL_CURRENT_RASTER_COLOR,currentState.currentRasterColor);
		glGetIntegerv(GL_CURRENT_RASTER_INDEX,&currentState.currentRasterIndex);
		glGetFloatv(GL_CURRENT_RASTER_TEXTURE_COORDS,currentState.currentRasterTextureCoords);

		GLboolean b = GL_FALSE;
		glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&b);
		currentState.currentRasterPositionValid = (b == GL_TRUE);

		glGetBooleanv(GL_EDGE_FLAG,&b);
		currentState.edgeFlag = (b == GL_TRUE);
	}

	if (query & GL_ARRAYS_STATE_QUERY)
	{
		arraysState.vertexArray.enable = (GL_TRUE == glIsEnabled(GL_VERTEX_ARRAY));
		arraysState.normalArray.enable = (GL_TRUE == glIsEnabled(GL_NORMAL_ARRAY));
		arraysState.colorArray.enable = (GL_TRUE == glIsEnabled(GL_COLOR_ARRAY));
		arraysState.indexArray.enable = (GL_TRUE == glIsEnabled(GL_INDEX_ARRAY));
		arraysState.textureArray.enable = (GL_TRUE == glIsEnabled(GL_TEXTURE_COORD_ARRAY));
		arraysState.edgeArray.enable = (GL_TRUE == glIsEnabled(GL_EDGE_FLAG_ARRAY));

		glGetIntegerv(GL_VERTEX_ARRAY_SIZE,&arraysState.vertexArray.arraySize);
		arraysState.normalArray.arraySize = 3;
		glGetIntegerv(GL_COLOR_ARRAY_SIZE,&arraysState.colorArray.arraySize);
		arraysState.indexArray.arraySize = 1;
		glGetIntegerv(GL_TEXTURE_COORD_ARRAY_SIZE,&arraysState.textureArray.arraySize);
		arraysState.edgeArray.arraySize = 1;

		glGetIntegerv(GL_VERTEX_ARRAY_TYPE,&arraysState.vertexArray.arrayType);
		glGetIntegerv(GL_NORMAL_ARRAY_TYPE,&arraysState.normalArray.arrayType);
		glGetIntegerv(GL_COLOR_ARRAY_TYPE,&arraysState.colorArray.arrayType);
		glGetIntegerv(GL_INDEX_ARRAY_TYPE,&arraysState.indexArray.arrayType);
		glGetIntegerv(GL_TEXTURE_COORD_ARRAY_TYPE,&arraysState.textureArray.arrayType);
		arraysState.edgeArray.arrayType = 0;	// must be GLboolean

		glGetIntegerv(GL_VERTEX_ARRAY_STRIDE,&arraysState.vertexArray.arrayStride);
		glGetIntegerv(GL_NORMAL_ARRAY_STRIDE,&arraysState.normalArray.arrayStride);
		glGetIntegerv(GL_COLOR_ARRAY_STRIDE,&arraysState.colorArray.arrayStride);
		glGetIntegerv(GL_INDEX_ARRAY_STRIDE,&arraysState.indexArray.arrayStride);
		glGetIntegerv(GL_TEXTURE_COORD_ARRAY_STRIDE,&arraysState.textureArray.arrayStride);
		glGetIntegerv(GL_EDGE_FLAG_ARRAY_STRIDE,&arraysState.edgeArray.arrayStride);

		glGetPointerv(GL_VERTEX_ARRAY_POINTER,&arraysState.vertexArray.arrayPointer);
		glGetPointerv(GL_NORMAL_ARRAY_POINTER,&arraysState.normalArray.arrayPointer);
		glGetPointerv(GL_COLOR_ARRAY_POINTER,&arraysState.colorArray.arrayPointer);
		glGetPointerv(GL_INDEX_ARRAY_POINTER,&arraysState.indexArray.arrayPointer);
		glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER,&arraysState.textureArray.arrayPointer);
		glGetPointerv(GL_EDGE_FLAG_ARRAY_POINTER,&arraysState.edgeArray.arrayPointer);

#ifdef GL_EXT_vertex_weighting
		arraysState.weightArray.enable = glIsEnabled(GL_VERTEX_WEIGHT_ARRAY_EXT);
		glGetIntegerv(GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT,&arraysState.weightArray.arraySize);
		glGetIntegerv(GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT,&arraysState.weightArray.arrayType);
		glGetIntegerv(GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT,&arraysState.weightArray.arrayStride);
		glGetPointerv(GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT,&arraysState.weightArray.arrayPointer);
#endif
	}

	if (query & GL_TRANSFORM_STATE_QUERY)
	{
		glGetFloatv(GL_MODELVIEW_MATRIX,transformState.modelviewMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX,transformState.projectionMatrix);
		glGetFloatv(GL_TEXTURE_MATRIX,transformState.textureMatrix);
		glGetIntegerv(GL_VIEWPORT,transformState.viewport);
		glGetFloatv(GL_DEPTH_RANGE,transformState.depthRange);
		glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,&transformState.modelviewMatrixStack);
		glGetIntegerv(GL_PROJECTION_STACK_DEPTH,&transformState.projectionMatrixStack);
		glGetIntegerv(GL_TEXTURE_STACK_DEPTH,&transformState.textureMatrixStack);
		glGetIntegerv(GL_MATRIX_MODE,&transformState.matrixMode);

		transformState.normalize = (GL_TRUE == glIsEnabled(GL_NORMALIZE));

#ifdef GL_EXT_vertex_weighting
		transformState.weighting = (GL_TRUE == glIsEnabled(GL_VERTEX_WEIGHTING_EXT));
		glGetFloatv(GL_MODELVIEW1_MATRIX_EXT,transformState.modelviewMatrix1);
		glGetIntegerv(GL_MODELVIEW1_STACK_DEPTH_EXT,&transformState.modelviewMatrixStack1);
#endif	
	}

	if (query & GL_COLORING_STATE_QUERY)
	{
		glGetFloatv(GL_FOG_COLOR,coloringState.fogColor);
		glGetFloatv(GL_FOG_INDEX,&coloringState.fogIndex);
		glGetFloatv(GL_FOG_DENSITY,&coloringState.fogDensity);
		glGetFloatv(GL_FOG_START,&coloringState.fogStart);
		glGetFloatv(GL_FOG_END,&coloringState.fogEnd);
		glGetIntegerv(GL_FOG_MODE,&coloringState.fogMode);
		coloringState.fogEnable = (GL_TRUE == glIsEnabled(GL_FOG));
		glGetIntegerv(GL_SHADE_MODEL,&coloringState.shadeModel);
	}

	if (query & GL_LIGHT_STATE_QUERY)
	{
		lightingState.lightingEnable = (GL_TRUE == glIsEnabled(GL_LIGHTING));
		lightingState.colorMaterialEnable = (GL_TRUE == glIsEnabled(GL_COLOR_MATERIAL));
		glGetIntegerv(GL_COLOR_MATERIAL_PARAMETER,&lightingState.colorMaterialParameter);
		glGetIntegerv(GL_COLOR_MATERIAL_FACE,&lightingState.colorMaterialFace);
		glGetMaterialfv(GL_FRONT,GL_AMBIENT,lightingState.materialAmbient);
		glGetMaterialfv(GL_FRONT,GL_DIFFUSE,lightingState.materialDiffuse);
		glGetMaterialfv(GL_FRONT,GL_EMISSION,lightingState.materialEmission);
		glGetMaterialfv(GL_FRONT,GL_SPECULAR,lightingState.materialSpecular);
		glGetMaterialfv(GL_FRONT,GL_SHININESS,&lightingState.materialShininess);
		glGetFloatv(GL_LIGHT_MODEL_AMBIENT,lightingState.lightModelAmbient);

		GLboolean b = GL_FALSE;
		glGetBooleanv(GL_LIGHT_MODEL_LOCAL_VIEWER,&b);
		lightingState.lightModelLocalViewer = (b == GL_TRUE);

		glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE,&b);
		lightingState.lightModelTwoSide = (b == GL_TRUE);

		lightingState.lightModelSeparateSpecular = false;
#ifdef GL_VERSION_1_2
		GLint s = GL_SINGLE_COLOR;
		glGetIntegerv(GL_LIGHT_MODEL_COLOR_CONTROL,&s);
		lightingState.lightModelSeparateSpecular = (b == GL_SEPARATE_SPECULAR_COLOR);
#endif

		for (int i=0;i<8;i++)
		{
			CRaptorDisplayConfig::GL_LIGHT_STATE* gls = &lightingState.light0 + i;

			gls->enable = (GL_TRUE == glIsEnabled(GL_LIGHT0 + i));
			glGetLightfv(GL_LIGHT0 + i,GL_AMBIENT,gls->ambient);
			glGetLightfv(GL_LIGHT0 + i,GL_DIFFUSE,gls->diffuse);
			glGetLightfv(GL_LIGHT0 + i,GL_SPECULAR,gls->specular);
			glGetLightfv(GL_LIGHT0 + i,GL_POSITION,gls->position);
			glGetLightfv(GL_LIGHT0 + i,GL_CONSTANT_ATTENUATION,&gls->constantAttenuation);
			glGetLightfv(GL_LIGHT0 + i,GL_LINEAR_ATTENUATION,&gls->linearAttenuation);
			glGetLightfv(GL_LIGHT0 + i,GL_QUADRATIC_ATTENUATION,&gls->quadraticAttenuation);
			glGetLightfv(GL_LIGHT0 + i,GL_SPOT_DIRECTION,gls->spotDirection);
			glGetLightfv(GL_LIGHT0 + i,GL_SPOT_EXPONENT,&gls->spotExponent);
			glGetLightfv(GL_LIGHT0 + i,GL_SPOT_CUTOFF,&gls->spotCutOff);
			glGetMaterialfv(GL_FRONT,GL_COLOR_INDEXES ,gls->colorIndexes);
		}
	}

	if (query & GL_RASTERIZE_STATE_QUERY)
	{
		rasterizeState.multisample = false;
#if defined(GL_ARB_multisample)
		rasterizeState.multisample = (GL_TRUE == glIsEnabled(GL_MULTISAMPLE_ARB));
#endif
		glGetFloatv(GL_POINT_SIZE,&rasterizeState.pointSize);
		rasterizeState.pointSmooth = (GL_TRUE == glIsEnabled(GL_POINT_SMOOTH));
		glGetFloatv(GL_LINE_WIDTH,&rasterizeState.lineWidth);
		rasterizeState.lineSmooth = (GL_TRUE == glIsEnabled(GL_LINE_SMOOTH));
		int pattern;
		glGetIntegerv(GL_LINE_STIPPLE_PATTERN,&pattern);
		rasterizeState.lineStipplePattern = (GLushort)(pattern & 0xFFFF);
		glGetIntegerv(GL_LINE_STIPPLE_REPEAT,&rasterizeState.lineStippleRepeat);
		rasterizeState.lineStipple = (GL_TRUE == glIsEnabled(GL_LINE_STIPPLE));
		rasterizeState.cullFace = (GL_TRUE == glIsEnabled(GL_CULL_FACE));
		glGetIntegerv(GL_CULL_FACE_MODE,&rasterizeState.cullFaceMode);
		glGetIntegerv(GL_FRONT_FACE,&rasterizeState.frontFace);
		rasterizeState.polygonSmooth = (GL_TRUE == glIsEnabled(GL_POLYGON_SMOOTH));
		glGetIntegerv(GL_POLYGON_MODE,rasterizeState.polygonMode);
		glGetFloatv(GL_POLYGON_OFFSET_FACTOR,&rasterizeState.polygonOffsetFactor);
		//	unknown identifier ... gl.h is incomplete
		//glGetFloatv(GL_POLYGON_OFFSET_BIAS,&rasterizeState.polygonOffsetBias);
		rasterizeState.polygonOffsetPoint = (GL_TRUE == glIsEnabled(GL_POLYGON_OFFSET_POINT));
		rasterizeState.polygonOffsetLine = (GL_TRUE == glIsEnabled(GL_POLYGON_OFFSET_LINE));
		rasterizeState.polygonOffsetFill = (GL_TRUE == glIsEnabled(GL_POLYGON_OFFSET_FILL));
		rasterizeState.polygonStipple = (GL_TRUE == glIsEnabled(GL_POLYGON_STIPPLE));
	}

	if (query & GL_TEXTURE_STATE_QUERY)
	{
	}

	if (query & GL_PIXEL_STATE_QUERY)
	{
		pixelState.scissorTest = (GL_TRUE == glIsEnabled(GL_SCISSOR_TEST));
		glGetIntegerv(GL_SCISSOR_BOX,pixelState.scissorBox);
		pixelState.alphaTest = (GL_TRUE == glIsEnabled(GL_ALPHA_TEST));
		glGetIntegerv(GL_ALPHA_TEST_FUNC,&pixelState.alphaTestFunc);
		glGetFloatv(GL_ALPHA_TEST_REF,&pixelState.alphaTestRef);
		pixelState.stencilTest = (GL_TRUE == glIsEnabled(GL_STENCIL_TEST));
		glGetIntegerv(GL_STENCIL_FUNC,&pixelState.stencilFunc);
		glGetIntegerv(GL_STENCIL_VALUE_MASK,&pixelState.stencilValueMask);
		glGetIntegerv(GL_STENCIL_REF,&pixelState.stencilRef);
		glGetIntegerv(GL_STENCIL_FAIL,&pixelState.stencilFail);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,&pixelState.stencilPassDepthFail);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,&pixelState.stencilPassDepthPass);
		pixelState.depthTest = (GL_TRUE == glIsEnabled(GL_DEPTH_TEST));
		glGetIntegerv(GL_DEPTH_FUNC,&pixelState.depthFunc);
		pixelState.blend = (GL_TRUE == glIsEnabled(GL_BLEND));
		glGetIntegerv(GL_BLEND_SRC,&pixelState.blendSrc);
		glGetIntegerv(GL_BLEND_DST,&pixelState.blendDest);
		pixelState.dither = (GL_TRUE == glIsEnabled(GL_DITHER));
		pixelState.indexLogicOp = (GL_TRUE == glIsEnabled(GL_INDEX_LOGIC_OP));
		pixelState.colorLogicOp = (GL_TRUE == glIsEnabled(GL_COLOR_LOGIC_OP));
		glGetIntegerv(GL_LOGIC_OP_MODE,&pixelState.logicOpMode);
		pixelState.samples = 1;
#if defined(GL_ARB_multisample)
		glGetIntegerv(GL_SAMPLES_ARB,&pixelState.samples);
#endif
	}

	if (query & GL_FRAME_STATE_QUERY)
	{
		glGetIntegerv(GL_DRAW_BUFFER,&framebufferState.drawBuffer);
		glGetIntegerv(GL_INDEX_WRITEMASK,&framebufferState.indexWriteMask);
		GLboolean mask[4];
		glGetBooleanv(GL_COLOR_WRITEMASK,mask);
		framebufferState.colorWriteMask[0] = (GL_TRUE == mask[0]);
		framebufferState.colorWriteMask[1] = (GL_TRUE == mask[1]);
		framebufferState.colorWriteMask[2] = (GL_TRUE == mask[2]);
		framebufferState.colorWriteMask[3] = (GL_TRUE == mask[3]);

		glGetBooleanv(GL_DEPTH_WRITEMASK,&mask[0]);
		framebufferState.depthWriteMask = (GL_TRUE == mask[0]);

		glGetIntegerv(GL_STENCIL_WRITEMASK,&framebufferState.stencilWriteMask);
		glGetFloatv(GL_COLOR_CLEAR_VALUE,framebufferState.colorClearValue);
		glGetFloatv(GL_INDEX_CLEAR_VALUE,&framebufferState.indexClearValue);
		glGetFloatv(GL_DEPTH_CLEAR_VALUE, &framebufferState.depthClearValue);
		glGetIntegerv(GL_STENCIL_CLEAR_VALUE,&framebufferState.stencilClearValue);
		glGetFloatv(GL_ACCUM_CLEAR_VALUE,framebufferState.accumClearValue);
	}

	if (query & GL_HINT_STATE_QUERY)
	{
		glGetIntegerv(GL_PERSPECTIVE_CORRECTION_HINT,&hintState.perspectiveCorrection);
		glGetIntegerv(GL_POINT_SMOOTH_HINT,&hintState.pointSmooth);
		glGetIntegerv(GL_LINE_SMOOTH_HINT,&hintState.lineSmooth);
		glGetIntegerv(GL_POLYGON_SMOOTH_HINT,&hintState.polygonSmooth);
		glGetIntegerv(GL_FOG_HINT,&hintState.fog);
	}

	return true;
}

bool CRaptorDisplayConfig::glApplyConfig(unsigned long query) const
{
	if (query & GL_CURRENT_STATE_QUERY)
	{
		glColor4fv(currentState.currentColor);
		glIndexi(currentState.currentIndex);
		glTexCoord4fv(currentState.currentTextureCoords);
		glNormal3fv(currentState.currentNormal);
		glRasterPos4fv(currentState.currentRasterPosition);
		glEdgeFlag(currentState.edgeFlag);
	}

	if (query & GL_ARRAYS_STATE_QUERY)
	{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
#else
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
#endif

		if(arraysState.vertexArray.enable)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(arraysState.vertexArray.arraySize,
							arraysState.vertexArray.arrayType,
							arraysState.vertexArray.arrayStride,
							arraysState.vertexArray.arrayPointer);
		}
		if(arraysState.normalArray.enable)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(arraysState.normalArray.arrayType,
							arraysState.normalArray.arrayStride,
							arraysState.normalArray.arrayPointer);
		}
		if(arraysState.colorArray.enable)
		{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(	arraysState.colorArray.arraySize,
							arraysState.colorArray.arrayType,
							arraysState.colorArray.arrayStride,
							arraysState.colorArray.arrayPointer);
#else
			pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
#endif
		}
		if(arraysState.indexArray.enable)
		{
			glEnableClientState(GL_INDEX_ARRAY);
			glIndexPointer(	arraysState.indexArray.arrayType,
							arraysState.indexArray.arrayStride,
							arraysState.indexArray.arrayPointer);
		}
		if(arraysState.textureArray.enable)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(	arraysState.textureArray.arraySize,
								arraysState.textureArray.arrayType,
								arraysState.textureArray.arrayStride,
								arraysState.textureArray.arrayPointer);
		}
		if(arraysState.edgeArray.enable)
		{
			glEnableClientState(GL_EDGE_FLAG_ARRAY);
			glEdgeFlagPointer(	arraysState.edgeArray.arrayStride,
								arraysState.edgeArray.arrayPointer);
		}

#ifdef GL_EXT_vertex_weighting
		if(arraysState.weightArray.enable)
		{
			const CRaptorExtensions *const pExtensions = 
				CContextManager::GetInstance()->glGetExtensions();

			if (pExtensions->glVertexWeightPointerEXT != NULL)
			{
				glEnableClientState(GL_VERTEX_WEIGHT_ARRAY_EXT);
				pExtensions->glVertexWeightPointerEXT(	arraysState.weightArray.arraySize,
											arraysState.weightArray.arrayType,
											arraysState.weightArray.arrayStride,
											arraysState.weightArray.arrayPointer);
			}
		}
#endif

	}

	if (query & GL_TRANSFORM_STATE_QUERY)
	{
		glViewport(	transformState.viewport[0],
					transformState.viewport[1],
					transformState.viewport[2],
					transformState.viewport[3]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(transformState.modelviewMatrix);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(transformState.projectionMatrix);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(transformState.textureMatrix);
#ifdef GL_EXT_vertex_weighting
		if (transformState.weighting)
			glEnable(GL_VERTEX_WEIGHTING_EXT);
		else
			glDisable(GL_VERTEX_WEIGHTING_EXT);
		glMatrixMode(GL_MODELVIEW1_EXT);
		glLoadMatrixf(transformState.modelviewMatrix1);
#endif
		glMatrixMode(transformState.matrixMode);
		glDepthRange(transformState.depthRange.x,transformState.depthRange.y);
		if (transformState.normalize)
			glEnable(GL_NORMALIZE);
		else
			glDisable(GL_NORMALIZE);
	}

	if (query & GL_COLORING_STATE_QUERY)
	{
		glFogfv(GL_FOG_COLOR,coloringState.fogColor);
		glFogf(GL_FOG_INDEX,coloringState.fogIndex);
		glFogf(GL_FOG_DENSITY,coloringState.fogDensity);
		glFogf(GL_FOG_START,coloringState.fogStart);
		glFogf(GL_FOG_END,coloringState.fogEnd);
		glFogi(GL_FOG_MODE,coloringState.fogMode);
		if (coloringState.fogEnable)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
		glShadeModel(coloringState.shadeModel);
	}

	if (query & GL_LIGHT_STATE_QUERY)
	{
		if (lightingState.lightingEnable)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
		if (lightingState.colorMaterialEnable)
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);

		glColorMaterial(lightingState.colorMaterialFace,lightingState.colorMaterialParameter);
		glMaterialfv(GL_FRONT,GL_AMBIENT,lightingState.materialAmbient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,lightingState.materialDiffuse);
		glMaterialfv(GL_FRONT,GL_EMISSION,lightingState.materialEmission);
		glMaterialfv(GL_FRONT,GL_SPECULAR,lightingState.materialSpecular);
		glMaterialfv(GL_FRONT,GL_SHININESS,&lightingState.materialShininess);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lightingState.lightModelAmbient);

		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,lightingState.lightModelLocalViewer);
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,lightingState.lightModelTwoSide);
	#if defined GL_VERSION_1_2
		glLightModeli(	GL_LIGHT_MODEL_COLOR_CONTROL,
						(lightingState.lightModelSeparateSpecular ? GL_SEPARATE_SPECULAR_COLOR : GL_SINGLE_COLOR));
	#endif
#endif
		for (int i=0;i<8;i++)
		{
			const CRaptorDisplayConfig::GL_LIGHT_STATE* gls = &lightingState.light0 + i;

			if (gls->enable)
				glEnable(GL_LIGHT0 + i);
			else
				glDisable(GL_LIGHT0 + i);
			glLightfv(GL_LIGHT0 + i,GL_AMBIENT,gls->ambient);
			glLightfv(GL_LIGHT0 + i,GL_DIFFUSE,gls->ambient);
			glLightfv(GL_LIGHT0 + i,GL_SPECULAR,gls->ambient);
			glLightfv(GL_LIGHT0 + i,GL_POSITION,gls->ambient);
			glLightfv(GL_LIGHT0 + i,GL_CONSTANT_ATTENUATION,&gls->constantAttenuation);
			glLightfv(GL_LIGHT0 + i,GL_LINEAR_ATTENUATION,&gls->linearAttenuation);
			glLightfv(GL_LIGHT0 + i,GL_QUADRATIC_ATTENUATION,&gls->quadraticAttenuation);
			glLightfv(GL_LIGHT0 + i,GL_SPOT_DIRECTION,gls->spotDirection);
			glLightfv(GL_LIGHT0 + i,GL_SPOT_EXPONENT,&gls->spotExponent);
			glLightfv(GL_LIGHT0 + i,GL_SPOT_CUTOFF,&gls->spotCutOff);
			glMaterialfv(GL_FRONT,GL_COLOR_INDEXES ,gls->colorIndexes);
		}
	}

	if (query & GL_RASTERIZE_STATE_QUERY)
	{
		glPointSize(rasterizeState.pointSize);

		if (rasterizeState.pointSmooth)
			glEnable(GL_POINT_SMOOTH);
		else
			glDisable(GL_POINT_SMOOTH);

		glLineWidth(rasterizeState.lineWidth);

		if (rasterizeState.lineSmooth)
			glEnable(GL_LINE_SMOOTH);
		else
			glDisable(GL_LINE_SMOOTH);

		glLineStipple(rasterizeState.lineStippleRepeat,rasterizeState.lineStipplePattern);

		if (rasterizeState.lineStipple)
			glEnable(GL_LINE_STIPPLE);
		else
			glDisable(GL_LINE_STIPPLE);
		if (rasterizeState.cullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		glCullFace(rasterizeState.cullFaceMode);
		glFrontFace(rasterizeState.frontFace);
		
		if (rasterizeState.polygonSmooth)
			glEnable(GL_POLYGON_SMOOTH);

		glPolygonMode(GL_FRONT,rasterizeState.polygonMode[0]);
		glPolygonMode(GL_BACK,rasterizeState.polygonMode[1]);

		//glGetFloatv(GL_POLYGON_OFFSET_FACTOR,&rasterizeState.polygonOffsetFactor);
		//	unknown identifier ... gl.h is incomplete
		//glGetFloatv(GL_POLYGON_OFFSET_BIAS,&rasterizeState.polygonOffsetBias);
		
		if (rasterizeState.polygonOffsetPoint)
			glEnable(GL_POLYGON_OFFSET_POINT);
		else
			glDisable(GL_POLYGON_OFFSET_POINT);
		if (rasterizeState.polygonOffsetLine)
			glEnable(GL_POLYGON_OFFSET_LINE);
		else
			glDisable(GL_POLYGON_OFFSET_LINE);
		if (rasterizeState.polygonOffsetFill)
			glEnable(GL_POLYGON_OFFSET_FILL);
		else
			glDisable(GL_POLYGON_OFFSET_FILL);
		if (rasterizeState.polygonStipple)
			glEnable(GL_POLYGON_STIPPLE);
		else
			glDisable(GL_POLYGON_STIPPLE);

#if defined(GL_ARB_multisample)
		if (rasterizeState.multisample)
			glEnable(GL_MULTISAMPLE_ARB);
		else
			glDisable(GL_MULTISAMPLE_ARB);
#endif
	}

	if (query & GL_TEXTURE_STATE_QUERY)
	{
	}

	if (query & GL_PIXEL_STATE_QUERY)
	{
		if (pixelState.scissorTest)
			glEnable(GL_SCISSOR_TEST);
		if ((pixelState.scissorBox[2] != 0) && (pixelState.scissorBox[3] != 0))
			glScissor(pixelState.scissorBox[0],pixelState.scissorBox[1],pixelState.scissorBox[2],pixelState.scissorBox[3]);
  
		if (pixelState.alphaTest)
			glEnable(GL_ALPHA_TEST);

		glAlphaFunc(pixelState.alphaTestFunc,pixelState.alphaTestRef);
  
		if (pixelState.stencilTest)
			glEnable(GL_STENCIL_TEST);

		glStencilFunc(pixelState.stencilFunc,pixelState.stencilRef,pixelState.stencilValueMask);

		glStencilOp(pixelState.stencilFail,pixelState.stencilPassDepthFail,pixelState.stencilPassDepthPass);
  
		if (pixelState.depthTest)
			glEnable(GL_DEPTH_TEST);

		glDepthFunc(pixelState.depthFunc);

		if (pixelState.blend)
			glEnable(GL_BLEND);

		glBlendFunc(pixelState.blendSrc,pixelState.blendDest);

		if (pixelState.dither)
			glEnable(GL_DITHER);

		if (pixelState.indexLogicOp)
			glEnable(GL_INDEX_LOGIC_OP);
		if (pixelState.colorLogicOp)
			glEnable(GL_COLOR_LOGIC_OP);

		glLogicOp(pixelState.logicOpMode);

		//! pixel samples cannot be updated, as it is initialised at display creation
	}

	if (query & GL_FRAME_STATE_QUERY)
	{
		glDrawBuffer(framebufferState.drawBuffer);
		glColorMask(framebufferState.colorWriteMask[0],framebufferState.colorWriteMask[1],framebufferState.colorWriteMask[2],framebufferState.colorWriteMask[3]);
		glDepthMask(framebufferState.depthWriteMask);
		glStencilMask(framebufferState.stencilWriteMask);
		glClearColor(framebufferState.colorClearValue.r,framebufferState.colorClearValue.g,framebufferState.colorClearValue.b,framebufferState.colorClearValue.a);
		glClearDepth(framebufferState.depthClearValue);
		glClearStencil(framebufferState.stencilClearValue);

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glIndexMask(framebufferState.indexWriteMask);
		glClearIndex(framebufferState.indexClearValue);
		glClearAccum(framebufferState.accumClearValue.r,framebufferState.accumClearValue.g,framebufferState.accumClearValue.b,framebufferState.accumClearValue.a);
#endif

	}

	if (query & GL_HINT_STATE_QUERY)
	{
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,hintState.perspectiveCorrection);
		glHint(GL_POINT_SMOOTH_HINT,hintState.pointSmooth);
		glHint(GL_LINE_SMOOTH_HINT,hintState.lineSmooth);
		glHint(GL_POLYGON_SMOOTH_HINT,hintState.polygonSmooth);
		glHint(GL_FOG_HINT,hintState.fog);
	}

	return true;
}


