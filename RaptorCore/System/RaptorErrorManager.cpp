// RaptorErrorManager.cpp: implementation of the CRaptorErrorManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
    #include "System/RaptorIO.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif

#include <time.h>

#if defined(WIN32)
	#define LOCALTIME(_Tm,_Time) localtime_s(_Tm,_Time)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_s(_Buf, _SizeInBytes, _Tm)
#elif defined(LINUX)
	#define LOCALTIME(_Tm,_Time) localtime_r(_Time,_Tm)
	#define ASCTIME(_Buf, _SizeInBytes, _Tm) asctime_r(_Tm, _Buf)
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorErrorManager::CRaptorErrorManager()
	:m_pLogger(NULL),RaptorErrors(),RaptorErrorHandlers()
{
}

CRaptorErrorManager::~CRaptorErrorManager()
{
    if (m_pLogger != NULL)
        delete m_pLogger;
}

bool CRaptorErrorManager::logToFile(const std::string &filename)
{
    if (m_pLogger != NULL)
	{
        delete m_pLogger;
		m_pLogger = NULL;
	}

    m_pLogger = CRaptorIO::Create(filename.data(),CRaptorIO::DISK_WRITE);

    bool opened = (m_pLogger != NULL) && (m_pLogger->getStatus() == CRaptorIO::IO_OK);
    if (opened)
    {
        time_t timer;
        time(&timer);

		struct tm newtime;
		LOCALTIME(&newtime, &timer);
		char buffer[256];
		ASCTIME(buffer, 256, &newtime);
        *m_pLogger << "\nRaptor log file initiated at " << buffer;
        *m_pLogger << '\n';
    }

    return opened;
}

void CRaptorErrorManager::generateRaptorError(	const CPersistence::CPersistenceClassID& classID,
												RAPTOR_ERROR_TYPE type,
												const std::string &str)
{
	if (m_pLogger == NULL)
		return;

	GL_RAPTOR_ERROR err;

	err.className = classID.ClassName();
	err.type = type;
	err.error = str;
	
	addRaptorError(err);
}

void CRaptorErrorManager::generateRaptorError(	const CPersistence::CPersistenceClassID& classID,
												RAPTOR_ERROR_TYPE type,
												CRaptorMessages::MESSAGE_ID id,
												vector<CRaptorMessages::MessageArgument> &args)
{
    GL_RAPTOR_ERROR err;

	err.className = classID.ClassName();
	err.type = type;

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    if (instance.pMessages != NULL)
        err.error = instance.pMessages->getMessage(err.className,id,args);
    else
        err.error = "Raptor not initialised.";
	
	addRaptorError(err);
}

void CRaptorErrorManager::addRaptorErrorHandler(CRaptorErrorHandler* errHandler)
{
    if (errHandler != NULL)
        RaptorErrorHandlers.push_back(errHandler);
}

void CRaptorErrorManager::addRaptorError(GL_RAPTOR_ERROR& err)
{
	if (RaptorErrorHandlers.size() > 0)
    {
        for (unsigned int i=0;i<RaptorErrorHandlers.size();i++)
		    RaptorErrorHandlers.at(i)->handleError(err);
    }

	RaptorErrors.push_back(err);

    bool opened = (m_pLogger != NULL) && (m_pLogger->getStatus() == CRaptorIO::IO_OK);
    if (opened)
    {
		*m_pLogger << err.className.data();

		time_t timer;
		time(&timer);

		struct tm newtime;
		LOCALTIME(&newtime, &timer);
		char buffer[256];
		ASCTIME(buffer, 256, &newtime);
		buffer[24] = buffer[25] = 0;	// msdn tells there is a new lire and \0 at end of buffer.

        switch (err.type)
        {
            case RAPTOR_NO_ERROR:
				*m_pLogger << " - Info(";
                break;
            case RAPTOR_WARNING:
				*m_pLogger << " - Warning(";
                break;
            case RAPTOR_ERROR:
				*m_pLogger << " - Error(";
                break;
            case RAPTOR_GL_ERROR:
				*m_pLogger << " - OpenGL error(";
                break;
			case RAPTOR_VK_ERROR:
				*m_pLogger << " - Vulkan error(";
                break;
            case RAPTOR_FATAL:
				*m_pLogger << " - Fatal(";
                break;
        }

		*m_pLogger << buffer << "): " << err.error.data() << '\n';
    }
}

int CRaptorErrorManager::getLastRaptorError(void)
{
	return RaptorErrors.size() - 1;
}

