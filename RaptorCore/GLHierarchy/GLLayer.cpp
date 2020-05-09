// GLLayer.cpp: implementation of the CGLLayer class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_GL2DFONT_H__FE8026E7_74FD_46FC_A70F_D6927E565F8D__INCLUDED_)
	#include "GLHierarchy/GL2DFont.h"
#endif
#if !defined(AFX_GLLAYER_H__12EA9DBD_DCDE_4C66_B607_DD32C023C8EF__INCLUDED_)
	#include "GLLayer.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#include "Subsys/FreeType/FTGlyphBitmap.h"
#include "Subsys/FreeType/TTBitmapFont.h"

RAPTOR_NAMESPACE

static CGLLayer::CGLLayerClassID layerId;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGLLayer::CGLLayer(int xpos,int ypos,unsigned int width,unsigned int height)
	:CPersistence(layerId,"LAYER"),
	m_bRebuild(true),m_bRedraw(false),
	m_pPlane(NULL),m_pBuffer(NULL),m_pBufferPointer(NULL)
{
	layer.handle(0);
	layer.hClass(COpenGL::COpenGLClassID::GetClassId().ID());

	m_xpos = xpos;
	m_ypos = ypos;
	m_layerHeight = height;
	m_layerWidth = width;
	m_depth = 0.0f;
	m_glTexCoordu = 0.0f;
	m_glTexCoordv = 0.0f;

	//	temporary buffer for texture loading
	CTextureFactory &f = CTextureFactory::getDefaultFactory();
	m_pPlane = f.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA,CTextureObject::CGL_OPAQUE);
	CImage plane;
	plane.allocatePixels(m_layerWidth, m_layerHeight);
	f.glLoadTexture(m_pPlane, plane);

	//	Final buffer for layers
	m_glTexCoordu = ((float)width)/m_pPlane->getWidth();
	m_glTexCoordv = ((float)height)/m_pPlane->getHeight();

	//	Allocate RGBA8 pixel format only. Other formats can be available
	//	depending on layer texture needs (floats ? require SSE2)
	//	32 bytes alignment for SSE4/AVX
	CHostMemoryManager::Allocator<unsigned char,32> allocator;
	//	allocate with padding for sse simplification
	m_pBuffer = allocator.allocate(m_layerWidth*m_layerHeight*4+16); 

	if (CTexelAllocator::GetInstance()->isMemoryRelocated())
	{
		CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
		m_pBufferPointer = pAllocator->allocateTexels(m_layerWidth*m_layerHeight*4);
	}

	CATCH_GL_ERROR
}

CGLLayer::~CGLLayer()
{
	if (m_pPlane != NULL)
		m_pPlane->releaseReference();

    unsigned int size = sprites.size();
    for (unsigned int i=0; i < size ; i++)
    {
        sprites[i].image->releaseReference();
    }
	if (CTexelAllocator::GetInstance()->isMemoryRelocated())
	{
		CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
		pAllocator->releaseTexels(m_pBufferPointer);
	}
}


const CTextureObject* CGLLayer::getLayerImage(void) const
{
    return m_pPlane; 
}

void CGLLayer::glRenderSingleBuffer(const CGLLayer *layer) const
{
	CTextureObject* singleBuffer = const_cast<CTextureObject*>(m_pPlane);
	singleBuffer->glvkRender();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(	(float)layer->m_xpos, (float)layer->m_ypos, layer->m_depth);

		glTexCoord2f(m_glTexCoordu,0.0f);
		glVertex3f(	(float)(layer->m_xpos+layer->m_layerWidth),
					      (float)layer->m_ypos,
					      layer->m_depth);
			
		glTexCoord2f(m_glTexCoordu,m_glTexCoordv);
		glVertex3f(	(float)(layer->m_xpos+layer->m_layerWidth),
					      (float)(layer->m_ypos+layer->m_layerHeight),
					      layer->m_depth);
		
		glTexCoord2f(0.0f,m_glTexCoordv);
		glVertex3f(	(float)layer->m_xpos,
					      (float)(layer->m_ypos+layer->m_layerHeight),
					      layer->m_depth);
	glEnd();

    CATCH_GL_ERROR
}

