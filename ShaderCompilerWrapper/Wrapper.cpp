#include "pch.h"

#include "Wrapper.h"
#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "Engine/IViewPoint.h"
#include "Engine/ViewModifier.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/OpenGLShaderStage.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/GeometryShader.h"
#include "System/Image.h"
#include "System/RaptorConsole.h"
#include "System/RaptorGLExtensions.h"
#include "System/RaptorVKExtensions.h"

#include "ToolBox/BasicObjects.h"
#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/Imaging.h"
#include "DataManager/RaptorDataManager.h"

#include <iostream>
#include <fstream>

RAPTOR_NAMESPACE

bool RAPTOR_WRAPPER_API glInitRaptor(CRaptorConfig_t &config)
{
	CRaptorConfig cfg;

	cfg.m_bAutoDestroy = config.m_bAutoDestroy;
	cfg.m_bCompute = config.m_bCompute;
	cfg.m_bRelocation = config.m_bRelocation;
	cfg.m_bVulkan = config.m_bVulkan;
	cfg.m_logFile = config.m_logFile;
	cfg.m_uiGarbageSize = config.m_uiGarbageSize;
	cfg.m_uiPolygons = config.m_uiPolygons;
	cfg.m_uiTexels = config.m_uiTexels;
	cfg.m_uiUniforms = config.m_uiUniforms;
	cfg.m_uiVertices = config.m_uiVertices;
	cfg.setFilterSizeFactor(config.m_fSizeFactor);

	bool res = IRaptor::glInitRaptor(cfg);
	res = res && CImaging::installImagers();

	return res;
}

bool RAPTOR_WRAPPER_API glQuitRaptor(void)
{
	return IRaptor::glQuitRaptor();
}

bool RAPTOR_WRAPPER_API glCreateDisplay(CRaptorDisplayConfig_t &config, CRaptorDisplay* &display)
{
	CRaptorDisplayConfig glcs;

	glcs.x = config.x;
	glcs.y = config.y;
	glcs.width = config.width;
	glcs.height = config.height;
	glcs.draw_logo = true; // config.draw_logo;
	glcs.status_bar = false;	// No status bar when embedded in external API.
	glcs.display_mode = config.display_mode;

	glcs.acceleration = config.acceleration;
	glcs.antialias = config.antialias;
	glcs.swap_buffer = config.swap_buffer;
	glcs.renderer = config.renderer;
	
	glcs.bind_to_texture = config.bind_to_texture;
	glcs.overlay = false;	// No overlay when embedded in external API.
	glcs.double_buffer = config.double_buffer;
	glcs.depth_buffer = config.depth_buffer;
	glcs.stencil_buffer = config.stencil_buffer;
	glcs.accumulator_buffer = config.accumulator_buffer;
	glcs.refresh_rate.fps = config.fps;
	glcs.refresh_rate.sync_to_monitor = config.sync_to_monitor;
	glcs.caption = config.caption;

	display = NULL;
	CRaptorDisplay *pDisplay = IRaptor::glCreateDisplay(glcs);

	if (NULL == pDisplay)
		return false;

	display = pDisplay;
	return true;
}

bool RAPTOR_WRAPPER_API glDestroyDisplay(CRaptorDisplay* display, HDC dc)
{
	if (NULL == display)
		return false;

	RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS, dc);
	display->glvkBindDisplay(device);
	display->glvkReleaseResources();
	display->glvkUnBindDisplay();

	IRaptor::glDestroyDisplay(display);

	return true;
}

bool RAPTOR_WRAPPER_API glBindDisplay(CRaptorDisplay* display, HDC dc)
{
	if (NULL == display)
		return false;
	if (NULL == dc)
		return false;

	RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS, dc);
	return display->glvkBindDisplay(device);
}

bool RAPTOR_WRAPPER_API glUnBindDisplay(CRaptorDisplay* display)
{
	if (NULL == display)
		return false;

	return display->glvkUnBindDisplay();
}

bool RAPTOR_WRAPPER_API glRender(void)
{
	IRaptor::glRender();

	return true;
}

class CRenderer : public CRenderEntryPoint
{
public:
	CRenderer(CRaptorDisplay* display): m_pDisplay(display) {};
	virtual  ~CRenderer() {};

	virtual void glRender()
	{
		m_pDisplay->glRender();
	}

	void glInitRenderer();

private:
	CRaptorDisplay* m_pDisplay;
};

CShadedGeometry *object = NULL;

