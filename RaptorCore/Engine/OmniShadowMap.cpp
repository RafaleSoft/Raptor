// OmniShadowMap.cpp: implementation of the COmniShadowMap class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OMNISHADOWMAP_H__FB391794_C7C1_404B_A146_061A62252C5D__INCLUDED_)
    #include "OmniShadowMap.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLViewPoint.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_OLD_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexProgram_old.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COmniShadowMap::COmniShadowMap(C3DScene& rScene)
	:CEnvironment(rScene), m_pShadowTexture(NULL)
{
}

COmniShadowMap::~COmniShadowMap()
{
	unLinkEnvironment();
}

void COmniShadowMap::unLink(const CPersistence *object)
{
    if (object == static_cast<CPersistence*>(m_pShadowCubeMap))
        m_pShadowCubeMap = NULL;
    else if (object == static_cast<CPersistence*>(m_pViewPoint))
        m_pViewPoint = NULL;
	else
		CEnvironment::unLink(object);
}

void COmniShadowMap::unLinkEnvironment(void)
{
    if (m_pShadowCubeMap != NULL)
    {
		Raptor::glDestroyDisplay(m_pShadowCubeMap);
        m_pShadowCubeMap = NULL; //redundant: done at unlink
    }

	if (m_pViewPoint != NULL)
    {
		delete m_pViewPoint;
        m_pViewPoint = NULL;  //redundant: done at unlink
    }

	if (m_pShadowTexture != NULL)
    {
		m_pShadowTexture->releaseReference();
        m_pShadowTexture = NULL;
    }

	CATCH_GL_ERROR
}

void COmniShadowMap::addObject(C3DSceneObject* object)
{
}

bool COmniShadowMap::glInitEnvironment(unsigned int width,unsigned int height)
{
#ifdef GL_ARB_texture_cube_map
	if (!Raptor::glIsExtensionSupported(GL_ARB_TEXTURE_CUBE_MAP_EXTENSION_NAME) ||
		!Raptor::glIsExtensionSupported(WGL_NV_RENDER_DEPTH_TEXTURE_EXTENSION_NAME) ||
		!(Raptor::glIsExtensionSupported(GL_ARB_COLOR_BUFFER_FLOAT_EXTENSION_NAME) || 
		Raptor::glIsExtensionSupported(WGL_ATI_PIXEL_FORMAT_FLOAT_EXTENSION_NAME)))
    {
		Raptor::GetErrorManager()->generateRaptorError(	COpenGL::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Missing hardware texture capabilities to render omni-directional Shadow Maps");
        return false;
    }

    if (width != height)
        return false;

    // TODO: Should prevent non power of 2 texture sizes.

	m_pViewPoint = new COpenGLViewPoint();
	m_pViewPoint->setViewVolume(	-1.0,1.0,-1.0,1.0,1.0,100.0,IViewPoint::PERSPECTIVE);
	m_pViewPoint->registerDestruction(m_pObserver);

    //  Square coordinates are mandatory
	CRaptorDisplayConfig cs;
	cs.width = width;
	cs.height = height;
    cs.display_mode = CGL_RGBA |  CGL_DEPTH | CGL_RENDER_CUBETEXTURE;
	cs.acceleration = CRaptorDisplayConfig::HARDWARE;

	m_pShadowCubeMap = Raptor::glCreateDisplay(cs);
    if (NULL == m_pShadowCubeMap)
		return false;
	m_pShadowCubeMap->registerDestruction(m_pObserver);

	CATCH_GL_ERROR


	//
	// prepare light's point of view transform
	//
    m_lightProjection.Ident();

	RAPTOR_HANDLE display;
	m_pShadowCubeMap->glvkBindDisplay(display);
	m_pShadowCubeMap->setViewPoint(NULL);
    m_pViewPoint->glvkRenderViewPointModel();
	m_pViewPoint->glvkRender();
	m_pShadowCubeMap->glvkUnBindDisplay();

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
    m_pShadowTexture = factory.glCreateCubemap(ITextureObject::CGL_COLOR24_ALPHA,
                                               CTextureObject::CGL_MULTIPLY,
                                               ITextureObject::CGL_UNFILTERED);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB,GL_TEXTURE_WRAP_T,GL_CLAMP);

    string vtxShader = 
"!!ARBvp1.0 \
ATTRIB iPos = vertex.position; \
PARAM mvp[4] = { state.matrix.mvp }; \
TEMP vpos; \
OUTPUT oPos = result.position; \
OUTPUT oTex0 = result.texcoord[0]; \
DP4 oPos.x , mvp[0] , iPos; \
DP4 oPos.y , mvp[1] , iPos; \
DP4 oPos.z , mvp[2] , iPos; \
DP4 oPos.w, mvp[3] , iPos; \
PARAM mv[4] = { state.matrix.modelview }; \
DP4 vpos.x , mv[0] , iPos; \
DP4 vpos.y , mv[1] , iPos; \
DP4 vpos.z , mv[2] , iPos; \
DP4 vpos.w, mv[3], iPos; \
MOV oTex0, vpos; \
END" ;

	m_pVSShadowMap = new CVertexProgram_old();
    m_pVSShadowMap->glLoadProgram(vtxShader);

    string texShader = 
"!!ARBfp1.0 \
ATTRIB iTex0 = fragment.texcoord[0]; \
PARAM one = { 256.0, 65536.0, 1.0, 1.0 };\
OUTPUT finalColor = result.color; \
TEMP depth; \
TEMP compact; \
DP3 depth.x, iTex0, iTex0; \
RCP finalColor, depth.x; \
MOV finalColor.w, one.w; \
END" ;

/*    
    string texShader = 
"!!ARBfp1.0 \
ATTRIB iTex0 = fragment.texcoord[0]; \
OUTPUT finalDepth = result.depth; \
TEMP depth; \
DP3 depth.x, iTex0, iTex0; \
RCP finalDepth.z, depth.x; \
END" ;
*/
    m_pFSShadowMap = new CFragmentShader();
    m_pFSShadowMap->glLoadProgram(texShader);

	return true;
#else
    return false;
#endif
}