void CGLLayer::glMakeList()
{
    m_bRebuild = false;

	if (glIsList(layer.glname()))
		glDeleteLists(layer.glname(),1);

	layer.handle(glGenLists(1));

	glNewList(layer.glname(),GL_COMPILE);	

		glRenderSingleBuffer(this);

		//	linked layers, if any
		const CGLLayer *pLayer;
		for (unsigned int i=0;i<links.size();i++)
		{
			pLayer = links.at(i);
	
			glRenderSingleBuffer(pLayer);
		}

	glEndList();

	CATCH_GL_ERROR
}

//////////////////////////////////////////////////////////////////////
//	Linked rendering
//////////////////////////////////////////////////////////////////////
void CGLLayer::linkRendering(const CGLLayer *layer)
{
	links.push_back(layer);

    m_bRebuild = true;
}

//////////////////////////////////////////////////////////////////////
//	Sprites
//////////////////////////////////////////////////////////////////////
void CGLLayer::manageSprite(CTextureObject *spr, float posx, float posy, float angle)
{
    for (unsigned int i=0;i<sprites.size();i++)
    {
        SPRITE &s = sprites[i];
        if (s.image == spr)
        {
            s.posx = posx;
            s.posy = posy;
            s.angle = angle;
            return;
        }
    }

    SPRITE sprite;
    sprite.posx = posx;
    sprite.posy = posy;
    sprite.angle = angle;
    sprite.image = spr;

    spr->addReference();

    sprites.push_back(sprite);
}

//////////////////////////////////////////////////////////////////////
//	Operations
//////////////////////////////////////////////////////////////////////
void CGLLayer::setPlaneDepth(float depth)
{ 
	if ((depth>1.0f) || (depth<0.0f))
		m_depth = 0.0f;
	else
		m_depth = depth - 1.0f; 

    m_bRebuild = true;
}

void CGLLayer::clear(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	//	ABGR ordering to match memory byte ordering
	unsigned long color = ((unsigned long)(a<<24)) + ((unsigned long)(b<<16)) + ((unsigned long)(g<<8)) + ((unsigned long)(r));
	clear(color);
}

//	Assume buffer size is a multiple of 2 pixels !
void CGLLayer::clear(unsigned long color)
{
	unsigned int * buffer = (unsigned int * )m_pBuffer;
	for (int i=m_layerWidth*m_layerHeight-1;i>=0;i--)
		buffer[i] = color;

    m_bRedraw = true;
}

