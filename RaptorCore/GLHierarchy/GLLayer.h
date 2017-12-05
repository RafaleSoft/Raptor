// GLLayer.h: interface for the CGLLayer class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GLLAYER_H__12EA9DBD_DCDE_4C66_B607_DD32C023C8EF__INCLUDED_)
#define AFX_GLLAYER_H__12EA9DBD_DCDE_4C66_B607_DD32C023C8EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CGL2DFont;
class CImageModifier;
class CTextureObject;


class RAPTOR_API CGLLayer : public CPersistence  
{
public:
	CGLLayer(int xpos,int ypos,unsigned int width,unsigned int height);
	virtual ~CGLLayer();

	//	Accessors
	int getXPos(void) const { return m_xpos; };
	int getYPos(void) const { return m_ypos; };
	int getWidth(void) const { return m_layerWidth; };
	int getHeight(void) const { return m_layerHeight; };

	float getDepth(void) const { return m_depth; };
	float getUTexCoord(void) const { return m_glTexCoordu;};
	float getVTexCoord(void) const { return m_glTexCoordv;};

	const CTextureObject* getLayerImage(void) const;


	//!	Fast clear routines:
    //! Clear the whole layer with r,g,b & a value
	void clear(unsigned char r, unsigned char g,unsigned char b,unsigned char a);
    //! Same as above but with color packed into a 32bit parameter.
	virtual void clear(unsigned long color);

	//	ReloadBuffer must be called before rendering
	//	to actually display all graphic operations
	//	such as DrawPoint,DrawText...
	virtual void glRender();

	//!	Linked rendering : allow the rendering of multiple layers
	//!	linked to a single one, which avoid state change management
	//!	With this method, glRender() should be faster for rendering 
	//!	several layers because it is called only once
	void linkRendering(const CGLLayer *layer);

    //! This method removes all layer links
	void clearLinks(void) {	links.clear(); }

	//	The depth of the layer is defined in the range [0.0f..1.0f]
	//	0.0f beeing the farthest value, 1.0f the nearest
	void setPlaneDepth( float depth);

	//	standard routines
	void drawPoint(unsigned int x,unsigned int y,unsigned long color);
	void drawLine(int x0,int y0,int x1,int y1,unsigned long color);
	virtual void drawRectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color);
	void drawPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels);
	void drawText(int x0,int y0,const std::string& text,CGL2DFont *font,unsigned long color);

	//	alpha blending extended routines:
	// blending equation is src_color * src_alpha + dst_color * (1 - src_alpha)
	// alpha values are not updated in destination buffer to enable fine control
	// of alpha values with standard routines above.
	//	( very fast with MMX2 :), activate in SSE code generation )
	//	( if not SSE code generation, standard routines are used instead )
	virtual void drawAPoint(unsigned int x,unsigned int y,unsigned long color);
	virtual void drawALine(int x0,int y0,int x1,int y1,unsigned long color);
	virtual void drawARectangle(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned long color);
	virtual void drawAPixels(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned char *pixels);
	virtual void drawAText(int x0,int y0,const std::string& text,CGL2DFont *font,unsigned long color);

	//!	Sprites management : creates a new sprite or update an existing one
	//!	@param spr : must have been loaded in its CTextureFactory Repository.
	//! @param posx : initial x position in layer
    //! @param posy : initial y position in layer
    //! @param angle : initial rotation angle of image
	void manageSprite(CTextureObject *spr, float posx, float posy, float angle);

	//!	Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CGLLayerClassID,"Layer",CPersistence)


protected:
	void glRenderSingleBuffer(const CGLLayer *layer) const;

	unsigned char *getBuffer(void) const
	{ return m_pBuffer; };

	float	m_depth;
	int		m_layerHeight;
	int		m_layerWidth;
	int		m_xpos;
	int		m_ypos;
	bool    m_bRedraw;

	float	m_glTexCoordu;
	float	m_glTexCoordv;

	RAPTOR_HANDLE	layer;

private:
    void glMakeList();

    bool    m_bRebuild;

    typedef struct SPRITE_t
    {
        float   posx;
        float   posy;
        float	angle;
        CTextureObject	*image;
    } SPRITE;

    vector<SPRITE>  sprites;

	vector<const CGLLayer*>	links;

	CTextureObject	*m_pPlane;
	unsigned char	*m_pBuffer;
	unsigned char	*m_pBufferPointer;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLLAYER_H__12EA9DBD_DCDE_4C66_B607_DD32C023C8EF__INCLUDED_)

