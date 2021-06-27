// AnimatorStream.h: interface for the CAnimatorStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
#define AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
    #include "Engine/Animator.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CEngineJob;


//
//
//
//	TODO:
//	- Mettre Record frame dans un thread !!!
//	- Créer un stream compressé
//
class CAnimatorStream  
{
public:
	//! Default constructor.
	CAnimatorStream();

	//!	Destructor.
	virtual ~CAnimatorStream();

    //! Sets a streamer.
    void setVideoKindIO(CAnimator::IVideoIO *streamer);

    //! Returns a streamer for the specified extention, if it exists.
    CAnimator::IVideoIO* const getVideoKindIO(const std::string& extension) const;


    //! Returns the number of opened streams in this streamer.
    size_t getNbStream(void) const { return m_pCurrentStreams.size(); };

	//!	This method returns a stream buffer produced from playFrame ( of stream number nStream )
    virtual const unsigned char* const glGetStreamBuffer(unsigned int nStream);

	//!	This method releases the stream buffer aquired by glGetStreamBuffer
    virtual void glReleaseStreamBuffer(unsigned int nStream);

    virtual bool getFrameSizes(unsigned int stream, unsigned int &width, unsigned int &height) const;
    virtual unsigned int getFramePosition(unsigned int stream) const;

	virtual long glStartPlayBack(const std::string& fname,bool loop = false);
	virtual void endPlayBack(unsigned int nStream);

	//!
	//! Renders all animator stream current frame.
	//! @param dt: time increment from previous frame, in seconds.
	//!
    virtual void playFrames(float dt);
	
	//!
	//! Renders a single animator stream current frame.
	//! @param dt: time increment from previous frame, in seconds.
	//! @param nStream: stream number to render from
	//! @param framePos: frameNumber to render
	//!
	virtual bool playFrame(float dt,unsigned int nStream,int framePos);

	virtual bool startRecord(const std::string& fname,unsigned int width,unsigned int height);
	virtual void endRecord(void);
	virtual bool recordFrame(void);


private:
    //! Default copy constructor forbidden
    CAnimatorStream(const CAnimatorStream& rsh) {};

    //! Currently playing streams
    typedef struct
    {
        bool			loopStream;
        float			framePosition;
        unsigned char	*streamBuffer;
		unsigned char	*bufferPointer;
		CRaptorMutex	streamMutex;
		CEngineJob		*pJob;
        CAnimator::IVideoIO *streamer;
    } VIDEO_STREAM;

    unsigned int jobId;
    vector<VIDEO_STREAM*> m_pCurrentStreams;

	//!	Switch to exploit GPU fast memory if available
	bool	m_bUseDMA;
    IDeviceMemoryManager::IBufferObject   *memoryBuffer;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)