void CRaptorErrorManager::getRaptorError(unsigned int index,GL_RAPTOR_ERROR& err)
{
	err.className = "";
	err.type = RAPTOR_NO_ERROR;
	err.error = "";

	if (index < RaptorErrors.size())
		err = RaptorErrors[index];
}

void CRaptorErrorManager::glGetError(const std::string& file,int line)
{
	GLenum err = ::glGetError(); //GL_INVALID_OPERATION;
	if (GL_NO_ERROR == err )
		return;

	GLenum prv_err = GL_NO_ERROR;
	GL_RAPTOR_ERROR r_err;
	r_err.className = COpenGL::COpenGLClassID::GetClassId().ClassName();
	r_err.type = RAPTOR_GL_ERROR;

	//	extract class name based on file name
    string::size_type pos1 = file.rfind('\\');
    string::size_type pos2 = file.rfind('.');
	string r_file = file.substr(pos1+1,pos2-pos1-1);

	stringstream r_line;
    r_line << " [line:";
    r_line << line;
    r_line << "]";

	while ( GL_NO_ERROR != err )
	{
		//err = ::glGetError();

		if ((err != GL_NO_ERROR) && (err == prv_err))
		{
            r_err.error = "Unrecoverable error has been detected.\n Are you issuing OpenGL commands without a rendering context ?";
            addRaptorError(r_err);
			break;
		}

		switch (err)
		{
			case GL_NO_ERROR:
				break;
			case GL_INVALID_ENUM:
				r_err.error = "Invalid enum ";
				break;
			case GL_INVALID_VALUE:
				r_err.error = "Invalid value ";
				break;
			case GL_INVALID_OPERATION:
				r_err.error = "Invalid operation ";
				break;
			case GL_STACK_OVERFLOW:
				r_err.error = "Stack overflow ";
				break;
			case GL_STACK_UNDERFLOW:
				r_err.error = "Stack underflow ";
				break;
			case GL_OUT_OF_MEMORY:
				r_err.error = "Out of memory ";
				break;
		}

		r_err.error += "(class ";
		r_err.error += r_file;
		r_err.error += r_line.str().c_str();
		r_err.error += ")";
		if (GL_NO_ERROR != err )
			addRaptorError(r_err);

		prv_err = err;
		err = ::glGetError();
	}
}

void CRaptorErrorManager::vkGetError(VkResult err, const std::string& file,int line)
{
	if (VK_SUCCESS == err)
		return;

	GL_RAPTOR_ERROR r_err;

	r_err.className = CVulkan::CVulkanClassID::GetClassId().ClassName();
	r_err.type = RAPTOR_VK_ERROR;

	//	extract class name based on file name
    string::size_type pos1 = file.rfind('\\');
    string::size_type pos2 = file.rfind('.');
	string r_file = file.substr(pos1+1,pos2-pos1-1);

	stringstream r_line;
    r_line << " [line:";
    r_line << line;
    r_line << "]";
    r_line << ends;

	switch (err)
	{
		case VK_SUCCESS:
			r_err.error = "No error, success ";
			break;
		case VK_NOT_READY:
			r_err.error = "Not ready ";
			break;
		case VK_TIMEOUT:
			r_err.error = "Timeout ";
			break;
		case VK_EVENT_SET:
			r_err.error = "Event Set ";
			break;
		case VK_EVENT_RESET:
			r_err.error = "Event Reset ";
			break;
		case VK_INCOMPLETE:
			r_err.error = "Incomplete ";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			r_err.error = "Error Out of Host Memory ";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			r_err.error = "Error Out of Host Memory ";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			r_err.error = "Error initialization failed ";
			break;
		case VK_ERROR_DEVICE_LOST:
			r_err.error = "Error device lost ";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			r_err.error = "Error memory map failed ";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			r_err.error = "Error layer not present ";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			r_err.error = "Error extension not present ";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			r_err.error = "Error feature not present ";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			r_err.error = "Error incompatible driver ";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			r_err.error = "Error too maby objects ";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			r_err.error = "Error format not supported ";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			r_err.error = "Error fragmented pool ";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			r_err.error = "Error surface lost ";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			r_err.error = "Error native window in use ";
			break;
		case VK_SUBOPTIMAL_KHR:
			r_err.error = "Error suboptimal ";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			r_err.error = "Error out of date ";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			r_err.error = "Error incompatible display ";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			r_err.error = "Error validation failed ";
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			r_err.error = "Error invalid shader NV ";
			break;
		default:
			r_err.error = "Unknown Error ";
			break;
	}

	r_err.error += "(class ";
	r_err.error += r_file;
	r_err.error += r_line.str().c_str();
	r_err.error += ")";

	if (VK_SUCCESS != err)
		addRaptorError(r_err);
}

