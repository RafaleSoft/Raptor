// Projector.cpp: implementation of the CProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PROJECTOR_H__0AEE2092_215F_40FA_BBAE_7D8A2F5A482F__INCLUDED_)
    #include "Projector.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "Shader.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "TextureUnitSetup.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "TextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_3DENGINEMATRIX_H__6CD1110E_1174_4f38_A452_30FB312022D0__INCLUDED_)
	#include "Engine/3DEngineMatrix.h"
#endif

RAPTOR_NAMESPACE_BEGIN

static CProjector::CProjectorClassID projectorId;
const CPersistence::CPersistenceClassID& CProjector::CProjectorClassID::GetClassId(void)
{
	return projectorId;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjector::CProjector(const std::string& name):
    CPersistence(projectorId,name),
	m_cutoff(0.0f),m_unitDistance(1.0f)
{
}

CProjector::~CProjector()
{
}

void CProjector::glSetProjection(CTextureObject* T)
{
	m_pProjection = T;

    CATCH_GL_ERROR
}

void CProjector::setCutOff(float cutoff)
{ 
	float cut = MIN(MAX(0.0f,cutoff),180.0f);
	if (cut < 180.0f)
		m_cutoff = (tan(TO_RADIAN(0.5f * cut)));
	else
		m_cutoff = 0.0f;
}

void CProjector::setUnitDistance(float d)
{
	if (d != 0)
		m_unitDistance = 1.0f / fabs(d);
	else
		m_unitDistance = 1.0;
}

void CProjector::glRender(void)
{
    if (m_pProjection == NULL)
        return;
    
	CGenericVector<float> P = m_position;
	C3DEngineMatrix M = C3DEngine::Get3DEngine()->getModelview();
	P *= M;

	CGenericVector<float> S(m_Sprojection);
	CGenericVector<float> T(m_Tprojection);
	CGenericVector<float> Q(m_Qprojection);
	M.Inverse();
	S *= M;
	T *= M;
	Q *= M;
	m_Sprojection.h = -S.X()*P.X() - S.Y()*P.Y() - S.Z()*P.Z();
	m_Tprojection.h = -T.X()*P.X() - T.Y()*P.Y() - T.Z()*P.Z();

	Q.H() = -Q.X()*P.X() - Q.Y()*P.Y() - Q.Z()*P.Z();
	if (m_cutoff > 0)
		Q *= 2 * m_cutoff;
	else
		Q *= 2 * m_unitDistance;
    
#if defined(GL_ARB_multitexture)
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
#endif

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	glRenderCone(30.0f);
#endif

	m_pProjection->glvkRender();
	//m_pProjection->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);

	glTexGenfv(GL_S,GL_EYE_PLANE,m_Sprojection);
	glTexGenfv(GL_T,GL_EYE_PLANE,m_Tprojection);
	glTexGenfv(GL_Q,GL_EYE_PLANE,Q.vector());

#if defined(GL_ARB_multitexture)
    pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
}

void CProjector::glActivate(bool activate)
{
#if defined(GL_ARB_multitexture)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
#endif

    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE,&mode);

    if (activate)
    {
        glEnable(GL_TEXTURE_2D);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glTranslatef(0.5f,0.5f,0.0f);
		glScalef(1.0f,-1.0f,1.0f);

        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_Q);
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
        glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    }
    else
    {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_Q);
        
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glDisable(GL_TEXTURE_2D);
    }

    glMatrixMode(mode);

#if defined(GL_ARB_multitexture)
    pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
}

void CProjector::setPosition(const CGenericVector<float>& position) 
{ 
	m_position = position;

	solve();
}

void CProjector::setDirection(const CGenericVector<float>& direction) 
{ 
	m_direction = direction;
    m_direction.Normalize();
	
	solve();
}

void CProjector::setProjector(	const CGenericVector<float>& position,
								const CGenericVector<float>& direction) 
{
	m_position = position;
	m_direction = direction;
    m_direction.Normalize();
	
	solve();
}

void CProjector::solve()
{
	GL_COORD_VERTEX &L = m_Qprojection;
	GL_COORD_VERTEX &L1 = m_Sprojection;
	GL_COORD_VERTEX &L2 = m_Tprojection;

	L.x = m_direction.X();
    L.y = m_direction.Y();
    L.z = m_direction.Z();
    L.h = 1.0f;

	C3DEngine::Get3DEngine()->normals(L, L1, L2);
}

// This method is for debugging purpose only.
void CProjector::glRenderCone(float d)
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor4f(1.0f,0.0f,1.0f,1.0f);

	CGenericVector<float>	target = m_position + (m_direction * d);
	CGenericVector<float>	s(m_Sprojection);
	CGenericVector<float>	t(m_Tprojection);
	CGenericVector<float>	p;

#if defined(GL_ARB_multitexture)
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
    pExtensions->glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
#endif

	float cut = d;
	if (m_cutoff > 0)
		cut *= m_cutoff;
	else
		cut *= m_unitDistance;

	glBegin(GL_TRIANGLE_FAN);
		glVertex3fv(m_position.vector());
		p = target - s*cut - t*cut;
		glVertex3fv(p.vector());
		p = target - s*cut + t*cut;
		glVertex3fv(p.vector());
		p = target + s*cut + t*cut;
		glVertex3fv(p.vector());
		p = target + s*cut - t*cut;
		glVertex3fv(p.vector());
	glEnd();

	glBegin(GL_LINES);
		glColor4f(1.0f,0.0f,0.0f,1.0f);
		glVertex3fv(m_position.vector());
		p = m_position + s*5.0f;
		glVertex3fv(p.vector());

		glColor4f(0.0f,1.0f,0.0f,1.0f);
		glVertex3fv(m_position.vector());
		p = m_position + t*5.0f;
		glVertex3fv(p.vector());

		glColor4f(0.0f,0.0f,1.0f,1.0f);
		glVertex3fv(m_position.vector());
		p = m_position + m_direction*d;
		glVertex3fv(p.vector());
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPopAttrib();
}

