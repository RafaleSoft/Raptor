// RaptorDisplayConfig.h: interface for the CRaptorDisplayConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
#define AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CRaptorDisplayConfig
{
public:
	//	obvious defines ...
	#define CGL_MINREFRESHRATE			1
	#define CGL_MAXREFRESHRATE			255

	//	state query mode
	#define GL_CONFIG_STATE_QUERY		0x1000
	#define GL_CURRENT_STATE_QUERY		0x0001
	#define GL_ARRAYS_STATE_QUERY		0x0002	
	#define GL_TRANSFORM_STATE_QUERY	0x0004
	#define GL_COLORING_STATE_QUERY		0x0008
	#define GL_LIGHT_STATE_QUERY		0x0010
	#define GL_RASTERIZE_STATE_QUERY	0x0020
	#define GL_TEXTURE_STATE_QUERY		0x0040
	#define GL_PIXEL_STATE_QUERY		0x0080
	#define GL_FRAME_STATE_QUERY		0x0100
	#define GL_HINT_STATE_QUERY			0x0200

	//	state structures
	typedef struct GL_CURRENT_STATE_TAG
	{
		CColor::RGBA	currentColor;
		int				currentIndex;
		GL_COORD_VERTEX	currentTextureCoords;
		GL_COORD_VERTEX	currentNormal;
		GL_COORD_VERTEX	currentRasterPosition;
		float			currentRasterDistance;
		CColor::RGBA	currentRasterColor;
		int				currentRasterIndex;
		GL_COORD_VERTEX	currentRasterTextureCoords;
		bool			currentRasterPositionValid;
		bool			edgeFlag;
	} GL_CURRENT_STATE;

	typedef struct GL_ARRAY_STATE_TAG
	{
		bool		enable;
		int			arraySize;
		int			arrayType;
		int			arrayStride;
		void*		arrayPointer;
	} GL_ARRAY_STATE;

	typedef struct GL_ARRAYS_STATE_TAG
	{
		GL_ARRAY_STATE	vertexArray;
		GL_ARRAY_STATE	normalArray;
		GL_ARRAY_STATE	colorArray;
		GL_ARRAY_STATE	indexArray;
		GL_ARRAY_STATE	textureArray;
		GL_ARRAY_STATE	edgeArray;
		GL_ARRAY_STATE	weightArray;
	} GL_ARRAYS_STATE;

	typedef struct GL_TRANSFORM_STATE_TAG
	{
		GL_MATRIX		modelviewMatrix;
		GL_MATRIX		projectionMatrix;
		GL_MATRIX		textureMatrix;
		int				viewport[4];
		GL_COORD_VERTEX	depthRange;
		int				modelviewMatrixStack;
		int				projectionMatrixStack;
		int				textureMatrixStack;
		int				matrixMode;
		bool			normalize;
		bool			weighting;
		GL_MATRIX		modelviewMatrix1;
		int				modelviewMatrixStack1;
	} GL_TRANSFORM_STATE;

	typedef struct GL_COLORING_STATE_TAG
	{
		CColor::RGBA	fogColor;
		float	fogIndex;
		float	fogDensity;
		float	fogStart;
		float	fogEnd;
		int		fogMode;
		bool	fogEnable;
		int		shadeModel;
	} GL_COLORING_STATE;

	typedef struct GL_LIGHT_STATE_TAG
	{
		bool			enable;
		CColor::RGBA	ambient;
		CColor::RGBA	diffuse;
		CColor::RGBA	specular;
		GL_COORD_VERTEX	position;
		float			constantAttenuation;
		float			linearAttenuation;
		float			quadraticAttenuation;
		GL_COORD_VERTEX	spotDirection;
		float			spotCutOff;
		float			spotExponent;
		CColor::RGBA	colorIndexes;
	} GL_LIGHT_STATE;

	typedef struct GL_LIGHTING_STATE_TAG
	{
		bool			lightingEnable;
		bool			colorMaterialEnable;
		int				colorMaterialParameter;
		int				colorMaterialFace;
		CColor::RGBA	materialAmbient;
		CColor::RGBA	materialDiffuse;
		CColor::RGBA	materialSpecular;
		CColor::RGBA	materialEmission;
		float			materialShininess;
		CColor::RGBA	lightModelAmbient;
		bool			lightModelLocalViewer;
		bool			lightModelTwoSide;
		bool			lightModelSeparateSpecular;
		GL_LIGHT_STATE	light0;
		GL_LIGHT_STATE	light1;
		GL_LIGHT_STATE	light2;
		GL_LIGHT_STATE	light3;
		GL_LIGHT_STATE	light4;
		GL_LIGHT_STATE	light5;
		GL_LIGHT_STATE	light6;
		GL_LIGHT_STATE	light7;
	} GL_LIGHTING_STATE;

	typedef struct GL_RASTERIZE_STATE_TAG
	{
		bool			multisample;
		float			pointSize;
		bool			pointSmooth;
		float			lineWidth;
		bool			lineSmooth;
		unsigned short	lineStipplePattern;
		int				lineStippleRepeat;
		bool			lineStipple;
		bool			cullFace;
		int				cullFaceMode;
		int				frontFace;
		bool			polygonSmooth;
		int				polygonMode[2];
		float			polygonOffsetFactor;
		float			polygonOffsetBias;
		bool			polygonOffsetPoint;
		bool			polygonOffsetLine;
		bool			polygonOffsetFill;
		bool			polygonStipple;
		int				polygonStipplePattern;
	} GL_RASTERIZE_STATE;

	typedef struct GL_HINT_STATE_TAG
	{
		int			perspectiveCorrection;
		int			pointSmooth;
		int			lineSmooth;
		int			polygonSmooth;
		int			fog;
	} GL_HINT_STATE;

	typedef struct GL_TEXTURE_STATE_TAG
	{
		bool		texture1D;
		bool		texture2D;
		bool		textureBinding1D;
		bool		textureBinding2D;
		int			level;		//	in/out parameter
		int			width;
		int			height;
		int			border;
		int			internalFormat;
		int			redSize;
		int			blueSize;
		int			greenSize;
		int			alphaSize;
		int			luminanceSize;
		int			intensitySize;
		CColor::RGBA	borderColor;
		int			minFilter;
		int			magFilter;
		int			wrapS;
		int			wrapT;
		float		priority;
		bool		residency;
		int			envMode;
		GL_COORD_VERTEX	envColor;
		bool		genS;
		bool		genT;
		bool		genR;
		bool		genQ;
		GL_COORD_VERTEX	eyePlane;
		GL_COORD_VERTEX	objectPlane;
		int			genMode;
	} GL_TEXTURE_STATE;

	typedef struct GL_PIXEL_STATE_TAG
	{
		bool		scissorTest;
		int			scissorBox[4];
		bool		alphaTest;
		int			alphaTestFunc;
		float		alphaTestRef;
		bool		stencilTest;
		int			stencilFunc;
		int			stencilValueMask;
		int			stencilRef;
		int			stencilFail;
		int			stencilPassDepthFail;
		int			stencilPassDepthPass;
		bool		depthTest;
		int			depthFunc;
		bool		blend;
		int			blendSrc;
		int			blendDest;
		bool		dither;
		bool		indexLogicOp;
		bool		colorLogicOp;
		int			logicOpMode;
		int			samples;
	} GL_PIXEL_STATE;

	typedef struct GL_FRAME_STATE_TAG
	{
		int		drawBuffer;
		int		indexWriteMask;
		bool	colorWriteMask[4];
		bool	depthWriteMask;
		int		stencilWriteMask;
		CColor::RGBA	colorClearValue;
		float	indexClearValue;
		int		depthClearValue;
		int		stencilClearValue;
		CColor::RGBA	accumClearValue;
	} GL_FRAME_STATE;

	typedef enum
	{
		SOFTWARE,
		GENERIC,
		HARDWARE,
	} GL_ACCELERATION;

	typedef enum
	{
		ANTIALIAS_NONE,
		ANTIALIAS_2X,
		ANTIALIAS_4X,
		ANTIALIAS_5X,
		ANTIALIAS_6X,
		ANTIALIAS_8X,
		ANTIALIAS_16X
	} GL_ANTIALIAS;

	typedef enum
	{
		SWAP_UNDEFINED,
		SWAP_COPY,
		SWAP_EXCHANGE
	} GL_SWAPBUFFER;

	typedef enum
	{
		NATIVE,		// ScreenDisplay
		FILTERED,
		BUFFERED,
		TEXTURE,
		VULKAN
	} GL_DISPLAY_RENDERER;

public:
	CRaptorDisplayConfig();
	virtual ~CRaptorDisplayConfig();

	//! Returns the display core status. Query can be the oring of
	//! GL_CONFIG_STATE_QUERY for basic config ( width, height, ... ),
	//! or for OpenGL state machine status ( Need an active OpenGL Context )
	//! GL_CURRENT_STATE_QUERY, GL_ARRAYS_STATE_QUERY, GL_TRANSFORM_STATE_QUERY,
	//! GL_COLORING_STATE_QUERY, GL_LIGHT_STATE_QUERY, GL_RASTERIZE_STATE_QUERY,
	//! GL_TEXTURE_STATE_QUERY, GL_PIXEL_STATE_QUERY, GL_FRAME_STATE_QUERY, 
	//! GL_HINT_STATE_QUERY.			
	bool glQueryConfig(unsigned long query);

	//! Applies a status like one returned here above,
	//! except the basic config which can only be set at creation.
	bool glApplyConfig(unsigned long query) const;

	//!	Copy base attributes and leaves OpenGL states unchanged.
	//!	A whole copy is simply by using default copy operator.
	void copyBaseConfig(const CRaptorDisplayConfig& config);

	//!	A helper to query the bumbe of samples for current config
	unsigned int getNbSamples(void) const;


public:
	int				x;
	int				y;
	int				width;
	int				height;
	bool			draw_logo;
	bool			status_bar;
	unsigned int	display_mode;
	GL_ACCELERATION	acceleration;
	GL_ANTIALIAS	antialias;
	GL_SWAPBUFFER	swap_buffer;
	GL_DISPLAY_RENDERER	renderer;
	bool			overlay;
	bool			double_buffer;
	bool			depth_buffer;
	bool			stencil_buffer;
	struct
	{
		unsigned char	fps;
		bool			sync_to_monitor;
	} refresh_rate;
	string			caption;

	//	Current state
	GL_CURRENT_STATE		currentState;
	GL_ARRAYS_STATE			arraysState;	
	GL_TRANSFORM_STATE		transformState;
	GL_COLORING_STATE		coloringState;
	GL_LIGHTING_STATE		lightingState;
	GL_RASTERIZE_STATE		rasterizeState;
	GL_TEXTURE_STATE		textureState;
	GL_PIXEL_STATE			pixelState;
	GL_FRAME_STATE			framebufferState;
	GL_HINT_STATE			hintState;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)

