// GLXView.cpp: implementation of the CGLXView class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLXVIEW_H__4B65A453_8F4F_4F97_835F_23EE19B5657E__INCLUDED_)
    #include "GLXView.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
    #include "System/Global.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLXView::CGLXView():m_pDisplay(NULL)
{

}

CGLXView::~CGLXView()
{
}

CGLXView::operator RAPTOR_HANDLE() const
{
    RAPTOR_HANDLE res;

    res.handle = (unsigned int)this;
    res.hClass = WINDOW_CLASS;

    return res;
}

void CGLXView::OnSize(int sx, int sy)
{
    if (m_pDisplay == NULL)
        return;

    RAPTOR_HANDLE device = m_pDisplay->getCurrentDevice();

    m_pDisplay->glBindDisplay(device);

    m_pDisplay->glResize(sx,sy,0,0);

    CATCH_GL_ERROR

    m_pDisplay->glUnBindDisplay();
}

void CGLXView::glRender()
{
    //  Temporary view or not yet created
    if (m_pDisplay == NULL)
        return;

    RAPTOR_HANDLE device = m_pDisplay->getCurrentDevice();

    m_pDisplay->glBindDisplay(device);

    m_pDisplay->glRender();

    m_pDisplay->glUnBindDisplay();
}

void CGLXView::GLInitContext(void)
{
}

bool CGLXView::GLCreateWindow ( const string &name, 
                                const CRaptorDisplayConfig& glCreateStruct)
{
    if (m_pDisplay != NULL)
        return false;

    //  The context returns a view, get attributes and discard it
    RAPTOR_HANDLE window = Raptor::glCreateWindow(glCreateStruct,m_pDisplay); 
    CGLXView *view = (CGLXView*)(window.handle);
    m_wnd = view->m_wnd; 
    delete view;

    if (m_pDisplay != NULL)
    {
        RAPTOR_HANDLE device = m_pDisplay->getCurrentDevice();
        m_pDisplay->glBindDisplay(device);

        GLInitContext();

        CATCH_GL_ERROR

        m_pDisplay->glUnBindDisplay();

        return true;
    }
    else
        return false;
}

