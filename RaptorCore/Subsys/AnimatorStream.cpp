// AnimatorStream.cpp: implementation of the CAnimatorStream class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
	#include "AnimatorStream.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_PLAYFRAMEJOB_H__0A9F1B5E_0BB5_4FA1_BFAB_4244514E6C75__INCLUDED_)
    #include "PlayFrameJob.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnimatorStream::CAnimatorStream()
    :memoryBuffer(NULL),m_bUseDMA(false)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
	jobId = taskManager->generateBatchId();
}

CAnimatorStream::~CAnimatorStream()
{
	while (m_pCurrentStreams.size() > 0)
	{
		endPlayBack(0);
	}
}

void CAnimatorStream::setVideoKindIO(CAnimator::IVideoIO *streamer)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((streamer == NULL) || (streamer->getKind().empty()))
	{
		RAPTOR_WARNING(	CAnimator::CAnimatorClassID::GetClassId(),
						"Null video streamer can not be registered in an Animator");
		return;
	}
#endif

	std::vector<std::string> extensionKind = streamer->getKind();
	for (size_t j = 0; j < extensionKind.size(); j++)
	{
		string ext = extensionKind[j];
		std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

		CRaptorInstance &instance = CRaptorInstance::GetInstance();
		instance.videoKindIO.insert(map<std::string, CAnimator::IVideoIO*>::value_type(ext, streamer));
	}
}

CAnimator::IVideoIO* const CAnimatorStream::getVideoKindIO(const std::string& extension) const
{
	//	extract the right image loader
	std::string ext = extension;
	std::string::size_type pos = ext.rfind('.');
	if (pos < ext.size())
		ext = ext.substr(pos + 1);

	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	map<std::string,CAnimator::IVideoIO*>::const_iterator itr = instance.videoKindIO.find(ext);
	if (instance.videoKindIO.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}

bool CAnimatorStream::getFrameSizes(unsigned int stream, unsigned int &width, unsigned int &height) const
{
    if (stream < m_pCurrentStreams.size())
    {
        VIDEO_STREAM *video = m_pCurrentStreams[stream];
        width = video->streamer->getWidth();
        height = video->streamer->getHeight();
        return true;
    }
    else
    {
        width = 0;
        height = 0;
        return false;
    }
}

unsigned int CAnimatorStream::getFramePosition(unsigned int stream) const
{
    if (stream < m_pCurrentStreams.size())
        return m_pCurrentStreams[stream]->streamer->getFramePosition();
    else
        return 0;
}

long CAnimatorStream::glStartPlayBack(const std::string& fname,bool loop)
{
	m_bUseDMA = CTexelAllocator::GetInstance()->isMemoryRelocated();

    //	extract the right image loader
    string ext = fname;
    string::size_type pos = ext.rfind('.');
    if (pos > ext.size())
        ext = "";
    else
        ext = ext.substr(pos+1);

    CAnimator::IVideoIO* streamer = getVideoKindIO(ext);
    if (streamer == NULL)
        return -1;

    streamer = streamer->newInstance();
    VIDEO_STREAM *video = new VIDEO_STREAM;
    video->streamer = streamer;
    video->framePosition = -1.0f;
    video->streamBuffer = NULL;
	video->bufferPointer = NULL;
    video->loopStream = loop;
	video->pJob = NULL;

    m_pCurrentStreams.push_back(video);

    if (streamer->openReader(fname))
    {
        unsigned int size = 4 * streamer->getWidth() * streamer->getHeight();

		if (!m_bUseDMA)
		{
#ifdef GL_EXT_bgra
			video->streamBuffer = NULL;
#else
			CHostMemoryManager::Allocator<unsigned char> allocator;
			video->streamBuffer = allocator.allocate(size); 

			for (int i=0;i<size;i+=4)
			{	
				*(video->streamBuffer+i) = 0;
				*(video->streamBuffer+i+1) = 0;
				*(video->streamBuffer+i+2) = 0;
				*(video->streamBuffer+i+3) = 255;
			}
#endif
		}
		else
		{
			CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
			video->bufferPointer = pAllocator->allocateTexels(size);
		}

#if defined(RAPTOR_SMP_CODE_GENERATION)
        video->pJob = new CPlayFrameJob(this,m_pCurrentStreams.size()-1,jobId);
		CRaptorInstance &instance = CRaptorInstance::GetInstance();
		C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
		taskManager->registerJob(video->pJob);
#endif

    	return m_pCurrentStreams.size() - 1;
    }
    else
        return -1;
}

void CAnimatorStream::playFrames(float dt)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	CPlayFrameJob::SetJobTime(dt);
	if (m_pCurrentStreams.size() > 0)
	{
		CRaptorInstance &instance = CRaptorInstance::GetInstance();
		C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
		taskManager->batchJobs(jobId);
	}
#else
    for (size_t i=0;i<m_pCurrentStreams.size();i++)
        playFrame(dt,i,-1);
#endif
}