void CRenderer::glInitRenderer()
{
	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager != NULL)
	{
		//	Change package and erase previous files in case of updates
		dataManager->managePackage("ShaderCompiler.pck");
		std::string teapot = dataManager->exportFile("TEAPOT.3DS");

		C3DSet *set = NULL;
		CRaptorToolBox::load3DStudioScene(teapot, set);
		set->scale(0.06f, 0.06f, 0.06f);
		C3DSet::C3DSetIterator it = set->getIterator();
		object = (CShadedGeometry*)(set->getChild(it++));
		GL_COORD_VERTEX c;
		object->getEditor().minimize();
		object->getCenter(c);
		object->translate(-c.x, -c.y, -c.z);
		object->rotationX(-90.0f);
		object->addModel(CGeometry::RENDERING_MODEL::CGL_NORMALS);
		object->addModel(CGeometry::RENDERING_MODEL::CGL_TANGENTS);

		const CGeometryEditor &pEditor = object->getEditor();
		pEditor.genBinormals();
	}
	else
	{
		CBasicObjects::CCube *obj = new CBasicObjects::CCube();
		object = obj;
	}

	CShader* s = object->getShader();
	CMaterial *pMat = s->getMaterial();
	pMat->setAmbient(0.1f, 0.1f, 0.1f, 1.0f);
	pMat->setDiffuse(0.3f, 0.4f, 0.8f, 1.0f);
	pMat->setSpecular(0.7f, 0.7f, 0.7f, 1.0f);
	pMat->setEmission(0.0f, 0.0f, 0.0f, 1.0f);
	pMat->setShininess(128.0f);

	IRenderingProperties &props = m_pDisplay->getRenderingProperties();
	props.enableLighting.disableTexturing;

	CRaptorConsole *pConsole = IRaptor::GetConsole();
	if (!pConsole->glInit("", true))
		pConsole->glInit("", false);
	pConsole->showStatus(true);
	pConsole->showFPS(true);
	pConsole->showFrameTime(true);
	pConsole->showObjectCount(true);
	pConsole->showTriangleCount(true);
	pConsole->activateConsole(true);

	CLight *pLight = new CLight("MAIN_LIGHT");
	pLight->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->setSpecular(1.0f, 0.0f, 1.0f, 1.0f);
	pLight->setLightPosition(GL_COORD_VERTEX(10.0f, 10.0f, 10.0f, 1.0f));
	pLight->setLightDirection(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f));

	CViewModifier *vm = new CViewModifier("MyCube");
	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.2f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.3f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.3f;
	ty.a1 = 0.2f;
	ty.a0 = 0.005f; //0;
	vm->addAction(CViewModifier::ROTATE_VIEW, tx, ty, tz);
	vm->setObject(object);

	C3DScene *pScene = m_pDisplay->getRootScene();
	pScene->addObject(vm->getObject());
	pScene->addLight(pLight);

	IViewPoint *vp = m_pDisplay->getViewPoint();
	vp->setPosition(0.0f, 0.0f, 2.75f, IViewPoint::EYE);
	vp->setPosition(0.0f, 0.0f, 0.0f, IViewPoint::TARGET);

	CTimeObject::setTimeFactor(1.0f);
	CAnimator *pAnimator = new CAnimator();
	CAnimator::SetAnimator(pAnimator);
}

bool RAPTOR_WRAPPER_API glCreateRaptorRenderer(CRaptorDisplay* display, CRenderEntryPoint* &renderer)
{
	if (NULL == display)
		return false;

	CRenderer *r = new CRenderer(display);
	r->glInitRenderer();
	renderer = r;

	return true;
}

bool RAPTOR_WRAPPER_API glSetMaps(const char* diffuse, const char* normal)
{
	if (NULL == object)
		return false;

	bool upadateMap = false;

	if ((NULL != diffuse) && (strlen(diffuse) > 0))
	{
		CImage i;
		CVaArray<CImage::IImageOP*> ops;
		if (i.loadImage(diffuse, ops))
		{
			CShader *shader = object->getShader();
			CTextureUnitSetup *tmu = shader->glGetTextureUnitsSetup();

			CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
			ITextureObject *t = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
			factory.glLoadTexture(t, i);
			tmu->setDiffuseMap(t);

			object->addModel(CGeometry::RENDERING_MODEL::CGL_TEXTURE);

			upadateMap = true;
			IRenderingProperties &props = CRaptorDisplay::GetCurrentDisplay()->getRenderingProperties();
			props.enableLighting.enableTexturing;
		}
	}
		
	if ((NULL != normal) && (strlen(normal) > 0))
	{
		CImage i;
		CVaArray<CImage::IImageOP*> ops;
		if (i.loadImage(normal, ops))
		{
			CShader *shader = object->getShader();
			CTextureUnitSetup *tmu = shader->glGetTextureUnitsSetup();

			CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
			ITextureObject *t = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
			factory.glLoadTexture(t, i);
			tmu->setNormalMap(t);

			object->addModel(CGeometry::RENDERING_MODEL::CGL_TEXTURE);

			upadateMap = true;
			IRenderingProperties &props = CRaptorDisplay::GetCurrentDisplay()->getRenderingProperties();
			props.enableLighting.enableTexturing;
		}
	}

	return upadateMap;
}

bool RAPTOR_WRAPPER_API glSetShaders(const char* vertex, const char* geometry, const char* fragment, const char **log)
{
	if (NULL == object)
		return false;

	bool compileShader = false;

	CShader *shader = object->getShader();
	COpenGLShaderStage *stage = shader->glGetOpenGLShader("GL_SHADER_STAGE");

	if ((NULL != vertex) && (strlen(vertex) > 0))
	{
		CVertexShader *vs = stage->glGetVertexShader("VERTEX_SHADER");
		vs->glLoadProgramFromFile(vertex);
		compileShader = true;
	}

	if ((NULL != geometry) && (strlen(geometry) > 0))
	{
		CGeometryShader *gs = stage->glGetGeometryShader("GEOMETRY_SHADER");
		gs->glLoadProgramFromFile(geometry);
		compileShader = true;
	}

	if ((NULL != fragment) && (strlen(fragment) > 0))
	{
		CFragmentShader *fs = stage->glGetFragmentShader("FRAGMENT_SHADER");
		fs->glLoadProgramFromFile(fragment);
		compileShader = true;
	}

	std::string log_msg = "";
	if (compileShader)
	{
		if (stage->glCompileShader())
		{
			log_msg = "Shader stage compiled successfully";
			compileShader = true;
		}
		else
		{
			log_msg = "Failed to compile shader stage";
			compileShader = false;
		}
	}

	if (NULL != log)
	{
		char *msg = (char*)calloc(log_msg.length() + 1, 1);
		memcpy(msg, log_msg.c_str(), log_msg.length());
		log[0] = msg;

		if (!compileShader)
		{
			CRaptorErrorManager *pErr = IRaptor::GetErrorManager();
			int nerr = pErr->getLastRaptorError();
			CRaptorErrorManager::GL_RAPTOR_ERROR err;
			pErr->getRaptorError(nerr, err);
		}
	}

	return compileShader;
}