void CGLLayer::glRender()
{
    float viewport[4];
	glGetFloatv(GL_VIEWPORT,viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,viewport[2],0,viewport[3],0,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
    if (m_bRedraw)
    {
		m_pPlane->glvkRender();

		if (CTexelAllocator::GetInstance()->isMemoryRelocated() &&
			CTexelAllocator::GetInstance()->isMemoryLocked() &&
			(NULL != m_pBufferPointer))
		{
			CTexelAllocator::GetInstance()->glvkSetPointerData(	m_pBufferPointer, m_pBuffer,
																m_layerWidth*m_layerHeight*4);
			glTexSubImage2D(GL_TEXTURE_2D,
							m_pPlane->getCurrentMipMapLevel(),
							0, 0, m_layerWidth, m_layerHeight,
							GL_RGBA, GL_UNSIGNED_BYTE, m_pBufferPointer);
		}
		else
			glTexSubImage2D(GL_TEXTURE_2D,
							m_pPlane->getCurrentMipMapLevel(),
							0, 0, m_layerWidth, m_layerHeight,
							GL_RGBA, GL_UNSIGNED_BYTE, m_pBuffer);
        m_bRedraw = false;
    }

    if (m_bRebuild)
        glMakeList();
	glCallList(layer.glname());

	glEnable(GL_SCISSOR_TEST);
	glScissor(m_xpos,m_ypos,m_layerWidth,m_layerHeight);

	//	sprites
	glDisable(GL_DEPTH_TEST);
	for (unsigned int i=0;i<sprites.size();i++)
	{
        SPRITE spr = sprites[i];
		CTextureObject*	T = spr.image;
		glPushMatrix();
		glTranslatef((float)(m_xpos+spr.posx),(float)(m_ypos+spr.posy),0.0f);
		glRotatef(spr.angle,0.0f,0.0f,1.0f);

		T->glvkRender();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(-(float)(T->getWidth())/2,-(float)(T->getHeight())/2,m_depth);

			glTexCoord2f(1.0f,0.0f);
			glVertex3f((float)(T->getWidth())/2,-(float)(T->getHeight())/2,m_depth);
			
			glTexCoord2f(1.0f,1.0f);
			glVertex3f((float)(T->getWidth())/2,(float)(T->getHeight())/2,m_depth);
		
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(-(float)(T->getWidth())/2,(float)(T->getHeight())/2,m_depth);
		glEnd();

		glPopMatrix();
	}

	glPopAttrib();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	CATCH_GL_ERROR
}



//////////////////////////////////////////////////////////////////////
//	Drawings
//////////////////////////////////////////////////////////////////////

void CGLLayer::drawPoint(unsigned int x,unsigned int y,unsigned long color)
{
	((unsigned int * )m_pBuffer)[y*m_layerWidth+x] = color;
    m_bRedraw = true;
}

void CGLLayer::drawAPoint(unsigned int x,unsigned int y,unsigned long color)
{
	unsigned int offset = 4*(y*m_layerWidth+x);
	unsigned int a = ((color >> 16) & 0xff00) / 255;
	unsigned int a2 = 256 - a;
	unsigned int r = a * (color & 0xff);
	unsigned int g = a * ((color >> 8) & 0xff);
	unsigned int b = a * ((color >> 16) & 0xff);
	unsigned int r2 = a2 * m_pBuffer[offset];
	unsigned int g2 = a2 * m_pBuffer[1+offset];
	unsigned int b2 = a2 * m_pBuffer[2+offset];
	m_pBuffer[offset] = (r2 + r) / 256;
	m_pBuffer[1+offset] = (g2 + g) / 256;
	m_pBuffer[2+offset] = (b2 + b) / 256;
	// do not update alpha to have easy control of transparency.
	//m_pBuffer[3+offset] = a;

    m_bRedraw = true;
}

//	Bresenham algorithm for line drawing
void CGLLayer::drawLine(int x1,int y1,int x2,int y2,unsigned long color)
{
	int dinc1;
	int dinc2;
	int screeninc1;
	int screeninc2;
	int xinc1 = 0;
	int yinc1 = 0;
	int xinc2 = 1;
	int yinc2 = 1;
	int deltax = (short)(abs(x2-x1));
	int deltay = (short)(abs(y2-y1));
	
	short d;
	short numpixels = 1;

	if (deltax>=deltay)
	{
		dinc1 = 2 * deltay;
		dinc2 = 2 * deltay - 2 * deltax;
		xinc1 = 1;
		d = 2 * deltay - deltax;
		numpixels += deltax;
	}
	else
	{
		dinc1 = 2 * deltax;
		dinc2 = 2 * deltax - 2 * deltay;
		yinc1 = 1;
		d = 2 * deltax - deltay;
		numpixels += deltay;
	}

	if (x1>x2)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1>y2)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	screeninc1 = yinc1 * m_layerWidth + xinc1;
	screeninc2 = yinc2 * m_layerWidth + xinc2;

	unsigned int offset = y1 * m_layerWidth + x1;
	unsigned int * buffer = (unsigned int * )m_pBuffer;

	while (numpixels > 0)
	{
		buffer[offset] = color;
		if (d > 0)
		{
			d +=  dinc2;
			offset += screeninc2;
		}
		else
		{
			d += dinc1;
			offset += screeninc1;
		}
		numpixels --;
	}

	m_bRedraw = true;
}