void CAnimatorStream::endPlayBack(unsigned int nStream)
{
    if (nStream < m_pCurrentStreams.size())
    {
        vector<VIDEO_STREAM*>::iterator it = m_pCurrentStreams.begin() + nStream;
        if ((*it)->streamer->closeReader())
        {
			if (!m_bUseDMA)
			{
#ifndef GL_EXT_bgra
				CHostMemoryManager::GetInstance()->garbage((*it)->streamBuffer);
#endif
			}
			else
			{
				CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
				pAllocator->releaseTexels((*it)->bufferPointer);
			}

#if defined(RAPTOR_SMP_CODE_GENERATION)
			CRaptorInstance &instance = CRaptorInstance::GetInstance();
			C3DEngineTaskManager *taskManager = instance.engineTaskMgr;
			taskManager->unregisterJob((*it)->pJob);
			delete (*it)->pJob;
#endif

            delete (*it);
            m_pCurrentStreams.erase(it);
        }
    }
}

bool CAnimatorStream::playFrame(float dt,unsigned int nStream,int framePos)
{
    if (nStream >= m_pCurrentStreams.size())
        return false;

    VIDEO_STREAM* video = m_pCurrentStreams[nStream];

	if (framePos >= 0)
	{
		video->streamer->seekFrame(framePos);
        video->framePosition = 0;
		return false;
	}

    // Synchronize stream with expected frame rate.
    float frameLength = video->streamer->getFrameLength();
    if (video->framePosition < 0)
        video->framePosition = frameLength;
    else
        video->framePosition += dt;
    if (video->framePosition >= frameLength)
        video->framePosition -= frameLength;
    else
        return false;

	//! After this point, we are in critical section until end of method.
	//!	This mutex exludes glGetStreamBuffer/glReleaseStreamBuffer
	CRaptorLock lock(video->streamMutex);

    unsigned char *readBuffer = NULL;
    if (video->streamer->readFrame(readBuffer, video->framePosition))
	{
        // filtering: if a filter is defined, it could be applyied here,
        // this needing to copy pixels and process them here directly
        //  because playFrame is designed to be called in a separate thread.
        //  Filter would be more efficient using a pixel shader in glGenerate
		//	NEED TO OPTIMIZE !!!
		if (!m_bUseDMA)
		{
    #ifdef GL_EXT_bgra
			video->streamBuffer = readBuffer;
    #else
		    int size = 4 * video->streamer->getWidth() * video->streamer->getHeight();
		    for (int i=0,j=0;i<size;i+=4,j+=3)
		    {	
			    *(video->streamBuffer+i) = *(readBuffer+j+2);
			    *(video->streamBuffer+i+1) = *(readBuffer+j+1);
			    *(video->streamBuffer+i+2) = *(readBuffer+j+0);
		    }
    #endif
		}
		else
		{
			// Assume GL_EXT_bgra is supported
            video->streamBuffer = readBuffer;
		}

		return true;
	}
	else
    {
        video->streamBuffer = NULL;
        if (video->loopStream)
            video->streamer->seekFrame(0);    // should use AVI start position instead of 0
        return false;
    }
}

void CAnimatorStream::glReleaseStreamBuffer(unsigned int nStream)
{
	if (nStream >= m_pCurrentStreams.size())
		return;

	VIDEO_STREAM *video = m_pCurrentStreams[nStream];
	video->streamMutex.unlock();

	if (m_bUseDMA)
	{
		//	This call is not necessary since the texel allocator is locked by the display,
		//	we keep it active for other stream data transfer.
		//CTexelAllocator::GetInstance()->glLockMemory(false);

		CATCH_GL_ERROR
	}
}

const unsigned char* const CAnimatorStream::glGetStreamBuffer(unsigned int nStream)
{
	if (nStream >= m_pCurrentStreams.size())
		return NULL;
    
	VIDEO_STREAM *video = m_pCurrentStreams[nStream];
	video->streamMutex.lock();

	if (!m_bUseDMA)
	{
		return video->streamBuffer;
	}
	else
    {
        if (video->streamBuffer != NULL)
        {
            unsigned int size = 3 * video->streamer->getWidth() * video->streamer->getHeight();
			CTexelAllocator::GetInstance()->glvkSetPointerData(video->bufferPointer,video->streamBuffer,size);
        }

        //	This call is not necessary since the texel allocator is locked by the display,
		//	we keep it active for other stream data transfer.
		//CTexelAllocator::GetInstance()->glLockMemory(true);

        CATCH_GL_ERROR

		return video->bufferPointer;
    }
}

bool CAnimatorStream::startRecord(const std::string& fname,unsigned int width,unsigned int height)
{
    return false;
}

void CAnimatorStream::endRecord(void)
{
}

bool CAnimatorStream::recordFrame(void)
{
    return false;
}

