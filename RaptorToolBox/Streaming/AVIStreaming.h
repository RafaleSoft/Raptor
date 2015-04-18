// AVIStreaming.h: interface for the CAVIStreaming class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTREAMING_H__BA673CA4_15E6_4B9E_8AF0_EC0184F57FDD__INCLUDED_)
#define AFX_AVISTREAMING_H__BA673CA4_15E6_4B9E_8AF0_EC0184F57FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
    #include "Engine/Animator.h"
#endif

#if defined(_WIN32)
    //	non standard structures used
    #pragma warning( disable : 4201 )  
    #include <vfw.h>
#endif


RAPTOR_NAMESPACE

#if defined(_WIN32)
class CAVIStreaming : public CAnimator::IVideoIO
{
public:
    CAVIStreaming()
        :AVIFile(NULL),AVIStream(NULL),AVIBuffer(NULL),AVIFrame(NULL),
        frameLength(0),streamPos(0),locked(false) {};
    virtual ~CAVIStreaming() {};

	virtual std::string getKind(void) const { return "AVI"; };
    virtual unsigned int getWidth() const { if (AVIBuffer == NULL) return 0; else return AVIBuffer->biWidth; };
    virtual unsigned int getHeight() const { if (AVIBuffer == NULL) return 0; else return AVIBuffer->biHeight; };
    virtual float getFrameLength() const { return frameLength; };

    virtual CAnimator::IVideoIO*   newInstance(void) const { return new CAVIStreaming; };
	virtual bool releaseInstance(void) { delete this; return true;};

	virtual bool openReader(const std::string &fname);
    virtual bool readFrame(unsigned char *& readBuffer);
    virtual void seekFrame(unsigned int framePos) { streamPos = framePos; };
    virtual unsigned int getFramePosition(void) const { return streamPos; };
    virtual bool closeReader(void);

    virtual bool openRecorder(const std::string& fname,unsigned int width,unsigned int height);
    virtual bool writeFrame(const unsigned char *writeBuffer);
    virtual bool closeRecorder(void);

private:
    PAVIFILE				AVIFile;
    PAVISTREAM				AVIStream;
    LPBITMAPINFOHEADER		AVIBuffer;
    PGETFRAME               AVIFrame;
    COMPVARS				cVars;

    float       frameLength;
    LONG		streamPos;
    bool        locked;
};
#endif

#endif	// !defined(AFX_AVISTREAMING_H__BA673CA4_15E6_4B9E_8AF0_EC0184F57FDD__INCLUDED_)