//	Bresenham algorithm for line drawing
void CGLLayer::drawALine(int x1,int y1,int x2,int y2,unsigned long color)
{
	m_bRedraw = true;
	
	int xinc1 = 0;
	int yinc1 = 1;
	int xinc2 = 1;
	int yinc2 = 1;
	int deltax = (short)(abs(x2-x1));
	int deltay = (short)(abs(y2-y1));
	int dinc1 = 2 * deltax;
	int dinc2 = 2 * deltax - 2 * deltay;
	int d = 2 * deltax - deltay;
	int numpixels = deltay + 1;

	if (deltax>=deltay)
	{
		dinc1 = 2 * deltay;
		dinc2 = 2 * deltay - 2 * deltax;
		xinc1 = 1;
		yinc1 = 0;
		d = 2 * deltay - deltax;
		numpixels = deltax + 1;
	}

	if (x1>x2)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1>y2)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	int screeninc1 = 4 * (yinc1 * m_layerWidth + xinc1);
	int screeninc2 = 4 * (yinc2 * m_layerWidth + xinc2);
	unsigned int offset = 4 * (y1 * m_layerWidth + x1);

	unsigned int a = ((color >> 16) & 0xff00) / 255;
	unsigned int a2 = 256 - a;
	unsigned int r = a * (color & 0xff);
	unsigned int g = a * ((color >> 8) & 0xff);
	unsigned int b = a * ((color >> 16) & 0xff);

	while (numpixels > 0)
	{
		unsigned int r2 = a2 * m_pBuffer[offset];
		unsigned int g2 = a2 * m_pBuffer[1+offset];
		unsigned int b2 = a2 * m_pBuffer[2+offset];
		
		m_pBuffer[offset] = (r2 + r) / 256;
		m_pBuffer[1+offset] = (g2 + g) / 256;
		m_pBuffer[2+offset] = (b2 + b) / 256;
		// do not update alpha to have easy control of transparency.
		//m_pBuffer[3+offset] = a;

		if (d > 0)
		{
			d +=  dinc2;
			offset += screeninc2;
		}
		else
		{
			d += dinc1;
			offset += screeninc1;
		}
		numpixels --;
	}
}

void CGLLayer::drawRectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color)
{
	unsigned int * buffer = (unsigned int * )m_pBuffer;
	unsigned int offset = y0 * m_layerWidth;
	for (unsigned int j=y0;j<y1;j++)
	{
		for (unsigned int i=x0;i<x1;i++)
			buffer[offset+i] = color;
		offset += m_layerWidth;
	}

    m_bRedraw = true;
}

void CGLLayer::drawARectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color)
{
	unsigned int a = ((color >> 16) & 0xff00) / 255;
	unsigned int a2 = 256 - a;
	unsigned int r = a * (color & 0xff);
	unsigned int g = a * ((color >> 8) & 0xff);
	unsigned int b = a * ((color >> 16) & 0xff);

	for (unsigned int j=y0;j<y1;j++)
	{
		unsigned int offset = 4 * (x0 + j * m_layerWidth);
		for (unsigned int i=x0;i<x1;i++)
		{
			unsigned int r2 = a2 * m_pBuffer[offset];
			unsigned int g2 = a2 * m_pBuffer[1+offset];
			unsigned int b2 = a2 * m_pBuffer[2+offset];
			
			m_pBuffer[offset] = (r2 + r) / 256;
			m_pBuffer[1+offset] = (g2 + g) / 256;
			m_pBuffer[2+offset] = (b2 + b) / 256;
			// do not update alpha to have easy control of transparency.
			//m_pBuffer[3+offset] = a;
			offset+=4;
		}
	}

    m_bRedraw = true;
}

void CGLLayer::drawPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels)
{
	unsigned int * buffer = (unsigned int * )m_pBuffer;
	unsigned int * src_buffer = (unsigned int * )pixels;
	unsigned int offset = x0 + y0 * m_layerWidth;
	for (unsigned int j=0;j<height;j++)
	{
		for (unsigned int i=0;i<width;i++)
			buffer[offset+i] = src_buffer[i+j*width];
		offset += m_layerWidth;
	}

    m_bRedraw = true;
}

void CGLLayer::drawAPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels)
{
	unsigned int offset = 4 * (x0 + y0 * m_layerWidth);
	unsigned int o = 0;

	for (unsigned int j=0;j<height;j++)
	{
		for (unsigned int i=0;i<width;i++)
		{
			unsigned int a = (256*pixels[o+3])/255;
			unsigned int a2 = 256 - a;

			unsigned int r2 = a2 * m_pBuffer[offset];
			unsigned int r = a * pixels[o++];
			m_pBuffer[offset++] = (r2 + r) / 256;

			unsigned int g2 = a2 * m_pBuffer[offset];
			unsigned int g = a * pixels[o++];
			m_pBuffer[offset++] = (g2 + g) / 256;

			unsigned int b2 = a2 * m_pBuffer[offset];
			unsigned int b = a * pixels[o++];
			m_pBuffer[offset++] = (b2 + b) / 256;

			// do not update alpha to have easy control of transparency.
			//m_pBuffer[offset++] = a;
			offset++;
			o++;
		}
		offset += 4 * (m_layerWidth-width);
	}

    m_bRedraw = true;
}


