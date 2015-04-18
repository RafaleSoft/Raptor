// RaptorMessages.h: interface for the CRaptorMessages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)
#define AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE_BEGIN


//! This class handles Raptor information and error messages
class RAPTOR_API CRaptorMessages  
{
public:
    typedef enum MESSAGE_ID_t
    {
        ID_NULL_OBJECT,
        ID_LOST_LINK,
        ID_NO_GPU_PROGRAM,
        ID_NO_GPU_COLOR2,
		ID_NO_GPU_DOT3,
        ID_FORMAT_NOT_SUPPORTED,
        ID_PROGRAM_ERROR,
		ID_WRONG_RENDERING,
		ID_TEXTURE_MISS,
		ID_SUBSYS_FAILED,
		ID_CREATE_FAILED,
		ID_INIT_FAILED,
		ID_DESTROY_FAILED,
		ID_OBJECT_DUPLICATE,
		ID_UPDATE_FAILED,
		ID_PROCEDURE_FAILED,
		ID_NO_RESOURCE
    } MESSAGE_ID;

    typedef enum
    {
        ARG_INT,
        ARG_FLOAT,
        ARG_SZ
    } ARG_TYPE;

    typedef union
    {
        int			arg_int;
        float		arg_float;
        const char*	arg_sz;
    } MessageArgument;

    static vector<MessageArgument>  no_args;
    static vector<ARG_TYPE> no_args_type;
    static string no_message;

public:
	CRaptorMessages();
	virtual ~CRaptorMessages();

    //! Return the available languages in database
	vector<std::string> getLanguages(void) const;

    //! Returns the translated message
	const std::string& getMessage(	const std::string& className, 
									MESSAGE_ID,
									vector<MessageArgument>& args=no_args,
									const char *language = NULL) const;

    //! Adds a message to database. Returns false if message already in base
    //! ( and then it is not added ) or if any error occured. Returns true otherwise.
	bool addMessage(const std::string& className, 
					MESSAGE_ID msgId, 
					const std::string& msgstr,
					vector<ARG_TYPE>& args = no_args_type);

    //! show messages for critical errors.
    void displayMessage(const char* msg) const;

	//!	load a message file
	//! @return false if an error is encountered
	bool LoadMessages(const std::string& fname);


private:
	//!	helper method : import all messages from a class
	//! @return false if an error is encountered
	bool importClass(CRaptorIO& io,void* planguage);

	//!	helper method : import one messages from a class
	//! @return false if an error is encountered
	bool importMessage(CRaptorIO& io,void *pmessages);

    void *dBase;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORMESSAGES_H__55776166_2943_4D08_BFC8_65DFB74FD780__INCLUDED_)

