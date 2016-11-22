// RaptorMessages.cpp: implementation of the CRaptorMessages class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
    #include "GLHierarchy/Persistence.h"
#endif

#if !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)
    #include "System/RaptorMessages.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "RaptorIO.h"
#endif



RAPTOR_NAMESPACE_BEGIN

vector<CRaptorMessages::MessageArgument>  CRaptorMessages::no_args;
vector<CRaptorMessages::ARG_TYPE> CRaptorMessages::no_args_type;
string CRaptorMessages::no_message("[]");
static string message_result;

typedef struct MSG_VAL_t
{ 
	string message; 
	vector<CRaptorMessages::ARG_TYPE> args; 

	MSG_VAL_t():
	message(""),args(CRaptorMessages::no_args_type) {}
	MSG_VAL_t(const char* str, vector<CRaptorMessages::ARG_TYPE> &arg_t):
	message(str),args(arg_t) {}
} MSG_VAL;
typedef map<unsigned int, MSG_VAL>  MSGS_IDS;
typedef map<std::string, MSGS_IDS >		MSGS_MAP;
typedef map<std::string,MSGS_MAP*>		LANGUAGES_MAP;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

#pragma warning(disable : 4503)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorMessages::CRaptorMessages()
{
    LANGUAGES_MAP      *planguages_map = new LANGUAGES_MAP;
    dBase = planguages_map;
}

CRaptorMessages::~CRaptorMessages()
{
    LANGUAGES_MAP &languages_map = *((LANGUAGES_MAP*)dBase);
    LANGUAGES_MAP::const_iterator it = languages_map.begin();

    while (it != languages_map.end())
    {
        delete (*it).second;
        it++;
    }

    languages_map.clear();

    delete ((LANGUAGES_MAP*)dBase);
}

void CRaptorMessages::displayMessage(const char* msg) const
{
#if defined(_WIN32)
	::MessageBox(NULL,msg,"Raptor Message",MB_OK);
#else
    std::cout << msg << std::endl;
#endif
}


vector<std::string> CRaptorMessages::getLanguages(void) const
{
    LANGUAGES_MAP &languages_map = *((LANGUAGES_MAP*)dBase);
    vector<string>  languages;

    LANGUAGES_MAP::const_iterator itr = languages_map.begin();
    while (itr != languages_map.end())
    {
        languages.push_back((*itr++).first);
    }

    return languages;
}

const std::string& CRaptorMessages::getMessage(const std::string& className, 
											   MESSAGE_ID id,
											   vector<MessageArgument>& args,
											   const char *lang) const
{
    LANGUAGES_MAP &languages_map = *((LANGUAGES_MAP*)dBase);

	LANGUAGES_MAP::const_iterator it_language = languages_map.begin();
    if (lang != NULL)
		it_language = languages_map.find(lang);
    
    if (it_language != languages_map.end())
    {
        const MSGS_MAP &language = *((*it_language).second);

        MSGS_MAP::const_iterator it_messages = language.find(className);
        if (it_messages != language.end())
        {
            const MSGS_IDS &ids = (*it_messages).second;

            MSGS_IDS::const_iterator it_message = ids.find(id);
            if (it_message != ids.end())
			{
                const MSG_VAL &msg = (*it_message).second;
				if (!args.empty())
				{
                    message_result = msg.message;
					for (unsigned int i=0;i<args.size();i++)
                    {
                        size_t argPos = message_result.find('%');
						if ((argPos != string::npos) && (argPos < message_result.length()-1))
                        {
                            stringstream argVal;
							char fmt = message_result[argPos+1];
							switch(fmt)
							{
								case 'd':
									argVal << args[i].arg_int;
                                    break;
								case 'f':
									argVal << args[i].arg_float;
                                    break;
								case 's':
									argVal << args[i].arg_sz;
                                    break;
							}
                            argVal << ends;
                            message_result = message_result.substr(0,argPos) + argVal.str() + message_result.substr(argPos+2);
                        }
                    }
					
					return message_result;
				}
				else
					return msg.message;
			}
            else
                return no_message;
        }
        else
            return no_message;
    }
    else
        return no_message;
}