void CGLLayer::drawText(int x0,int y0,
						const std::string& text,
						CGL2DFont *font,
						unsigned long color)
{
	const TTBitmapFont *fnt = font->getBitmapFont();
	int px = x0;
	unsigned int width;
	unsigned int height;

	for (unsigned int i=0;i<text.size();i++)
	{
		FTGlyphBitmap* gbitmap = fnt->getBitmap(text[i]);
		width = gbitmap->getWidth();

		if (width == 0)
		{
			px += gbitmap->getAdvance();
			continue;
		}
		
		int deltax = gbitmap->getDeltaX();
		int deltay = gbitmap->getDeltaY();
		height = gbitmap->getHeight();
		const GLubyte* bmp = (const GLubyte*)(gbitmap->getBitmap());

		unsigned int offset = (px + deltax) + (y0 + deltay) * m_layerWidth;
		unsigned int *buffer = (unsigned int *)m_pBuffer;
		for (unsigned int k=0;k<height;k++)
		{
			unsigned int bpos = k * gbitmap->getRowSize();
			unsigned char bitmask = 128;
			for (unsigned int j=0;j<width;j++)
			{
				if (bitmask == 0)
				{
					bpos++;
					bitmask = 128;
				}
				if (0 != (bmp[bpos] & bitmask))
					buffer[offset] = color;
				offset++;
				bitmask /= 2;
			}
			offset += (m_layerWidth - width);
		}

		px += gbitmap->getAdvance();
	}

    m_bRedraw = true;
}


void CGLLayer::drawAText(int x0,int y0,
						 const std::string& text,
						 CGL2DFont *font,
						 unsigned long color)
{
	const TTBitmapFont *fnt = font->getBitmapFont();
	int px = x0;
	unsigned int width;
	unsigned int height;

	unsigned int a = ((color >> 16) & 0xff00) / 255;
	unsigned int a2 = 256 - a;
	unsigned int r = a * (color & 0xff);
	unsigned int g = a * ((color >> 8) & 0xff);
	unsigned int b = a * ((color >> 16) & 0xff);

	for (unsigned int i=0;i<text.size();i++)
	{
		FTGlyphBitmap* gbitmap = fnt->getBitmap(text[i]);
		width = gbitmap->getWidth();

		if (width == 0)
		{
			px += gbitmap->getAdvance();
			continue;
		}

		int deltax = gbitmap->getDeltaX();
		int deltay = gbitmap->getDeltaY();
		height = gbitmap->getHeight();
		const GLubyte* bmp = (const GLubyte*)(gbitmap->getBitmap());

		unsigned int offset = 4 * ((px + deltax) + (y0 + deltay) * m_layerWidth);
		for (unsigned int k=0;k<height;k++)
		{
			unsigned char bitmask = 128;
			unsigned int bpos = k * gbitmap->getRowSize();
			for (unsigned int j=0;j<width;j++)
			{
				if (bitmask == 0)
				{
					bpos++;
					bitmask = 128;
				}
				if (0 != (bmp[bpos] & bitmask))
				{
					unsigned int r2 = a2 * m_pBuffer[offset];
					m_pBuffer[offset] = (r2 + r) >> 8;

					unsigned int g2 = a2 * m_pBuffer[1+offset];
					m_pBuffer[1+offset] = (g2 + g) >> 8;

					unsigned int b2 = a2 * m_pBuffer[2+offset];
					m_pBuffer[2+offset] = (b2 + b) >> 8;
				}
				offset += 4;
				bitmask /= 2;
			}
			bpos++;
			offset += 4 * (m_layerWidth - width);
		}

		px += gbitmap->getAdvance();
	}

    m_bRedraw = true;
}


bool CGLLayer::exportObject(CRaptorIO& o)
{
	CPersistence::exportObject(o);

	o << m_depth;
	o << m_layerHeight;
	o << m_layerWidth;
	o << m_xpos;
	o << m_ypos;

	o << m_glTexCoordu;
	o << m_glTexCoordv;

	return true;
}

bool CGLLayer::importObject(CRaptorIO& i)
{
	CPersistence::importObject(i);

	i >> m_depth;
	i >> m_layerHeight;
	i >> m_layerWidth;
	i >> m_xpos;
	i >> m_ypos;

	i >> m_glTexCoordu;
	i >> m_glTexCoordv;

	return true;
}

