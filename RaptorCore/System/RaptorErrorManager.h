// RaptorErrorManager.h: interface for the CRaptorErrorManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
#define AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)
    #include "System/RaptorMessages.h"
#endif
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
		#include "System/Raptor.h"
	#endif
#endif
#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CRaptorIO;


class RAPTOR_API CRaptorErrorManager  
{
public:
    typedef enum
    {
        RAPTOR_NO_ERROR,
        RAPTOR_WARNING,
        RAPTOR_ERROR,
        RAPTOR_GL_ERROR,
        RAPTOR_VK_ERROR,
        RAPTOR_FATAL
    } RAPTOR_ERROR_TYPE;

    typedef struct GL_RAPTOR_ERROR_TAG
    {
		std::string			className;
	    RAPTOR_ERROR_TYPE	type;
	    std::string			error;
		std::string			filename;
		uint32_t			line;
    } GL_RAPTOR_ERROR;

    typedef GL_RAPTOR_ERROR* LP_GL_RAPTOR_ERROR;


    class CRaptorErrorHandler
    {
    public:
        CRaptorErrorHandler() {};
        virtual ~CRaptorErrorHandler() {};

        virtual void handleError(const GL_RAPTOR_ERROR& err);
    };

    
#if defined RAPTOR_DEBUG_MODE_GENERATION
	#define	CATCH_GL_ERROR \
		Raptor::GetErrorManager()->glGetError(__FILE__,__LINE__);
	#define	CATCH_VK_ERROR(err) \
		Raptor::GetErrorManager()->vkGetError(err,__FILE__,__LINE__);
#else
	#define	CATCH_GL_ERROR
	#define	CATCH_VK_ERROR(err)
#endif


public:
	CRaptorErrorManager();
	virtual ~CRaptorErrorManager();

    //! Returns the index of the last registered error.
	int getLastRaptorError();

    //! Returns the error of which index is passed.
	void getRaptorError(unsigned int index,GL_RAPTOR_ERROR& err);

    //! Installs a callback to enable a user management.
	void addRaptorErrorHandler(CRaptorErrorHandler* errHandler);

    //! This method is rather dedicated to internal error handling,
    //! but user can use it to unify external API errors.
	void addRaptorError(GL_RAPTOR_ERROR& err);

    //! Intercepts openGL errors and manage them as RaptorErrors
	void glGetError(const std::string& file,int line);

	//! Text format Vulkan errors and manage them as RaptorErrors
	void vkGetError(VkResult err, const std::string& file,int line);

    //!	Errors management  ( Need an active OpenGL Context )
	void generateRaptorError(const CPersistence::CPersistenceClassID& classID,
							 RAPTOR_ERROR_TYPE type,
							 const std::string &str,
							 const char *file = "",
							 uint32_t line = 0);

	//!	Simple Marcros to raise errors.
#define RAPTOR_GEN_ERROR(cid, str, err)\
	{\
		CRaptorErrorManager* mgr = Raptor::GetErrorManager();\
		if (NULL != mgr)\
			mgr->generateRaptorError(cid, err, str, __FILE__, __LINE__);\
	}
#define RAPTOR_NO_ERROR(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_NO_ERROR)
#define RAPTOR_WARNING(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_WARNING)
#define RAPTOR_ERROR(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_ERROR)
#define RAPTOR_GLERROR(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_GL_ERROR)
#define RAPTOR_VKERROR(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_VK_ERROR)
#define RAPTOR_FATAL(cid,str) RAPTOR_GEN_ERROR(cid,str,CRaptorErrorManager::RAPTOR_FATAL)


    //!	Same as above. The message ID is one of the Raptor Message Base or user extension.
    void generateRaptorError(	const CPersistence::CPersistenceClassID& classID,
								RAPTOR_ERROR_TYPE type,
								CRaptorMessages::MESSAGE_ID id,
								const char *file = "",
								uint32_t line = 0,
								vector<CRaptorMessages::MessageArgument> &args = CRaptorMessages::no_args);

    //! Logs errors in a file in addition to memory
    //! Returns false if log file cannot be initiated.
	bool logToFile(const std::string &filename);


private:
    CRaptorIO    *m_pLogger;

	std::vector<GL_RAPTOR_ERROR>		RaptorErrors;
	std::vector<CRaptorErrorHandler*>	RaptorErrorHandlers;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
