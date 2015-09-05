// DeamonManager.h: interface for the CDeamonManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEAMONMANAGER_H__05C915AD_3AFE_48BD_918D_2B6975E9CB9E__INCLUDED_)
#define AFX_DEAMONMANAGER_H__05C915AD_3AFE_48BD_918D_2B6975E9CB9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaysServerApp;

class CDeamonManager  
{
public:
	typedef struct WORKUNITSTRUCT
	{
		int					regID;		// work unit register index
		unsigned int		workUnitID; // outer identifier ( server counter )
		unsigned int		clientID;	// inner identifier of WU ( sock port )
		PROCESS_INFORMATION	pi;			// process and thread IDs
		STARTUPINFO			si;			// startup process information
		float				jobDone;	// percentage of job actually done
		void				*connection;// connection to the work unit
	} work_unit_struct_t;

	typedef WORKUNITSTRUCT* LPWORKUNITSTRUCT;

	typedef struct WU_reg_t
	{
		CString			source;
		CString			deamonIP;
		unsigned char	nbProcs;
		unsigned char	available;
		bool			active;
		void			*connection;
	} WUREGSTRUCT;
	typedef WUREGSTRUCT* LPWUREGSTRUCT;

	typedef struct Message_reg_t
	{
		MSGSTRUCT		msg;
		unsigned char	*raw_data;
	} MSGREGSTRUCT;
	typedef MSGREGSTRUCT* LPMSGREGSTRUCT;



public:
	void Run(void);
	void RegisterDeamon(int numDeamon);
	void DisconnectDeamon(CString sname, unsigned int port) const;

	bool ActivateDeamon(unsigned int IPAddr) const;
	bool DeActivateDeamon(unsigned int IPAddr) const;

	bool InstallPlugin(	unsigned int IP,unsigned int port,unsigned int validDate,
						CString name,unsigned int size,unsigned char* plugin) const;

	CDeamonManager(CRaysServerApp *server);
	virtual ~CDeamonManager();

	int	GetNbWorkUnits(void) const { return m_WorkUnits.GetCount(); };
	int	GetNbRegWorkUnits(void) const { return m_RegWorkUnits.GetSize(); };
	
	const CDeamonManager::WORKUNITSTRUCT * const GetWorkUnit(unsigned short WUID) const;
	const CDeamonManager::WUREGSTRUCT * const GetRegWorkUnit(int numRegWU) const;

	//	Return false if WU update failed, true otherwize.
	bool SetRegWUNbCPU(unsigned int nbWU,unsigned char nbCPU) const;
	//	Return false if WU update failed, true otherwize.
	bool SetRegWUDeamon(unsigned int nbWU,CString deamonIP) const;
	//	Return false if WU update failed, true otherwize.
	bool SetWUClient(unsigned short WUID,unsigned int clientID,void* connection) const;
	//	Return false if WU update failed, true otherwize.
	bool SetWUPercentage(unsigned short WUID,float percentage) const;

	//	Allocate workunits as requested and if possible.
	//	Workunits are based on the registered workunits model set
	//	Returns the number of workunits actually reserved
	int AllocateWorkUnits(unsigned int requestedWU,const CDeamonManager::WORKUNITSTRUCT **&pListWU);

	//	Return true if workunit actually deleted, false otherwise.
	bool DestroyWorkUnit(unsigned short WUID);

	//	A workunit used for a job has finished
	//	and then returns to the pool of available
	//	workunit for next job
	bool ReleaseWorkUnit(unsigned short WUID);

	//	Return the created of incremented WU pointer :
	//	if WU already was registered, its nb procs is incremented,
	//	otherwise, the WU is created.
	const CDeamonManager::WUREGSTRUCT * const CreateRegWorkUnit(CString wu,CString deamon);

	//	Return true if registered workunit successfuly created
	bool DestroyRegWorkUnit(unsigned int numRegWU);

private:
	friend UINT DeamonProcessor( LPVOID pParam );

	CRaysServerApp	*m_ownerServer;
	CWinThread		*m_deamonPoller;

	//	TODO :
	//	Replace the CMapWordToPtr because it is not
	//	safe : the work unit counter may overflow as it is an int.
	//	Sol : save a 16bit modulo assuming that there cannot 
	//	reasonnably exist more than 65535 workunits at the same time.

	CMapWordToPtr		m_WorkUnits;	// for fast retrieval of a WU, given an ID
	CPtrArray			m_RegWorkUnits;	// array of registered work units
	unsigned int		m_counter;		// unique work Unit ID counter

	int* SelectWorkUnits(unsigned int requestedWU);
};

#endif // !defined(AFX_DEAMONMANAGER_H__05C915AD_3AFE_48BD_918D_2B6975E9CB9E__INCLUDED_)