void COmniShadowMap::glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
    if (!isEnabled())
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(objects);
        return;
    }

    glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	//	Create the shadow
    vector<C3DSceneObject*> casters;
    vector<C3DSceneObject*> receivers;
    vector<C3DSceneObject*> others;
    vector<C3DSceneObject*>::const_iterator itr = objects.begin();
    while (itr != objects.end())
    {
        C3DSceneObject* const h = *itr++;
		CObject3D* obj = h->getObject();

        bool cast = obj->getProperties().isCastShadow();
        bool receive = obj->getProperties().isReceiveShadow();
        if (cast)
            casters.push_back(h);
        if (receive)
            receivers.push_back(h);
        if (!receive && !cast)
            others.push_back(h);
    }
    glRenderMap(currentLight,casters);

	// Normal rendering with shadow
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

    glRenderShadow(receivers);

    // Render 'other' objects
    if (others.size() > 0)
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(others);
    }
}


void COmniShadowMap::glRenderMap(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
#ifdef GL_ARB_texture_cube_map
    GL_COORD_VERTEX coord =  currentLight->getLightPosition();

    RAPTOR_HANDLE display;
	m_pShadowCubeMap->glvkBindDisplay(display);
    //glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	unsigned int cubefaces[6] = {	CTextureObject::CGL_CUBEMAP_NZ,
									CTextureObject::CGL_CUBEMAP_PX,
									CTextureObject::CGL_CUBEMAP_PZ,
									CTextureObject::CGL_CUBEMAP_NX,
									CTextureObject::CGL_CUBEMAP_PY,
									CTextureObject::CGL_CUBEMAP_NY };

    float rotates[6][4] = { { 180.0f, 0.0f, 0.0f, 1.0f }, 
							{ 180.0f, 0.0f, 0.0f, 1.0f },
							{ 180.0f, 0.0f, 0.0f, 1.0f },
							{ 180.0f, 0.0f, 0.0f, 1.0f },
							{ -90.0f, 1.0f, 0.0f, 0.0f },
							{ 90.0f, 1.0f, 0.0f, 0.0f } };
    float rotates2[6][4] = {{ 0.0f, 0.0f, 0.0f, 0.0f },
							{ 90.0f, 0.0f, 1.0f, 0.0f },
							{ 180.0f, 0.0f, 1.0f, 0.0f },
							{ -90.0f, 0.0f, 1.0f, 0.0f },
							{ 0.0f, 0.0f, 0.0f, 0.0f },
							{ 0.0f, 0.0f, 0.0f, 0.0f } };

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
	
     for (unsigned int i=0 ; i<6 ; i++)
    {
        RAPTOR_HANDLE _display(cubefaces[i],(void*)0);
	    m_pShadowCubeMap->glvkBindDisplay(_display);

        glLoadIdentity();
        glRotatef(rotates[i][0],rotates[i][1],rotates[i][2],rotates[i][3]);
        if (rotates2[i][0] != 0)
            glRotatef(rotates2[i][0],rotates2[i][1],rotates2[i][2],rotates2[i][3]);
        glTranslatef(-coord.x,-coord.y,-coord.z);

	    glPushMatrix();

        //glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        m_pVSShadowMap->glRender();
        m_pFSShadowMap->glRender();

        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(objects);

	    glPopMatrix();
    }

   //m_pVSShadowMap->Stop();
   //m_pFSShadowMap->Stop();

	 m_pShadowCubeMap->glvkUnBindDisplay();
#endif
}


void COmniShadowMap::glRenderShadow(const vector<C3DSceneObject*>& objects)
{
#ifdef GL_ARB_texture_cube_map
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	GLint previousTMU = GL_TEXTURE0_ARB;
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&previousTMU);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);

	if (m_pShadowTexture != NULL)
	{
		m_pShadowTexture->glvkRender();

		RAPTOR_HANDLE renderTexture(CTextureFactory::CTextureFactoryClassID::GetClassId().ID(), (void*)0);
		m_pShadowCubeMap->glvkBindDisplay(renderTexture);
	}

    glActiveTextureARB(previousTMU);

    C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
	glRenderObjects(objects);
    
	glActiveTextureARB(GL_TEXTURE1_ARB);

	if (m_pShadowTexture != NULL)
		m_pShadowCubeMap->glvkUnBindDisplay();
    
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glActiveTextureARB(previousTMU);
    
#endif
}

void COmniShadowMap::glRenderTexture(void)
{
#ifdef GL_ARB_texture_cube_map
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	m_pShadowTexture->glvkRender();

	RAPTOR_HANDLE renderTexture(CTextureFactory::CTextureFactoryClassID::GetClassId().ID(), (void*)0);
	m_pShadowCubeMap->glvkBindDisplay(renderTexture);
#endif
}

