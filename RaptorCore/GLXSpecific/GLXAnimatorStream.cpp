// GLXAnimatorStream.cpp: implementation of the CGLXAnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_GLXANIMATORSTREAM_H__401A89EB_3ABE_4CF2_8C2C_2358596FC741__INCLUDED_)
    #include "GLXAnimatorStream.h"
#endif

#include <sys/time.h>

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLXAnimatorStream::CGLXAnimatorStream()
{
    struct timezone tz;
    int res = gettimeofday(&oldtime,&tz);
}

CGLXAnimatorStream::~CGLXAnimatorStream()
{

}

float CGLXAnimatorStream::deltaTime(void)
{
    float dt = 0.0;

    struct timezone tz;
    struct timeval tv;
    int res = gettimeofday(&tv,&tz);

    if (res == 0)
    {
        dt = (tv.tv_sec - oldtime.tv_sec) + 
             0.000001 * (tv.tv_usec - oldtime.tv_usec);

        oldtime = tv;
    }

    return dt;
}




