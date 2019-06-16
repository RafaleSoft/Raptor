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
		localtime_s(&newtime, &timer);
		char buffer[256];
		asctime_s(buffer,256,&newtime);
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
    {
        RaptorErrorHandlers.push_back(errHandler);
    }
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
		localtime_s(&newtime, &timer);
		char buffer[256];
		asctime_s(buffer, 256, &newtime);

        switch (err.type)
        {
            case RAPTOR_NO_ERROR:
				*m_pLogger << " - Info(" << buffer << "): " << err.error.data() << '\n';
                break;
            case RAPTOR_WARNING:
				*m_pLogger << " - Warning(" << buffer << "): " << err.error.data() << '\n';
                break;
            case RAPTOR_ERROR:
				*m_pLogger << " - Error(" << buffer << "): " << err.error.data() << '\n';
                break;
            case RAPTOR_GL_ERROR:
				*m_pLogger << " - OpenGL error(" << buffer << "): " << err.error.data() << '\n';
                break;
			case RAPTOR_VK_ERROR:
				*m_pLogger << " - Vulkan error(" << buffer << "): " << err.error.data() << '\n';
                break;
            case RAPTOR_FATAL:
				*m_pLogger << " - Fatal(" << buffer << "): " << err.error.data() << '\n';
                break;
        }
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
	{
		err = RaptorErrors[index];
	}
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
    r_line << ends;

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
				r_err.error = "Invalid enum (class " + r_file + r_line.str() + ")";
				break;
			case GL_INVALID_VALUE:
				r_err.error = "Invalid value (class " + r_file + r_line.str() + ")";
				break;
			case GL_INVALID_OPERATION:
				r_err.error = "Invalid operation (class " + r_file + r_line.str() + ")";
				break;
			case GL_STACK_OVERFLOW:
				r_err.error = "Stack overflow (class " + r_file + r_line.str() + ")";
				break;
			case GL_STACK_UNDERFLOW:
				r_err.error = "Stack underflow (class " + r_file + r_line.str() + ")";
				break;
			case GL_OUT_OF_MEMORY:
				r_err.error = "Out of memory (class " + r_file + r_line.str() + ")";
				break;
		}

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
			r_err.error = "No error, success (class " + r_file + r_line.str() + ")";
			break;
		case VK_NOT_READY:
			r_err.error = "Not ready (class " + r_file + r_line.str() + ")";
			break;
		case VK_TIMEOUT:
			r_err.error = "Timeout (class " + r_file + r_line.str() + ")";
			break;
		case VK_EVENT_SET:
			r_err.error = "Event Set (class " + r_file + r_line.str() + ")";
			break;
		case VK_EVENT_RESET:
			r_err.error = "Event Reset (class " + r_file + r_line.str() + ")";
			break;
		case VK_INCOMPLETE:
			r_err.error = "Incomplete (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			r_err.error = "Error Out of Host Memory (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			r_err.error = "Error Out of Host Memory (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			r_err.error = "Error initialization failed (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_DEVICE_LOST:
			r_err.error = "Error device lost (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			r_err.error = "Error memory map failed (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			r_err.error = "Error layer not present (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			r_err.error = "Error extension not present (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			r_err.error = "Error feature not present (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			r_err.error = "Error incompatible driver (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			r_err.error = "Error too maby objects (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			r_err.error = "Error format not supported (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			r_err.error = "Error fragmented pool (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			r_err.error = "Error surface lost (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			r_err.error = "Error native window in use (class " + r_file + r_line.str() + ")";
			break;
		case VK_SUBOPTIMAL_KHR:
			r_err.error = "Error suboptimal (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			r_err.error = "Error out of date (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			r_err.error = "Error incompatible display (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			r_err.error = "Error validation failed (class " + r_file + r_line.str() + ")";
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			r_err.error = "Error invalid shader NV (class " + r_file + r_line.str() + ")";
			break;
		default:
			r_err.error = "Unknown Error (class " + r_file + r_line.str() + ")";
			break;
	}

	if (VK_SUCCESS != err)
		addRaptorError(r_err);
}