void write_int(GLenum pval, const char* pname, ofstream& out)
{
	GLint iparam = 0;
	glGetIntegerv(pval, &iparam);
	out.write(pname, strlen(pname));
	stringstream str;
	str << iparam;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}

bool RAPTOR_WRAPPER_API glDiag(void)
{
	char buffer[MAX_PATH];
	GetEnvironmentVariable("TMP", buffer, MAX_PATH);
	std::string filename = buffer;
	filename += "\\";
	filename += "gldiag.txt";

	ofstream gldiag;
	gldiag.open(filename, ios_base::trunc);
	if (gldiag.is_open())
	{
		std::streamsize nl = strlen("\n");

		const char *vendor = (char*)glGetString(GL_VENDOR);
		gldiag.write(vendor, strlen(vendor)); gldiag.write("\n", nl);

		const char *renderer = (char*)glGetString(GL_RENDERER);
		gldiag.write(renderer, strlen(renderer)); gldiag.write("\n", nl);

		const char *version = (char*)glGetString(GL_VERSION);
		gldiag.write(version, strlen(version)); gldiag.write("\n", nl);

		const char *glsl = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
		gldiag.write(glsl, strlen(glsl)); gldiag.write("\n", nl);

		gldiag.write("95\n", 2 + nl);

		write_int(GL_MAX_3D_TEXTURE_SIZE, "MAX_3D_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_ARRAY_TEXTURE_LAYERS, "MAX_ARRAY_TEXTURE_LAYERS ", gldiag);
		write_int(GL_MAX_ATTRIB_STACK_DEPTH, "MAX_ATTRIB_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, "MAX_CLIENT_ATTRIB_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_CLIP_DISTANCES, "MAX_CLIP_DISTANCES ", gldiag);
		write_int(GL_MAX_CLIP_PLANES, "MAX_CLIP_PLANES ", gldiag);
		write_int(GL_MAX_COLOR_ATTACHMENTS, "MAX_COLOR_ATTACHMENTS ", gldiag);
		write_int(GL_MAX_COLOR_MATRIX_STACK_DEPTH, "MAX_COLOR_MATRIX_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_COLOR_TEXTURE_SAMPLES, "MAX_COLOR_TEXTURE_SAMPLES ", gldiag);
		write_int(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, "MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS ", gldiag);
		write_int(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, "MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS ", gldiag);
		write_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "MAX_COMBINED_TEXTURE_IMAGE_UNITS ", gldiag);
		write_int(GL_MAX_COMBINED_UNIFORM_BLOCKS, "MAX_COMBINED_UNIFORM_BLOCKS ", gldiag);
		write_int(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, "MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS ", gldiag);
		write_int(GL_MAX_CONVOLUTION_HEIGHT, "MAX_CONVOLUTION_HEIGHT ", gldiag);
		write_int(GL_MAX_CONVOLUTION_WIDTH, "MAX_CONVOLUTION_WIDTH ", gldiag);
		write_int(GL_MAX_CUBE_MAP_TEXTURE_SIZE, "MAX_CUBE_MAP_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_DEBUG_MESSAGE_LENGTH, "MAX_DEBUG_MESSAGE_LENGTH ", gldiag);
		write_int(GL_MAX_DEBUG_LOGGED_MESSAGES, "MAX_DEBUG_LOGGED_MESSAGES ", gldiag);
		write_int(GL_MAX_DEBUG_GROUP_STACK_DEPTH, "MAX_DEBUG_GROUP_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_DEPTH_TEXTURE_SAMPLES, "MAX_DEPTH_TEXTURE_SAMPLES ", gldiag);
		write_int(GL_MAX_DRAW_BUFFERS, "MAX_DRAW_BUFFERS ", gldiag);
		write_int(GL_MAX_ELEMENTS_VERTICES, "MAX_ELEMENTS_VERTICES ", gldiag);
		write_int(GL_MAX_ELEMENTS_INDICES, "MAX_ELEMENTS_INDICES ", gldiag);
		write_int(GL_MAX_EVAL_ORDER, "MAX_EVAL_ORDER ", gldiag);
		write_int(GL_MAX_FRAGMENT_INPUT_COMPONENTS, "MAX_FRAGMENT_INPUT_COMPONENTS ", gldiag);
		write_int(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, "MAX_FRAGMENT_UNIFORM_BLOCKS ", gldiag);
		write_int(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, "MAX_FRAGMENT_UNIFORM_COMPONENTS ", gldiag);
		write_int(GL_MAX_FRAMEBUFFER_WIDTH, "MAX_FRAMEBUFFER_WIDTH ", gldiag);
		write_int(GL_MAX_FRAMEBUFFER_HEIGHT, "MAX_FRAMEBUFFER_HEIGHT ", gldiag);
		write_int(GL_MAX_FRAMEBUFFER_LAYERS, "MAX_FRAMEBUFFER_LAYERS ", gldiag);
		write_int(GL_MAX_FRAMEBUFFER_SAMPLES, "MAX_FRAMEBUFFER_SAMPLES ", gldiag);
		write_int(GL_MAX_GENERAL_COMBINERS_NV, "MAX_GENERAL_COMBINERS_NV ", gldiag);
		write_int(GL_MAX_GEOMETRY_INPUT_COMPONENTS, "MAX_GEOMETRY_INPUT_COMPONENTS ", gldiag);
		write_int(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, "MAX_GEOMETRY_OUTPUT_COMPONENTS ", gldiag);
		write_int(GL_MAX_GEOMETRY_OUTPUT_VERTICES, "MAX_GEOMETRY_OUTPUT_VERTICES ", gldiag);
		write_int(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, "MAX_GEOMETRY_TEXTURE_IMAGE_UNITS ", gldiag);
		write_int(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, "MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS ", gldiag);
		write_int(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, "MAX_GEOMETRY_UNIFORM_BLOCKS ", gldiag);
		write_int(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, "MAX_GEOMETRY_UNIFORM_COMPONENTS ", gldiag);
		write_int(GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB, "MAX_GEOMETRY_VARYING_COMPONENTS_ARB ", gldiag);
		write_int(GL_MAX_INTEGER_SAMPLES, "MAX_INTEGER_SAMPLES ", gldiag);
		write_int(GL_MAX_LIGHTS, "MAX_LIGHTS ", gldiag);
		write_int(GL_MAX_LIST_NESTING, "MAX_LIST_NESTING ", gldiag);
		//write_int(GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB, "MAX_MATRIX_PALETTE_STACK_DEPTH_ARB ", gldiag);
		write_int(GL_MAX_MODELVIEW_STACK_DEPTH, "MAX_MODELVIEW_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_NAME_STACK_DEPTH, "MAX_NAME_STACK_DEPTH ", gldiag);
		//write_int(GL_MAX_PALETTE_MATRICES_ARB, "MAX_PALETTE_MATRICES_ARB ", gldiag);
		write_int(GL_MAX_PIXEL_MAP_TABLE, "MAX_PIXEL_MAP_TABLE ", gldiag);
		write_int(GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB, "MAX_PROGRAM_ADDRESS_REGISTERS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB, "MAX_PROGRAM_ALU_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_ATTRIBS_ARB, "MAX_PROGRAM_ATTRIBS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, "MAX_PROGRAM_ENV_PARAMETERS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_INSTRUCTIONS_ARB, "MAX_PROGRAM_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, "MAX_PROGRAM_LOCAL_PARAMETERS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_MATRICES_ARB, "MAX_PROGRAM_MATRICES_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB, "MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, "MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, "MAX_PROGRAM_NATIVE_ATTRIBS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, "MAX_PROGRAM_NATIVE_PARAMETERS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, "MAX_PROGRAM_NATIVE_TEMPORARIES_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB, "MAX_PROGRAM_TEX_INDIRECTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, "MAX_PROGRAM_TEX_INSTRUCTIONS_ARB ", gldiag);
		write_int(GL_MAX_PROGRAM_TEXEL_OFFSET, "MAX_PROGRAM_TEXEL_OFFSET ", gldiag);
		write_int(GL_MAX_PROJECTION_STACK_DEPTH, "MAX_PROJECTION_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_RECTANGLE_TEXTURE_SIZE, "MAX_RECTANGLE_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_RENDERBUFFER_SIZE, "MAX_RENDERBUFFER_SIZE ", gldiag);
		write_int(GL_MAX_SAMPLES, "MAX_SAMPLES ", gldiag);
		write_int(GL_MAX_SAMPLE_MASK_WORDS, "MAX_SAMPLE_MASK_WORDS ", gldiag);
		write_int(GL_MAX_SERVER_WAIT_TIMEOUT, "MAX_SERVER_WAIT_TIMEOUT ", gldiag);
		write_int(GL_MAX_TEXTURE_BUFFER_SIZE, "MAX_TEXTURE_BUFFER_SIZE ", gldiag);
		write_int(GL_MAX_TEXTURE_COORDS, "MAX_TEXTURE_COORDS ", gldiag);
		write_int(GL_MAX_TEXTURE_IMAGE_UNITS, "MAX_TEXTURE_IMAGE_UNITS ", gldiag);
		write_int(GL_MAX_TEXTURE_LOD_BIAS, "MAX_TEXTURE_LOD_BIAS ", gldiag);
		write_int(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, "MAX_TEXTURE_MAX_ANISOTROPY_EXT ", gldiag);
		write_int(GL_MAX_TEXTURE_SIZE, "MAX_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_TEXTURE_STACK_DEPTH, "MAX_TEXTURE_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_TEXTURE_UNITS, "MAX_TEXTURE_UNITS ", gldiag);
		write_int(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, "MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS ", gldiag);
		write_int(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, "MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS ", gldiag);
		write_int(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, "MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS ", gldiag);
		write_int(GL_MAX_UNIFORM_BUFFER_BINDINGS, "MAX_UNIFORM_BUFFER_BINDINGS ", gldiag);
		write_int(GL_MAX_UNIFORM_BLOCK_SIZE, "MAX_UNIFORM_BLOCK_SIZE ", gldiag);
		write_int(GL_MAX_VARYING_COMPONENTS, "MAX_VARYING_COMPONENTS ", gldiag);
		write_int(GL_MAX_VARYING_FLOATS, "MAX_VARYING_FLOATS ", gldiag);
		write_int(GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV, "MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV ", gldiag);
		write_int(GL_MAX_VERTEX_ATTRIBS, "MAX_VERTEX_ATTRIBS ", gldiag);
		write_int(GL_MAX_VERTEX_OUTPUT_COMPONENTS, "MAX_VERTEX_OUTPUT_COMPONENTS ", gldiag);
		write_int(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "MAX_VERTEX_TEXTURE_IMAGE_UNITS ", gldiag);
		write_int(GL_MAX_VERTEX_UNIFORM_BLOCKS, "MAX_VERTEX_UNIFORM_BLOCKS ", gldiag);
		write_int(GL_MAX_VERTEX_UNIFORM_COMPONENTS, "MAX_VERTEX_UNIFORM_COMPONENTS ", gldiag);

		//write_int(GL_MAX_VERTEX_UNITS_ARB, "MAX_VERTEX_UNITS_ARB ", gldiag);
		write_int(GL_MAX_VERTEX_VARYING_COMPONENTS_ARB, "MAX_VERTEX_VARYING_COMPONENTS_ARB ", gldiag);

		GLint iparam[2];
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &iparam[0]);
		gldiag.write("MAX_VIEWPORT_DIMS ", strlen("MAX_VIEWPORT_DIMS "));
		stringstream str;
		str << iparam[0] << "x" << iparam[1] << std::endl;
		gldiag.write(str.str().c_str(), str.str().length());


		const CRaptorGLExtensions *ext = IRaptor::glGetExtensions();
		const std::string &exts = ext->glExtensions();

		size_t p1 = 0;
		size_t p2 = exts.find(' ', p1);
		while (std::string::npos != p2)
		{
			std::string e = exts.substr(p1, p2 - p1);
			gldiag.write(e.c_str(), e.length()); gldiag.write("\n", nl);

			p1 = p2 + 1;
			p2 = exts.find(' ', p1);
		}

		std::string e = exts.substr(p1);
		gldiag.write(e.c_str(), e.length()); gldiag.write("\n", nl);

		gldiag.close();
	}

	return true;
}

std::string gtVKExtensions(VkPhysicalDeviceProperties **vkprops)
{
	std::string vkextensions = "";

	char buffer[MAX_PATH];
	GetEnvironmentVariable("VULKAN_BIN_PATH", (LPTSTR)buffer, MAX_PATH);
	std::string vkpath = buffer;
	vkpath += "\\VULKAN-1.DLL";

	HMODULE module = LoadLibrary(vkpath.c_str());
	if (module != NULL)
	{
		PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)(GetProcAddress(module, "vkCreateInstance"));
		VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
													NULL,                                   // const void* pNext;
													0,                                      // VkInstanceCreateFlags flags;
													NULL,                                   // const VkApplicationInfo* pApplicationInfo;
													0,                                      // uint32_t enabledLayerNameCount;
													NULL,                                   // const char* const* ppEnabledLayerNames;
													0,                                      // uint32_t enabledExtensionNameCount;
													NULL };									// const char* const* ppEnabledExtensionNames;
		VkInstance inst = NULL;
		if (VK_SUCCESS == vkCreateInstance(&instanceCreateInfo, NULL, &inst))
		{
			PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties =
				(PFN_vkEnumerateInstanceExtensionProperties)(GetProcAddress(module, "vkEnumerateInstanceExtensionProperties"));
			uint32_t pPropertyCount = 0;
			if (VK_SUCCESS == vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, NULL))
			{
				VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
				vkEnumerateInstanceExtensionProperties(NULL, &pPropertyCount, pProperties);
				for (uint32_t i = 0; i < pPropertyCount; i++)
				{
					vkextensions += pProperties[i].extensionName;
					vkextensions += " ";
				}
				delete[] pProperties;
			}

			PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices =
				(PFN_vkEnumeratePhysicalDevices)(GetProcAddress(module, "vkEnumeratePhysicalDevices"));
			
			uint32_t pPhysicalDeviceCount = 0;
			VkResult res = vkEnumeratePhysicalDevices(inst, &pPhysicalDeviceCount, NULL);
			if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
			{
				VkPhysicalDevice *m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
				res = vkEnumeratePhysicalDevices(inst, &pPhysicalDeviceCount, m_pPhysicalDevices);
				if (VK_SUCCESS == res)
				{
					*vkprops = new VkPhysicalDeviceProperties[pPhysicalDeviceCount];
					for (uint32_t i = 0; i < pPhysicalDeviceCount; i++)
					{
						VkPhysicalDevice device = m_pPhysicalDevices[i];
						CRaptorVKExtensions::vkGetPhysicalDeviceProperties(device, &(*vkprops)[i]);
					}

					PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties =
						(PFN_vkEnumerateDeviceExtensionProperties)(GetProcAddress(module, "vkEnumerateDeviceExtensionProperties"));
					for (uint32_t i = 0; i < pPhysicalDeviceCount; i++)
					{
						pPropertyCount = 0;
						res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i], NULL, &pPropertyCount, NULL);
						if ((VK_SUCCESS == res) && (pPropertyCount > 0))
						{
							VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
							res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i], NULL, &pPropertyCount, pProperties);
							for (uint32_t j = 0; j < pPropertyCount; j++)
							{
								vkextensions += pProperties[j].extensionName;
								vkextensions += " ";
							}
							delete[] pProperties;
						}
					}
				}

				delete[] m_pPhysicalDevices;
			}
		}

		PFN_vkDestroyInstance vkDestroyInstance = (PFN_vkDestroyInstance)(GetProcAddress(module, "vkDestroyInstance"));
		vkDestroyInstance(inst, NULL);
	}
	FreeLibrary(module);

	return vkextensions;
}


