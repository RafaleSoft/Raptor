#if !defined(AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
#define AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_


#ifdef EXPORT_RAPTOR_NETWORK
	#define RAPTORNETWORK_API __declspec(dllexport)
#else
	#define RAPTORNETWORK_API __declspec(dllimport)
#endif


class RAPTORNETWORK_API CRaptorNetwork 
{
public:
	static const int PIXEL_SIZE;

	typedef struct BLOC_HEADER_t
	{
		unsigned short blocWidth;
		unsigned short blocHeight;
		unsigned short xOffset;
		unsigned short yOffset;
		unsigned short compressionType;
	} BLOC_HEADER;

	typedef struct BLOC_DATA_t
	{
		BLOC_HEADER		header;
		unsigned char	*pData;
	} BLOC_DATA;

	typedef struct SERVER_COMMAND_t
	{
		unsigned char	commandLen;
		unsigned char	replyLen;
		unsigned char	requestLen;
		char			command[13];
	} SERVER_COMMAND;
	typedef struct SESSION_COMMAND_t
	{
		SERVER_COMMAND	command;
		unsigned short width;
		unsigned short height;
	} SESSION_COMMAND;

	static const CRaptorNetwork::SERVER_COMMAND& getRenderCommand(void);
	static const CRaptorNetwork::SERVER_COMMAND& getStatusCommand(void);
	static const CRaptorNetwork::SESSION_COMMAND& getOpenSessionCommand(void);

private:
	CRaptorNetwork(void);
	virtual ~CRaptorNetwork(void);
};

#endif	// AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_

