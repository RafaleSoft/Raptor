#ifndef __RAPTOR_HPP__
#define __RAPTOR_HPP__

/////////////////////////////////////////////////////
//	Raptor configuration
//
//	This header is VERY IMPORTANT.
//	It must not be undefined because a generated version
//	of any application using Raptor will not run properly
//	if the compiler options are not the same.
#include "Subsys/CodeGeneration.h"

//	Raptor type definitions
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

// Engine section
class C3DEngine;
class C3DPath;
class C3DScene;
class CAnimator;
class CBoundingBox;
class CEngineJob;
class CEnvironment;
class CGeometricModifier;
class CImageModifier;
class CLightModifier;
class CMirror;
class CModifier;
class CParticleManager;
class CPhysics;
class CTimeObject;
class CViewModifier;
class IViewPoint;

// GLHierarchy section
class C3DSet;
class C3DSqueleton;
class CBumppedGeometry;
class CEMBMGeometry;
class CFragmentProgram;
class CFragmentShader;
class CGLFont;
class CGLLayer;
class CGLLod;
class CGeometry;
class CGeometryPrimitive;
class CGeometryProgram;
class CLight;
class CLightGlow;
class CLightObserver;
class CMaterial;
class CObject3D;
class CObject3DContour;
class CObject3DInstance;
class CObject3DInstanceSet;
class CObject3DShadow;
class CObjectFactory;
class CObjectProperties;
class CObjectReference;
class CParticle;
class CPerlinNoise;
class CPersistence;
class CProjector;
class CRenderingProperties;
class CShadedGeometry;
class CShader;
class CShaderProgram;
class CSimpleObject;
class CTextureFactory;
class CTextureFactoryConfig;
class CTextureObject;
class CTextureSet;
class CTextureUnitSetup;
class CVertexProgram;
class CVertexShader;
class ITextureObject;

// SSE Engine section
class CSSE_3DEngine;
class CSSE_BoundingBox;
class CSSE_Object3DContour;

// System section
class CHostMemoryManager;
class Raptor;
class CRaptorApplication;
class CRaptorConsole;
class CRaptorDisplay;
class CRaptorDisplayConfig;
class CRaptorDisplayFilter;
class CRaptorErrorManager;
class CRaptorEventManager;
class CRaptorExtensions;
class CRaptorIO;
class CRaptorMessages;
class CRenderEntryPoint;

// Win32 section
class CGLDocument;
class CGLFrameWnd;
class CMDIChildWnd;
class CGLView;
class CGLWnd;
class CMultiGLWnd;

// Dialog section
class CRaptorStatus;


RAPTOR_NAMESPACE_END


#endif	// __RAPTOR_HPP__