void write_sint(const char* pname, int32_t pval, ofstream& out)
{
	out.write(pname, strlen(pname));
	stringstream str;
	str << pval;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}
void write_int(const char* pname, uint32_t pval, ofstream& out)
{
	out.write(pname, strlen(pname));
	stringstream str;
	str << pval;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}
void write_lint(const char* pname, uint64_t pval, ofstream& out)
{
	out.write(pname, strlen(pname));
	stringstream str;
	str << pval;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}
void write_float(const char* pname, float pval, ofstream& out)
{
	out.write(pname, strlen(pname));
	stringstream str;
	str << pval;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}
void write_sample(const char* pname, VkSampleCountFlags pval, ofstream& out)
{
	out.write(pname, strlen(pname));
	
	stringstream str;
	if (VK_SAMPLE_COUNT_1_BIT & pval)
		str << ":1_bit:";
	if (VK_SAMPLE_COUNT_2_BIT & pval)
		str << "2_bits:";
	if (VK_SAMPLE_COUNT_4_BIT & pval)
		str << "4_bits:";
	if (VK_SAMPLE_COUNT_8_BIT & pval)
		str << "8_bits:";
	if (VK_SAMPLE_COUNT_16_BIT & pval)
		str << "16_bits:";
	if (VK_SAMPLE_COUNT_32_BIT & pval)
		str << "32_bits:";
	if (VK_SAMPLE_COUNT_64_BIT & pval)
		str << "64_bits:";
	
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}

