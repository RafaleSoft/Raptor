// GenericRenderObject.h: interface for the CGenericRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICRENDEROBJECT_H__CC388141_715F_4FB4_9858_015088E7A9A8__INCLUDED_)
#define AFX_GENERICRENDEROBJECT_H__CC388141_715F_4FB4_9858_015088E7A9A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Raytracer.h"
#include "..\messages.h"

class CTexture;
class CGenericRay;
class CSurfacer;

class CGenericRenderObject  
{
public:
	typedef enum 
	{
		UNKNOWN,
		PLANE,
		TRIANGLE,
		QUADRIC,
		MESH
	} OBJECTTYPE;

	typedef enum
	{
		FLAT,
		SMOOTH,
		BUMP_MAP,
		BUMP_USER
	} SURFACETYPE;

	//	Rendering parameters
	rays_shading_t	shading;
	CColor::RGBA	color;

public:
	CGenericRenderObject(const std::string& name);
	virtual ~CGenericRenderObject();

	virtual OBJECTTYPE GetType() const = 0;
	const std::string& GetName() const { return m_name; }; 

	virtual void Scale(float sx,float sy,float sz) = 0;
	virtual void Translate(float tx,float ty,float tz) = 0;
	virtual void Rotate(float angle,float ax,float ay,float az) = 0;

	bool InitPlugins(const CRaytracerData& world);

	void SetTexture(CTexture *t) { m_pTexture = t; };
	CTexture * GetTexture(void) { return m_pTexture; };

	void SetNormalMap(CTexture *t) { m_pNormalMap = t; if (t!= NULL) surface = BUMP_MAP; };
	CSurfacer* GetSurfacer(SURFACETYPE type);

	virtual float Intersect( CGenericRay &ray ) = 0 ;
	virtual float FastIntersect( CGenericRay &ray ) = 0 ;

	virtual CColor::RGBA& GetLocalColor(const CGenericRay &ray) = 0;
	virtual CGenericVector<float>& GetTexel(const CGenericVector<float> &hit ) = 0;
	virtual CGenericVector<float>& GetNormal( const CGenericVector<float> &hit );

	virtual void ResetLastHit(void) {};

protected:
	//	Texture mapping
	CTexture				*m_pTexture;
	CTexture				*m_pNormalMap;

	//	Surface generator
	SURFACETYPE				surface;
	CSurfacer				*m_pSurfacer;

	virtual CGenericVector<float>& Normal( const CGenericVector<float> &hit ) = 0 ;
	virtual CGenericVector<float>& GetLocalNormal( const CGenericVector<float> &hit ) = 0;

private:
	CGenericVector<float>	tmpVect;
	std::string				m_name;
};

#endif // !defined(AFX_GENERICRENDEROBJECT_H__CC388141_715F_4FB4_9858_015088E7A9A8__INCLUDED_)
