#pragma once

#include <windows.h>
#include "..\Messages.h"

#include <string>
#include <vector>
using namespace std;

class CDeamonManager  
{
public:
	typedef struct WORKUNITSTRUCT
	{
		int					regID;		// work unit register index
		unsigned int		workUnitID; // outer identifier ( server counter )
		unsigned int		clientID;	// inner identifier of WU ( sock port )
		float				jobDone;	// percentage of job actually done
		void				*connection;// connection to the work unit
	} work_unit_struct_t;

	typedef WORKUNITSTRUCT* LPWORKUNITSTRUCT;

	typedef struct WU_reg_t
	{
		std::string		source;
		std::string		deamonIP;
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
	//void Run(void);
	//void RegisterDeamon(int numDeamon);
	//void DisconnectDeamon(CString sname, unsigned int port) const;

	//bool ActivateDeamon(unsigned int IPAddr) const;
	//bool DeActivateDeamon(unsigned int IPAddr) const;

	//bool InstallPlugin(	unsigned int IP,unsigned int port,unsigned int validDate,
	//					CString name,unsigned int size,unsigned char* plugin) const;

	CDeamonManager();
	virtual ~CDeamonManager();

	unsigned int getNbRegWorkUnits(void) const { return m_RegWorkUnits.size(); };
	
	const WORKUNITSTRUCT * const getWorkUnit(unsigned short WUID) const;
	const WUREGSTRUCT * const getRegWorkUnit(unsigned int numRegWU) const;

	//	Return false if WU update failed, true otherwize.
	bool setRegWUNbCPU(unsigned int nbWU,unsigned char nbCPU) const;
	//	Return false if WU update failed, true otherwize.
	bool setRegWUDeamon(unsigned int nbWU,const std::string& deamonIP) const;
	//	Return false if WU update failed, true otherwize.
	bool SetWUClient(unsigned short WUID,unsigned int clientID,void* connection) const;
	//	Return false if WU update failed, true otherwize.
	bool SetWUPercentage(unsigned short WUID,float percentage) const;

	//	Allocate workunits as requested and if possible.
	//	Workunits are based on the registered workunits model set
	//	Returns the number of workunits actually reserved
	//int AllocateWorkUnits(unsigned int requestedWU,const CDeamonManager::WORKUNITSTRUCT **&pListWU);

	//	Return true if workunit actually deleted, false otherwise.
	//bool DestroyWorkUnit(unsigned short WUID);

	//	A workunit used for a job has finished
	//	and then returns to the pool of available
	//	workunit for next job
	//bool ReleaseWorkUnit(unsigned short WUID);

	//	Return the created of incremented WU pointer :
	//	if WU already was registered, its nb procs is incremented,
	//	otherwise, the WU is created.
	//const CDeamonManager::WUREGSTRUCT * const CreateRegWorkUnit(CString wu,CString deamon);

	//	Return true if registered workunit successfuly created
	bool destroyRegWorkUnit(unsigned int numRegWU);

private:
	//friend UINT DeamonProcessor( LPVOID pParam );

	vector<LPWUREGSTRUCT>	m_RegWorkUnits;	// array of registered work units
	unsigned int			m_counter;		// unique work Unit ID counter

	//int* SelectWorkUnits(unsigned int requestedWU);
};
