// GLXAnimatorStream.h: interface for the CGLXAnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLXANIMATORSTREAM_H__401A89EB_3ABE_4CF2_8C2C_2358596FC741__INCLUDED_)
#define AFX_GLXANIMATORSTREAM_H__401A89EB_3ABE_4CF2_8C2C_2358596FC741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
    #include "Subsys/AnimatorStream.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CGLXAnimatorStream : public CAnimatorStream  
{
public:
	CGLXAnimatorStream();
	virtual ~CGLXAnimatorStream();

    virtual float deltaTime(void);

private:
    timeval     oldtime;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLXANIMATORSTREAM_H__401A89EB_3ABE_4CF2_8C2C_2358596FC741__INCLUDED_)
