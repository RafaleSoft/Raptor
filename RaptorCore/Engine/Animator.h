/***************************************************************************/
/*                                                                         */
/*  Animator.h                                                             */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
#define AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CAnimatorStream;
class ITextureGenerator;



//////////////////////////////////////////////////////////////////////
//!
//! This class is the base class for time-synchronisation of Raptor
//!	objects :
//!	- CPhysics
//!	- CViewPoint
//!	- CTexture ( vidéo animated textures )
//!	- CModifiers.
//!
//!	Raptor processes these objects before the frame rendering. Thus
//!	user job done in these classes should be OpenGL-state independant.
//!	The whole process is done automatically and cannot be derived
//!	because of synchronisation constraints.
//!	
//!	The benefit of this job splitting ( mathematical / graphical 
//!	pipelines ) is that SMP can and should be used. Even on a mono-CPU
//!	job can be done while waiting for framerate v-sync and GPU readiness
//!
//!
class RAPTOR_API CAnimator : public CPersistence
{
public:
    class RAPTOR_API IVideoIO
    {
    public:
        //!	Returns the kind of image managed ( common extension used for image type: e.g. AVI)
        virtual string getKind(void) const = 0;

        //! Returns a new instance for a video stream
        virtual IVideoIO*   newInstance(void) const = 0;

		//!	Frees the instance, releasing shared resources if any.
		virtual bool releaseInstance(void) = 0;

        //! Returns video width in pixels
        virtual unsigned int getWidth() const = 0;
        //! Returns video height in pixels
        virtual unsigned int getHeight() const = 0;
        //! Returns the frame duration in ms.
        virtual float getFrameLength() const = 0;

        //! Starts a video player
		virtual bool openReader(const std::string &fname) = 0;

        //! Reads the next video frame.
        //! @Param readBuffer : a reference to a pointer that will receive the frame pixels,
        //! the user must not modify the readBuffer in any circumstancies.
        //! @Return true if next frame read successfully, false otherwise ( end of stream ? )
        virtual bool readFrame(unsigned char *& readBuffer) = 0;

        //! Sets the read head to the frame passed in parameter if it exists.
        //! Rq: the frame index is zero based, and if a position is requested beyond the
        //! end of the stream, it is ignored and the read head is after the last frame.
        virtual void seekFrame(unsigned int framePos) = 0;

        //! Returns the cursor position ( volid for both reading & writing )
        virtual unsigned int getFramePosition(void) const = 0;

        //! Terminates a video player.
        //! @Return true if closing successfull, false otherwise ( not opened ? )
        virtual bool closeReader(void) = 0;

        //! Starts a video recorder
        virtual bool openRecorder(	const std::string& fname,
									unsigned int width,
									unsigned int height) = 0;

        //! Writes the next video frame.
        //! @param writeBuffer : a user pointer to a memory block containing the frame pixels to write,
        //! the user is responsible for managing the write buffer memory.
        //! @return : true if the frame is written successfully, false otherwise ( stream not opened ? )
        virtual bool writeFrame(const unsigned char *writeBuffer) = 0;

        //! Terminates a video recorder.
        //! @Return true if closing successfull, false otherwise ( not opened ? )
        virtual bool closeRecorder(void) = 0;


    protected:
		//!	Destructor is not accessible
        IVideoIO() {};

		//!	Desctructor needs to be accessible to clean memory.
		//!	Animator will manage proper destruction
		virtual ~IVideoIO() {};
    };


public:
	CAnimator(const std::string& name = "ANIMATOR");
	virtual ~CAnimator();

	//!	This method specifies the only one Animator in the process.
	static void SetAnimator(CAnimator *Animator);

	//!	This method returns the single animator of the process.
	static CAnimator *GetAnimator(void);


	//!	SMP Animator if available
	//!	This method is called by Raptor once for each frame, 
	//! it shouldn't be necessary to call it directly
	void animate(void);

	//!	Enable/Disable animation for all CTimeObjects
	//! ( the previous state of each modified object is not saved )
	void animateAll(bool animate);

	//!	User animation processing.
	//!	Synchronization is not done on this method,
	//!	thus it is the ideal place to do the job that
	//!	is to be done for each frame and that
	//!	the renderer do not have to wait on.
	virtual void asyncAnimate(void);

	//!	Initialize time objects for next call to 'animate'.
	void initSynchro(void);

    //!	Nb of frames to skip for animation processing
	//!	(e.g. if framedelay == 3, the Animate job is
	//!	done every 3 frames )
    void setFrameDelay(short framedelay) { m_sFrameDelay = framedelay; };

    //! Returns the number of time objects that remain for update
    size_t getAnimateRemaining(void) const { return m_uiAnimateRemaining; }



    //!
	//!	Video streaming management
    //!
	//!	set a video streamer class for a specific kind of video.
	//!	The different streamers are choosen by the file extension when glStartPlayBack is called. 
    //! If there is already a streamer for an extension, the one given here replaces the existing one.
	//!	By default, there is no streamer set. Some basics are provided by CRaptorToolBox
	void setVideoKindIO(IVideoIO *streamer);

	//!	Returns a streamer given a file extension.
	//! The streamer is one defined above.
	IVideoIO* const getVideoKindIO(const std::string& extension) const;

    //! Starts recording frames in a new stream:
    //! -   fname : names the stream on disk
    //! -   width : is the stream width ( no specific testing or algorithm are applyied in this version )
    //! -   height : is the stream height ( no specific testing or algorithm are applyied in this version )
	bool startRecord(	const std::string& fname,
						unsigned int width,
						unsigned int height);

    //! Records a single frame on the current recording stream
	bool recordFrame(void);

    //! Finishes recording, closing files and releasing datas.
    void endRecord(void);

	//!	Prepares a video stream for video animation
	//!	This stream can be a source for animated textures.
    //!    @return : newly created texture generator for the stream or NULL if there were errors.
	ITextureGenerator *glStartPlayBack(	const std::string& fname,
										bool loop = false) const;

    //! Terminates a stream playback, releasing all allocated datas.
	void endPlayBack(unsigned int nStream = 0);

    //! Returns the frame dimensions ( in pixels ) of the requested stream.
    bool getFrameSizes(unsigned int stream, unsigned int &width, unsigned int &height) const;

	//! Implements CPersistence
	DECLARE_CLASS_ID(CAnimatorClassID,"Animator",CPersistence)



private:
	CAnimatorStream	*m_pStream;

    //! The number of time object that remaining for update.
    size_t	m_uiAnimateRemaining;

    //! The amount of time ( in ms ) since last frame
	float	deltat;

    //!	Number of frames to skip between each animation
    short	m_sFrameDelay;

    //!	Number of frames skipped
	short	m_sCount;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)