bool RAPTOR_WRAPPER_API vkDiag(void)
{
	VkPhysicalDeviceProperties *vkprops = NULL;
	const std::string &exts = gtVKExtensions(&vkprops);

	char buffer[MAX_PATH];
	GetEnvironmentVariable("TMP", buffer, MAX_PATH);
	std::string filename = buffer;
	filename += "\\";
	filename += "vkdiag.txt";

	ofstream vkdiag;
	vkdiag.open(filename, ios_base::trunc);
	if (vkdiag.is_open())
	{
		std::streamsize nl = strlen("\n");

		// From : https://devicehunt.com/all-pci-vendors
		const char *vendor = "Vulkan Vendor";
		if (vkprops->vendorID == 4318)
			vendor = "nVidia Corporation";
		else if ((vkprops->vendorID == 4098) || (vkprops->vendorID == 4130))
			vendor = "Advanced Micro Devices Inc.";
		else if (vkprops->vendorID == 5140)
			vendor = "Microsoft Corporation";
		else if (vkprops->vendorID == 32902)	// a.k.a. 8086
			vendor = "Intel Corporation";

		vkdiag.write(vendor, strlen(vendor)); vkdiag.write("\n", nl);

		const char *renderer = vkprops->deviceName;
		vkdiag.write(renderer, strlen(renderer)); vkdiag.write("\n", nl);

		std::stringstream driverversion;
		driverversion << vkprops->driverVersion;
		std::string version = driverversion.str();
		vkdiag.write(version.c_str(), version.length()); vkdiag.write("\n", nl);

		std::stringstream apiversion;
		apiversion << VK_VERSION_MAJOR(vkprops->apiVersion);
		apiversion << ".";
		apiversion << VK_VERSION_MINOR(vkprops->apiVersion);
		apiversion << ".";
		apiversion << VK_VERSION_PATCH(vkprops->apiVersion);
		std::string spirv = apiversion.str();
		vkdiag.write(spirv.c_str(), spirv.length()); vkdiag.write("\n", nl);
	
		vkdiag.write("114\n", 3 + nl);

		write_int("maxImageDimension1D ", vkprops->limits.maxImageDimension1D, vkdiag);
		write_int("maxImageDimension2D ", vkprops->limits.maxImageDimension2D, vkdiag);
		write_int("maxImageDimension3D ", vkprops->limits.maxImageDimension3D, vkdiag);
		write_int("maxImageDimensionCube ", vkprops->limits.maxImageDimensionCube, vkdiag);
		write_int("maxImageArrayLayers ", vkprops->limits.maxImageArrayLayers, vkdiag);
		write_int("maxTexelBufferElements ", vkprops->limits.maxTexelBufferElements, vkdiag);
		write_int("maxUniformBufferRange ", vkprops->limits.maxUniformBufferRange, vkdiag);
		write_int("maxStorageBufferRange ", vkprops->limits.maxStorageBufferRange, vkdiag);
		write_int("maxPushConstantsSize ", vkprops->limits.maxPushConstantsSize, vkdiag);
		write_int("maxMemoryAllocationCount ", vkprops->limits.maxMemoryAllocationCount, vkdiag);
		write_int("maxSamplerAllocationCount ", vkprops->limits.maxSamplerAllocationCount, vkdiag);	// 11
		write_lint("bufferImageGranularity ", vkprops->limits.bufferImageGranularity, vkdiag);
		write_lint("sparseAddressSpaceSize ", vkprops->limits.sparseAddressSpaceSize, vkdiag);
		write_int("maxBoundDescriptorSets ", vkprops->limits.maxBoundDescriptorSets, vkdiag);
		write_int("maxPerStageDescriptorSamplers ", vkprops->limits.maxPerStageDescriptorSamplers, vkdiag);
		write_int("maxPerStageDescriptorUniformBuffers ", vkprops->limits.maxPerStageDescriptorUniformBuffers, vkdiag);
		write_int("maxPerStageDescriptorStorageBuffers ", vkprops->limits.maxPerStageDescriptorStorageBuffers, vkdiag);
		write_int("maxPerStageDescriptorSampledImages ", vkprops->limits.maxPerStageDescriptorSampledImages, vkdiag);
		write_int("maxPerStageDescriptorStorageImages ", vkprops->limits.maxPerStageDescriptorStorageImages, vkdiag);
		write_int("maxPerStageDescriptorInputAttachments ", vkprops->limits.maxPerStageDescriptorInputAttachments, vkdiag);
		write_int("maxPerStageResources ", vkprops->limits.maxPerStageResources, vkdiag);
		write_int("maxDescriptorSetSamplers ", vkprops->limits.maxDescriptorSetSamplers, vkdiag);
		write_int("maxDescriptorSetUniformBuffers ", vkprops->limits.maxDescriptorSetUniformBuffers, vkdiag);
		write_int("maxDescriptorSetUniformBuffersDynamic ", vkprops->limits.maxDescriptorSetUniformBuffersDynamic, vkdiag);
		write_int("maxDescriptorSetStorageBuffers ", vkprops->limits.maxDescriptorSetStorageBuffers, vkdiag);
		write_int("maxDescriptorSetStorageBuffersDynamic ", vkprops->limits.maxDescriptorSetStorageBuffersDynamic, vkdiag);
		write_int("maxDescriptorSetSampledImages ", vkprops->limits.maxDescriptorSetSampledImages, vkdiag);
		write_int("maxDescriptorSetStorageImages ", vkprops->limits.maxDescriptorSetStorageImages, vkdiag);
		write_int("maxDescriptorSetInputAttachments ", vkprops->limits.maxDescriptorSetInputAttachments, vkdiag);
		write_int("maxVertexInputAttributes ", vkprops->limits.maxVertexInputAttributes, vkdiag);
		write_int("maxVertexInputBindings ", vkprops->limits.maxVertexInputBindings, vkdiag);
		write_int("maxVertexInputAttributeOffset ", vkprops->limits.maxVertexInputAttributeOffset, vkdiag);
		write_int("maxVertexInputBindingStride ", vkprops->limits.maxVertexInputBindingStride, vkdiag);
		write_int("maxVertexOutputComponents ", vkprops->limits.maxVertexOutputComponents, vkdiag);
		write_int("maxTessellationGenerationLevel ", vkprops->limits.maxTessellationGenerationLevel, vkdiag);
		write_int("maxTessellationPatchSize ", vkprops->limits.maxTessellationPatchSize, vkdiag);
		write_int("maxTessellationControlPerVertexInputComponents ", vkprops->limits.maxTessellationControlPerVertexInputComponents, vkdiag);
		write_int("maxTessellationControlPerVertexOutputComponents ", vkprops->limits.maxTessellationControlPerVertexOutputComponents, vkdiag); // 38
		write_int("maxTessellationControlPerPatchOutputComponents ", vkprops->limits.maxTessellationControlPerPatchOutputComponents, vkdiag);
		write_int("maxTessellationControlTotalOutputComponents ", vkprops->limits.maxTessellationControlTotalOutputComponents, vkdiag);
		write_int("maxTessellationEvaluationInputComponents ", vkprops->limits.maxTessellationEvaluationInputComponents, vkdiag);
		write_int("maxTessellationEvaluationOutputComponents ", vkprops->limits.maxTessellationEvaluationOutputComponents, vkdiag);
		write_int("maxGeometryShaderInvocations ", vkprops->limits.maxGeometryShaderInvocations, vkdiag);
		write_int("maxGeometryInputComponents ", vkprops->limits.maxGeometryInputComponents, vkdiag);
		write_int("maxGeometryOutputComponents ", vkprops->limits.maxGeometryOutputComponents, vkdiag);
		write_int("maxGeometryOutputVertices ", vkprops->limits.maxGeometryOutputVertices, vkdiag);
		write_int("maxGeometryTotalOutputComponents ", vkprops->limits.maxGeometryTotalOutputComponents, vkdiag);
		write_int("maxFragmentInputComponents ", vkprops->limits.maxFragmentInputComponents, vkdiag);
		write_int("maxFragmentOutputAttachments ", vkprops->limits.maxFragmentOutputAttachments, vkdiag);
		write_int("maxFragmentDualSrcAttachments ", vkprops->limits.maxFragmentDualSrcAttachments, vkdiag);
		write_int("maxFragmentCombinedOutputResources ", vkprops->limits.maxFragmentCombinedOutputResources, vkdiag);
		write_int("maxComputeSharedMemorySize ", vkprops->limits.maxComputeSharedMemorySize, vkdiag);
		write_int("maxComputeWorkGroupCount[0] ", vkprops->limits.maxComputeWorkGroupCount[0], vkdiag);
		write_int("maxComputeWorkGroupCount[1] ", vkprops->limits.maxComputeWorkGroupCount[1], vkdiag);
		write_int("maxComputeWorkGroupCount[2] ", vkprops->limits.maxComputeWorkGroupCount[2], vkdiag);
		write_int("maxComputeWorkGroupInvocations ", vkprops->limits.maxComputeWorkGroupInvocations, vkdiag);
		write_int("maxComputeWorkGroupSize[0] ", vkprops->limits.maxComputeWorkGroupSize[0], vkdiag);
		write_int("maxComputeWorkGroupSize[1] ", vkprops->limits.maxComputeWorkGroupSize[1], vkdiag);
		write_int("maxComputeWorkGroupSize[2] ", vkprops->limits.maxComputeWorkGroupSize[2], vkdiag);
		write_int("subPixelPrecisionBits ", vkprops->limits.subPixelPrecisionBits, vkdiag);
		write_int("subTexelPrecisionBits ", vkprops->limits.subTexelPrecisionBits, vkdiag);
		write_int("mipmapPrecisionBits ", vkprops->limits.mipmapPrecisionBits, vkdiag);
		write_int("maxDrawIndexedIndexValue ", vkprops->limits.maxDrawIndexedIndexValue, vkdiag);
		write_int("maxDrawIndirectCount ", vkprops->limits.maxDrawIndirectCount, vkdiag);	// 64
		write_float("maxSamplerLodBias ", vkprops->limits.maxSamplerLodBias, vkdiag);
		write_float("maxSamplerAnisotropy ", vkprops->limits.maxSamplerAnisotropy, vkdiag);
		write_int("maxViewports ", vkprops->limits.maxViewports, vkdiag);
		write_int("maxViewportDimensions[0] ", vkprops->limits.maxViewportDimensions[0], vkdiag);
		write_int("maxViewportDimensions[1] ", vkprops->limits.maxViewportDimensions[1], vkdiag);
		write_float("viewportBoundsRange[0] ", vkprops->limits.viewportBoundsRange[0], vkdiag);
		write_float("viewportBoundsRange[1] ", vkprops->limits.viewportBoundsRange[1], vkdiag);
		write_int("viewportSubPixelBits ", vkprops->limits.viewportSubPixelBits, vkdiag);
		write_lint("minMemoryMapAlignment ", vkprops->limits.minMemoryMapAlignment, vkdiag);
		write_lint("minTexelBufferOffsetAlignment ", vkprops->limits.minTexelBufferOffsetAlignment, vkdiag);
		write_lint("minUniformBufferOffsetAlignment ", vkprops->limits.minUniformBufferOffsetAlignment, vkdiag);
		write_lint("minStorageBufferOffsetAlignment ", vkprops->limits.minStorageBufferOffsetAlignment, vkdiag); // 76
		write_sint("minTexelOffset ", vkprops->limits.minTexelOffset, vkdiag);
		write_int("maxTexelOffset ", vkprops->limits.maxTexelOffset, vkdiag);
		write_sint("minTexelGatherOffset ", vkprops->limits.minTexelGatherOffset, vkdiag);
		write_int("maxTexelGatherOffset ", vkprops->limits.maxTexelGatherOffset, vkdiag);
		write_float("minInterpolationOffset ", vkprops->limits.minInterpolationOffset, vkdiag);
		write_float("maxInterpolationOffset ", vkprops->limits.maxInterpolationOffset, vkdiag);
		write_int("subPixelInterpolationOffsetBits ", vkprops->limits.subPixelInterpolationOffsetBits, vkdiag);
		write_int("maxFramebufferWidth ", vkprops->limits.maxFramebufferWidth, vkdiag);
		write_int("maxFramebufferHeight ", vkprops->limits.maxFramebufferHeight, vkdiag);
		write_int("maxFramebufferLayers ", vkprops->limits.maxFramebufferLayers, vkdiag); // 86
		write_sample("framebufferColorSampleCounts ", vkprops->limits.framebufferColorSampleCounts, vkdiag);
		write_sample("framebufferDepthSampleCounts ", vkprops->limits.framebufferDepthSampleCounts, vkdiag);
		write_sample("framebufferStencilSampleCounts ", vkprops->limits.framebufferStencilSampleCounts, vkdiag);
		write_sample("framebufferNoAttachmentsSampleCounts ", vkprops->limits.framebufferNoAttachmentsSampleCounts, vkdiag);
		write_int("maxColorAttachments ", vkprops->limits.maxColorAttachments, vkdiag);
		write_sample("sampledImageColorSampleCounts ", vkprops->limits.sampledImageColorSampleCounts, vkdiag);
		write_sample("sampledImageIntegerSampleCounts ", vkprops->limits.sampledImageIntegerSampleCounts, vkdiag);
		write_sample("sampledImageDepthSampleCounts ", vkprops->limits.sampledImageDepthSampleCounts, vkdiag);
		write_sample("sampledImageStencilSampleCounts ", vkprops->limits.sampledImageStencilSampleCounts, vkdiag);
		write_sample("storageImageSampleCounts ", vkprops->limits.storageImageSampleCounts, vkdiag);
		write_int("maxSampleMaskWords ", vkprops->limits.maxSampleMaskWords, vkdiag);
		write_int("timestampComputeAndGraphics ", vkprops->limits.timestampComputeAndGraphics, vkdiag);
		write_float("timestampPeriod ", vkprops->limits.timestampPeriod, vkdiag);
		write_int("maxClipDistances ", vkprops->limits.maxClipDistances, vkdiag);
		write_int("maxCullDistances ", vkprops->limits.maxCullDistances, vkdiag);
		write_int("maxCombinedClipAndCullDistances ", vkprops->limits.maxCombinedClipAndCullDistances, vkdiag);
		write_int("discreteQueuePriorities ", vkprops->limits.discreteQueuePriorities, vkdiag);
		write_float("pointSizeRange[0] ", vkprops->limits.pointSizeRange[0], vkdiag);
		write_float("pointSizeRange[1] ", vkprops->limits.pointSizeRange[1], vkdiag);
		write_float("lineWidthRange[0] ", vkprops->limits.lineWidthRange[0], vkdiag);
		write_float("lineWidthRange[1] ", vkprops->limits.lineWidthRange[1], vkdiag);
		write_float("pointSizeGranularity ", vkprops->limits.pointSizeGranularity, vkdiag);
		write_float("lineWidthGranularity ", vkprops->limits.lineWidthGranularity, vkdiag);
		write_int("strictLines ", vkprops->limits.strictLines, vkdiag);
		write_int("standardSampleLocations ", vkprops->limits.standardSampleLocations, vkdiag);
		write_lint("optimalBufferCopyOffsetAlignment ", vkprops->limits.optimalBufferCopyOffsetAlignment, vkdiag);
		write_lint("optimalBufferCopyRowPitchAlignment ", vkprops->limits.optimalBufferCopyRowPitchAlignment, vkdiag);
		write_lint("nonCoherentAtomSize ", vkprops->limits.nonCoherentAtomSize, vkdiag); // 114

		size_t p1 = 0;
		size_t p2 = exts.find(' ', p1);
		while (std::string::npos != p2)
		{
			std::string e = exts.substr(p1, p2 - p1);
			vkdiag.write(e.c_str(), e.length()); vkdiag.write("\n", nl);

			p1 = p2 + 1;
			p2 = exts.find(' ', p1);
		}

		std::string e = exts.substr(p1);
		vkdiag.write(e.c_str(), e.length()); vkdiag.write("\n", nl);

		vkdiag.close();
	}

	return true;
}