bool CRaptorMessages::importMessage(CRaptorIO& io,void *pmessages)
{
    string name;
    io >> name;

	MSGS_IDS &objectMessages = *((MSGS_IDS*)pmessages);

	MESSAGE_ID messageId = UNKNOWN_ID;
	MSG_VAL	objectMessage;
	string	id = "";
	string	str = "";

	string data = io.getValueName();
    while (!data.empty())
    {
        if (data == "id")
		{
			io >> id;
			if (id == "ID_NULL_OBJECT")
				messageId = ID_NULL_OBJECT;
			else if (id == "ID_LOST_LINK")
				messageId = ID_LOST_LINK;
			else if (id == "ID_NO_GPU_PROGRAM")
				messageId = ID_NO_GPU_PROGRAM;
			else if (id == "ID_NO_GPU_COLOR2")
				messageId = ID_NO_GPU_COLOR2;
			else if (id == "ID_NO_GPU_DOT3")
				messageId = ID_NO_GPU_DOT3;
			else if (id == "ID_FORMAT_NOT_SUPPORTED")
				messageId = ID_FORMAT_NOT_SUPPORTED;
			else if (id == "ID_PROGRAM_ERROR")
				messageId = ID_PROGRAM_ERROR;
			else if (id == "ID_WRONG_RENDERING")
				messageId = ID_WRONG_RENDERING;
			else if (id == "ID_TEXTURE_MISS")
				messageId = ID_TEXTURE_MISS;
			else if (id == "ID_SUBSYS_FAILED")
				messageId = ID_SUBSYS_FAILED;
			else if (id == "ID_CREATE_FAILED")
				messageId = ID_CREATE_FAILED;
			else if (id == "ID_INIT_FAILED")
				messageId = ID_INIT_FAILED;
			else if (id == "ID_DESTROY_FAILED")
				messageId = ID_DESTROY_FAILED;
			else if (id == "ID_OBJECT_DUPLICATE")
				messageId = ID_OBJECT_DUPLICATE;
			else if (id == "ID_UPDATE_FAILED")
				messageId = ID_UPDATE_FAILED;
			else if (id == "ID_PROCEDURE_FAILED")
				messageId = ID_PROCEDURE_FAILED;
			else if (id == "ID_NO_RESOURCE")
				messageId = ID_NO_RESOURCE;
		}
        else if (data == "str")
			io >> objectMessage.message;
        else if (data == "arg")
		{
			io >> name;
		}
		else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	if (UNKNOWN_ID != messageId)
	{
		objectMessages[messageId] = objectMessage;
		return true;
	}
	else
		return false;
}

bool CRaptorMessages::importClass(CRaptorIO& io,void *planguage)
{
    string name;
    io >> name;

	MSGS_IDS	objectMessages;
	string		className = "";

	string data = io.getValueName();
    while (!data.empty())
    {
        if (data == "name")
        {
			io >> className;
        }
        else if (data == "Message")
        {
            importMessage(io,&objectMessages);
        }
        else
			io >> name;
		
		data = io.getValueName();
	}
	io >> name;

	if (!className.empty())
	{
		MSGS_MAP	&rlanguage = *((MSGS_MAP*)planguage);
		rlanguage[className] = objectMessages;
		return true;
	}
	else
		return false;
}

bool CRaptorMessages::LoadMessages(const std::string& fname)
{
    CRaptorIO *m_pTranslator = CRaptorIO::Create(NULL,CRaptorIO::DISK_READ,CRaptorIO::ASCII_XML);

	stringstream shemaLocation;
	char *raptor_root = getenv("RAPTOR_ROOT");
	if (NULL != raptor_root)
	{
		shemaLocation << raptor_root;
		shemaLocation << "/Redist/bin/RaptorMessages.xsd";
	}
	else
		shemaLocation << "./RaptorMessages.xsd";

	m_pTranslator->read((void*)shemaLocation.str().c_str(),0);
	m_pTranslator->read((void*)fname.c_str(),0);

    string name;
	*m_pTranslator >> name;
    string data = m_pTranslator->getValueName();

	if (data.compare("RaptorMessages"))
	{
		delete m_pTranslator;
		return false;
	}

	MSGS_MAP *planguage = NULL;

    //  skip data intro
    *m_pTranslator  >> name; 
    data = m_pTranslator->getValueName();

    while (!data.empty())
    {
        if (data == "Class")
        {
            if (importClass(*m_pTranslator,planguage))
			{
			}
        }
		else if (data == "language")
		{
			*m_pTranslator >> name;
			if (!name.empty())
			{
				LANGUAGES_MAP &languages_map = *((LANGUAGES_MAP*)dBase);
				if (languages_map.find(name) == languages_map.end())
				{
					planguage = new MSGS_MAP;
					languages_map[name] = planguage;
				}
			}
			else
			{
				delete m_pTranslator;
				return false;
			}
		}
		else
			*m_pTranslator >> name;
    
        data = m_pTranslator->getValueName();
	}

    delete m_pTranslator;

	return true;
}

