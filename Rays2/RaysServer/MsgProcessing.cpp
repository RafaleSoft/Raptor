#include "stdafx.h"
#include "RaysServer.h"
#include "RaysServerDlg.h"
#include "MsgManager.h"
#include "DeamonSocket.h"
#include "FileManager.h"
#include "PostProcessor.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Thread messages processing
//
//
void CRaysServerApp::Process_THREAD_MSG(UINT wParam,LONG lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	CDeamonManager::LPMSGREGSTRUCT regMsg = m_msgManager->getRegMsg();

	CString msgStr;
	msgStr.LoadString(IDS_SERVER_THREAD_MSG);
	ADD_MSG_STRING(msgStr,regMsg->msg.msg_id);

	WriteMessage(IDS_SERVER_STRING,msgStr);

	ProcessMsg(regMsg->msg,regMsg->raw_data);

	m_msgEvent->SetEvent();
}


void CRaysServerApp::Process_DEAMON_MSG(UINT wParam,LONG lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	
	CString msgStr;
	msgStr.LoadString(IDS_DEAMON_THREAD_MSG);
	
	WriteMessage(IDS_SERVER_STRING,msgStr);

	m_deamonManager->RegisterDeamon((int)(lParam));
}

//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//
//	Main messages
//
void CRaysServerApp::Process_JOB_STOP(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	const CDeamonManager::WORKUNITSTRUCT * const lpWU = 
		m_deamonManager->GetWorkUnit((unsigned short)msg.msg_data[0]);

	if (NULL != lpWU)
	{
		msgStr.Format("Work unit (%d) job complete",msg.msg_data[0]);
		WriteMessage(IDS_WORKUNIT_STRING,msgStr);

		m_deamonManager->ReleaseWorkUnit(lpWU->workUnitID);

		//	test to finish job if it was the last WU
		bool jobFound = false;
		int nJob = 0;
		unsigned int nWu = 0;
		LPJOBSTRUCT job = NULL;
		CDeamonManager::LPWORKUNITSTRUCT wu = NULL;

		while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
		{
			job = (LPJOBSTRUCT)m_Jobs[nJob++];
			nWu = 0;
			while ((!jobFound)&&(nWu<job->nbWorkUnits))
			{
				wu = (CDeamonManager::LPWORKUNITSTRUCT)job->workUnits[nWu++];
				if (wu != NULL)
					jobFound = (wu->workUnitID == msg.msg_data[0]);
			}
		}

		if (!jobFound)
		{
			msgStr.Format("Work unit (%d) lost by server",msg.msg_data[0]);
			WriteMessage(IDS_SERVER_STRING,msgStr);
		}
		else
		{
			// receive image & z-buffer
			int pos = msg.msg_data[1]*job->jobWidth*4;
			msgStr.Format("Receiving Work unit (%d) job",msg.msg_data[0]);
			WriteMessage(IDS_SERVER_STRING,msgStr);
			
			unsigned int sz = msg.msg_size / 2;
			memcpy(job->globalImageBuffer+pos,raw_data,sz);
			memcpy(job->ZBuffer+pos,raw_data + sz,sz);

			job->nbWorkUnitsReady--;
			
			if (job->nbWorkUnitsReady == 0)
			{
				time(&job->jobEnd);

				msgStr.Format("Job %d (%d on port %d) is complete!",job->clientID,job->jobID,job->clientPort);
				WriteMessage(IDS_SERVER_STRING,msgStr);

				//m_FinishedJobs.Add(job);
				m_Jobs.RemoveAt(nJob-1);
				WND->SetNbJobs(m_Jobs.GetSize());

				job->processor->Run();
			}
		}
	}
	else
	{
		msgStr.Format("Work unit (%d) data : ID Lost !",msg.msg_data[0]);
		WriteMessage(IDS_WORKUNIT_STRING,msgStr);
	}
}


void CRaysServerApp::Process_JOB_STATUS(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	LPJOBSTRUCT			lpJob = NULL;
	iosock_t<CSSocket>	*connection = NULL;
	bool				jobFound = false;
	int					nJob = 0;
	CDWordArray			jobIDs;

	unsigned int		*buffer = NULL;

	MSGSTRUCT rmsg;
	rmsg.msg_header = MSG_START;
	rmsg.msg_size = 0;
	rmsg.msg_id = JOB_STATUS;
	rmsg.msg_tail = MSG_DATA;
	rmsg.msg_data[0] = msg.msg_data[0];
	rmsg.msg_data[1] = 0;
	rmsg.msg_data[2] = 0;
	rmsg.msg_data[3] = 0;
	rmsg.msg_data[4] = 0;

	// search for available job results
	for (int i=0;i<m_FinishedJobs.GetSize();i++)
	{
		lpJob = (LPJOBSTRUCT)m_FinishedJobs[i];
		connection = (iosock_t<CSSocket> *)lpJob->connection;

		if (
			(msg.msg_data[3] == connection->Port()) && 
			(msg.msg_data[4] == connection->IP())
			)
			jobIDs.Add(lpJob->jobID);
	}
	//jobFound = jobIDs.GetSize() > 0;

	// search in active jobs
	while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
	{
		LPJOBSTRUCT lpJob2 = (LPJOBSTRUCT)m_Jobs[nJob++];
		jobFound = (lpJob2->jobID == msg.msg_data[0]);
		if (jobFound)
			lpJob = lpJob2;
	}

	if (!jobFound)
		jobFound = jobIDs.GetSize() > 0;
	
	// search in completed jobs
	//nJob = 0;
	//if (!jobFound) while ((!jobFound)&&(nJob<m_FinishedJobs.GetSize()))
	//{
	//	lpJob = (LPJOBSTRUCT)m_FinishedJobs[nJob++];
	//	jobFound = (lpJob->jobID == msg.msg_data[0]);
	//}

	if (jobFound)
	{
		if (lpJob->connection == NULL)
		{
			msgStr.Format("Job (%d) connection lost !",msg.msg_data[0]);
			WriteMessage(IDS_SERVER_STRING,msgStr);
		}
		else
		{
			float percent=0.0;
			time_t now;
			int nWU = lpJob->nbWorkUnits;
			for (int i=0;i<nWU;i++)
				percent+=lpJob->workUnits[i]->jobDone;

			
			rmsg.msg_size = (jobIDs.GetSize() + 1)*sizeof(unsigned int);
			rmsg.msg_data[1] = lpJob->jobStart;
			rmsg.msg_data[3] = jobIDs.GetSize();
			rmsg.msg_data[4] = (int)(100*percent/nWU);
			buffer = new unsigned int[jobIDs.GetSize() + 1];

			buffer[0] = jobIDs.GetSize();
			for (i=0;i<jobIDs.GetSize();i++)
				buffer[i+1] = jobIDs[i];

			time(&now);
			if (percent>0)
			{
				if (percent<100*nWU)
				{
					int usedTime = ((now-lpJob->jobStart)*10000)/rmsg.msg_data[4];
					rmsg.msg_data[2] = usedTime + lpJob->jobStart;
				}
				else
					rmsg.msg_data[2] = lpJob->jobEnd;
			}
			else
				rmsg.msg_data[2] = lpJob->jobStart;

			connection = (iosock_t<CSSocket> *)lpJob->connection;
		}
	}
	else
	{
		msgStr.Format("Job session (%d) not found !",msg.msg_data[0]);
		WriteMessage(IDS_SERVER_STRING,msgStr);		

		buffer = new unsigned int[1];
		buffer[0] = 0;
		rmsg.msg_size = 4;
		connection = UseNewConnection(msg.msg_data[4],msg.msg_data[3],false);
	}

	if (connection != NULL)
	{
		int buf_size = rmsg.msg_size;
		int txt_size = 0;
		int nbTxt = m_fileManager->GetNbFiles(CFileManager::TEXTURE);

		CString tname;
		CStringArray tnames;

		for (int i=0;i<nbTxt;i++)
		{
			tname = (m_fileManager->GetFile(i,CFileManager::TEXTURE))->name;
			tnames.Add(tname);
			txt_size += tname.GetLength() + 1;
		}

		rmsg.msg_size += txt_size + sizeof(unsigned int);

		int plg_size = 0;
		int nbPlug = m_fileManager->GetNbFiles(CFileManager::PLUGIN);

		CString pname;
		CStringArray pnames;

		for (i=0;i<nbPlug;i++)
		{
			pname = (m_fileManager->GetFile(i,CFileManager::PLUGIN))->name;
			pnames.Add(pname);
			plg_size += pname.GetLength() + 1;
		}

		rmsg.msg_size += plg_size + sizeof(unsigned int);


		connection->Write(&rmsg,MSGSIZE);
		if (rmsg.msg_size>0)
		{
			connection->Write(buffer,buf_size);
			connection->Write(&nbTxt,sizeof(unsigned int));

			for (int i=0;i<tnames.GetSize();i++)
			{
				tname = tnames[i];
				connection->Write((void*)LPCTSTR(tname),tname.GetLength() + 1);
			}

			connection->Write(&nbPlug,sizeof(unsigned int));

			for (i=0;i<pnames.GetSize();i++)
			{
				pname = pnames[i];
				connection->Write((void*)LPCTSTR(pname),pname.GetLength() + 1);
			}
		}

		msgStr.Format("Job session (%d) status sent !",msg.msg_data[0]);
		WriteMessage(IDS_SERVER_STRING,msgStr);

		if (buffer != NULL)	delete [] buffer;
	}
}


void CRaysServerApp::Process_JOB_WUNIT(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	msgStr.Format("Work unit (%d) identification",msg.msg_data[0]);
	{
		const CDeamonManager::WORKUNITSTRUCT * const lpWU = 
		m_deamonManager->GetWorkUnit((unsigned short)msg.msg_data[0]);

		if (NULL != lpWU)
		{
			void *connection = UseNewConnection(msg.msg_data[4],msg.msg_data[3]);
			m_deamonManager->SetWUClient(msg.msg_data[0],msg.msg_data[3],connection);

			LPJOBSTRUCT			lpJob = NULL;
			CDeamonManager::LPWORKUNITSTRUCT	wu;
			bool				jobFound = false;
			int					nJob = 0;	

			while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
			{
				lpJob = (LPJOBSTRUCT)m_Jobs[nJob++];
				unsigned int nWu = 0;
				while ((!jobFound)&&(nWu<lpJob->nbWorkUnits))
				{
					wu = (CDeamonManager::LPWORKUNITSTRUCT)lpJob->workUnits[nWu++];
					if (wu != NULL)
						jobFound = (wu->workUnitID == msg.msg_data[0]);
				}
			}

			if (jobFound)
			{
				lpJob->nbWorkUnitsReady++;
				if (lpJob->nbWorkUnitsReady == lpJob->nbWorkUnits)
				{
					MSGSTRUCT rmsg;
					rmsg.msg_header = MSG_START;
					rmsg.msg_id = JOB_ID;
					rmsg.msg_size = 0;
					rmsg.msg_tail = MSG_END;
					rmsg.msg_data[0] = lpJob->jobID;
					((iosock_t<CSSocket>*)lpJob->connection)->Write(&rmsg,MSGSIZE);
					msgStr+=" : job Ready";
				}
			}
			else
				msgStr+=" : ID start Lost!";
		}
		else
			msgStr+=" : ID Work Unit Lost!";
	}
	WriteMessage(IDS_WORKUNIT_STRING,msgStr);
}


void CRaysServerApp::Process_JOB_START(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	// beginning job creation process
	msgStr="Creating a new job";
		
	LPJOBSTRUCT lpJob = new JOBSTRUCT;
	
	//char buffer[64];
	char WUID[16];

	// create many work units, according nbprocs & frequency
	lpJob->processedAcks = 0;
	lpJob->nbWorkUnitsReady = 0;

	// searching available work units
	lpJob->nbWorkUnits = m_deamonManager->AllocateWorkUnits(m_nbWUperJOB,lpJob->workUnits);

	if (NULL == lpJob->workUnits)
	{
		msgStr="No more registered work units available";
		WriteMessage(IDS_SERVER_STRING,msgStr);
		delete lpJob;
		return;
	}
	else


	//	configure job settings.
	//	internal errors souldn't happen in 'UseNewConnection'
	//	but NULL result should be tested to avoid parameter errors
	WND->SetNbJobs(m_Jobs.GetSize());
	lpJob->jobID = m_counter++;
	lpJob->clientID = msg.msg_data[0];
	lpJob->clientPort = msg.msg_data[3];
	lpJob->jobWidth = msg.msg_data[1];
	lpJob->jobHeight = msg.msg_data[2];
	lpJob->connection = UseNewConnection(msg.msg_data[4],msg.msg_data[3]);

	//	create job data storage
	//	image is a 32 bits RGBA buffer
	unsigned char *imageBuffer = new unsigned char[msg.msg_data[1]*msg.msg_data[2]*4];
	float *zBuffer = new float[msg.msg_data[1]*msg.msg_data[2]];
	lpJob->globalImageBuffer = imageBuffer;
	lpJob->ZBuffer = zBuffer;
	lpJob->processor = new CPostProcessor(lpJob);

	for (unsigned int i=0;i<lpJob->nbWorkUnits;i++)
	{
		const CDeamonManager::WUREGSTRUCT * const lpWUReg = m_deamonManager->GetRegWorkUnit(lpJob->workUnits[i]->regID);
		sprintf(WUID,"%d %d",lpJob->workUnits[i]->workUnitID,m_Server->GetPort());

		int size = lpWUReg->source.GetLength() + 1;
		unsigned char *buffer = new unsigned char[size + MSGSIZE];
		MSGSTRUCT msg;

		msg.msg_header = MSG_START;
		msg.msg_id = DMN_DISPATCHJOB;
		msg.msg_size = size;
		msg.msg_tail = MSG_DATA;
		msg.msg_data[0] = lpJob->workUnits[i]->workUnitID;
		msg.msg_data[1] = lpWUReg->nbProcs;
		msg.msg_data[2] = m_Server->GetAddr();
		msg.msg_data[3] = m_Server->GetPort();
		msg.msg_data[4] = m_wUnitPriority;

		memcpy(buffer,&msg,MSGSIZE);
		memcpy(buffer+MSGSIZE,LPCTSTR(lpWUReg->source),size);

		CClient<CDeamonSocket>* deamon = (CClient<CDeamonSocket>*)(lpWUReg->connection);
		deamon->Write(buffer,size+MSGSIZE);
	}

	m_Jobs.Add(lpJob);
	WND->SetNbJobs(m_Jobs.GetSize());

	WriteMessage(IDS_CLIENT_STRING,msgStr);
}


void CRaysServerApp::Process_OBJ_DATA(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString msgStr;

	bool jobFound = false;
	int nJob = 0;
	LPJOBSTRUCT job = NULL;
	CDeamonManager::LPWORKUNITSTRUCT wu = NULL;

	while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
	{
		job = (LPJOBSTRUCT)m_Jobs[nJob++];
		jobFound = (job->jobID == msg.msg_data[0]);
	}

	if (!jobFound)
	{
		msgStr.Format("Job ID (%d) lost by server",msg.msg_data[0]);
		WriteMessage(IDS_CLIENT_STRING,msgStr);
	}
	else
	{
		job->processor->AddData(msg,raw_data,m_fileManager);

		for (unsigned int i=0;i<job->nbWorkUnits;i++)
		{
			wu = (CDeamonManager::LPWORKUNITSTRUCT)(job->workUnits[i]);
			if (wu->connection != NULL)
			{
				msgStr.Format("Job ID (%d): sending data to work unit %d...",msg.msg_data[0],wu->workUnitID);
				WriteMessage(IDS_CLIENT_STRING,msgStr);

				iosock_t<CSSocket> *connection = (iosock_t<CSSocket> *)wu->connection;
				connection->Write(&msg,MSGSIZE);
				connection->Write(raw_data,msg.msg_size);
			}
			else
			{
				msgStr.Format("Work unit (%d) connection lost by server",wu->workUnitID);
				WriteMessage(IDS_SERVER_STRING,msgStr);
			}
		}
	}
}


void CRaysServerApp::Process_ACK(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	bool jobFound = false;
	int nJob = 0;
	LPJOBSTRUCT job = NULL;

	while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
	{
		job = (LPJOBSTRUCT)m_Jobs[nJob++];
		jobFound = (job->jobID == msg.msg_data[0]);
	}

	if (!jobFound)
	{
		msgStr.Format("Job ID (%d) lost by server",msg.msg_data[0]);
		WriteMessage(IDS_SERVER_STRING,msgStr);
	}
	else
	{
		job->processedAcks++;
		msgStr.Format("acknowledement %d/%d ",job->processedAcks,job->nbWorkUnits);
		WriteMessage(IDS_SERVER_STRING,msgStr);

		if (job->processedAcks == job->nbWorkUnits)
		{
			job->processedAcks = 0;

			((iosock_t<CSSocket> *)job->connection)->Write(&msg,MSGSIZE);

			msgStr.Format("Sending acknowledement to client (%d) ",job->jobID);
			WriteMessage(IDS_SERVER_STRING,msgStr);
		}
	}
}


void CRaysServerApp::Process_JOB_RUN(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	bool jobFound = false;
	int nJob = 0;
	LPJOBSTRUCT job = NULL;
	CDeamonManager::LPWORKUNITSTRUCT wu = NULL;

	while ((!jobFound)&&(nJob<m_Jobs.GetSize()))
	{
		job = (LPJOBSTRUCT)m_Jobs[nJob++];
		jobFound = (job->jobID == msg.msg_data[0]);
	}

	if (!jobFound)
	{
		msgStr.Format("Job ID (%d) lost by server",msg.msg_data[0]);
		WriteMessage(IDS_CLIENT_STRING,msgStr);
	}
	else
	{
		unsigned int nb = job->nbWorkUnits;
		int curScanLine = 0;
		int stepScanLine = job->jobHeight / job->nbWorkUnits;

		for (unsigned int i=nb-1;i<nb;i--)
		{
			msg.msg_data[1] = curScanLine;
			curScanLine+=stepScanLine;
			if (i==0) curScanLine = job->jobHeight;
			msg.msg_data[2] = curScanLine;

			wu = (CDeamonManager::LPWORKUNITSTRUCT)(job->workUnits[i]);
			if (wu->connection != NULL)
			{
				msgStr.Format("Job ID (%d): launching work unit %d...",msg.msg_data[0],wu->workUnitID);
				WriteMessage(IDS_CLIENT_STRING,msgStr);

				((iosock_t<CSSocket> *)wu->connection)->Write(&msg,MSGSIZE);
			}
			else
			{
				msgStr.Format("Work unit (%d) connection lost by server",wu->workUnitID);
				WriteMessage(IDS_SERVER_STRING,msgStr);
			}
		}
		time(&job->jobStart);
		job->jobEnd = job->jobStart;
	}
}


void CRaysServerApp::Process_JOB_PERCENT(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	if (m_deamonManager->SetWUPercentage((unsigned short)msg.msg_data[0],((float)msg.msg_data[4])/100.0f))
	{
		msgStr.Format("Work unit (%d) job percentage",msg.msg_data[0]);
		WriteMessage(IDS_WORKUNIT_STRING,msgStr);
	}
	else
	{
		msgStr.Format("Work unit (%d) data : ID Lost !",msg.msg_data[0]);
		WriteMessage(IDS_WORKUNIT_STRING,msgStr);
	}
}

void CRaysServerApp::Process_JOB_REQUEST(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);
	CString msgStr;

	bool jobFound = false;
	int nJob = 0;
	LPJOBSTRUCT job = NULL;
	CDeamonManager::LPWORKUNITSTRUCT lpWUnit = NULL;

	iosock_t<CSSocket> *connection = NULL;
	iosock_t<CSSocket> *validConnection = NULL;

	while ((!jobFound)&&(nJob<m_FinishedJobs.GetSize()))
	{
		job = (LPJOBSTRUCT)m_FinishedJobs[nJob++];

		connection = (iosock_t<CSSocket> *)job->connection;
		if (
			(msg.msg_data[3] == connection->Port()) && 
			(msg.msg_data[4] == connection->IP())
			)
		{
			validConnection = connection;
			jobFound = (job->jobID == msg.msg_data[0]);
		}
	}

	//	job is found and jobID is owner
	if (jobFound)
	{
		switch (msg.msg_data[1])
		{
			case IMG_DATA:
			{
				msgStr.Format("Sending image data of Job (%d)...",msg.msg_data[0]);
				WriteMessage(IDS_WORKUNIT_STRING,msgStr);
				msgStr.Format("Image data of job (%d) sent!",msg.msg_data[0]);

				//	image is a 32 bits RGBA buffer
				int totalsize = job->jobWidth * job->jobHeight * 4;
				int pos = 0;	
				int nbp = totalsize/PACKET_SIZE;

				//	sending image message first
				msg.msg_id = ACK_IMG;
				msg.msg_size = totalsize;
				msg.msg_tail = MSG_DATA;
				msg.msg_data[0] = job->jobWidth;
				msg.msg_data[1] = job->jobHeight;
				msg.msg_data[4] = IMG_DATA;
				connection->Write(&msg,MSGSIZE);

				connection->Write(job->globalImageBuffer,totalsize);
			
				WriteMessage(IDS_WORKUNIT_STRING,msgStr);
			}
			break;
			case JOB_DELETE:
			{
				msgStr.Format("Deleting image data of Job (%d)...",msg.msg_data[0]);
				WriteMessage(IDS_WORKUNIT_STRING,msgStr);

				for (unsigned int j=0;j<job->nbWorkUnits;j++)
					m_deamonManager->DestroyWorkUnit(job->workUnits[j]->workUnitID);

				delete [] job->workUnits;
				delete [] job->globalImageBuffer;
				delete [] job->ZBuffer;
				delete job->processor;
		
				//	sending image message first
				msg.msg_id = ACK_JOB;
				msg.msg_data[1] = JOB_DELETE;
				msg.msg_size = 0;
				msg.msg_tail = MSG_END;
				connection->Write(&msg,MSGSIZE);

				delete job;
				m_FinishedJobs.RemoveAt(nJob-1);
			}
			break;
		}
	}
	else if (validConnection != NULL)
	{
		msg.msg_id = ACK_NONE;
		validConnection->Write(&msg,MSGSIZE);
	}
}


void CRaysServerApp::Process_DMN_ACTIVE(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);

	int b1 = ((msg.msg_data[4] & 0xFF000000) >> 24);
	int b2 = ((msg.msg_data[4] & 0x00FF0000) >> 16);
	int b3 = ((msg.msg_data[4] & 0x0000FF00) >> 8);
	int b4 = ((msg.msg_data[4] & 0x000000FF));

	CString msgStr;

	if (m_deamonManager->ActivateDeamon(msg.msg_data[4]))
		msgStr.Format("Deamon at host %d.%d.%d.%d is ACTIVE",b1,b2,b3,b4);
	else
		msgStr.Format("Deamon at host %d.%d.%d.%d is NOT REGISTERED",b1,b2,b3,b4);
	
	WriteMessage(IDS_DEAMON_STRING,msgStr);
}

void CRaysServerApp::Process_DMN_INACTIVE(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);

	int b1 = ((msg.msg_data[4] & 0xFF000000) >> 24);
	int b2 = ((msg.msg_data[4] & 0x00FF0000) >> 16);
	int b3 = ((msg.msg_data[4] & 0x0000FF00) >> 8);
	int b4 = ((msg.msg_data[4] & 0x000000FF));

	CString msgStr;

	if (m_deamonManager->DeActivateDeamon(msg.msg_data[4]))
		msgStr.Format("Deamon at host %d.%d.%d.%d is INACTIVE",b1,b2,b3,b4);
	else
		msgStr.Format("Deamon at host %d.%d.%d.%d is NOT REGISTERED",b1,b2,b3,b4);
	
	WriteMessage(IDS_DEAMON_STRING,msgStr);
}

void CRaysServerApp::Process_DMN_DISPATCHJOB(MSGSTRUCT& msg,unsigned char raw_data[])
{
	UNREFERENCED_PARAMETER(raw_data);

	CString msgStr;
	msgStr.Format("Deamon started WU(%d) ...",msg.msg_data[0]);
	WriteMessage(IDS_DEAMON_STRING,msgStr);
}


void CRaysServerApp::Process_OBJ_TEXTURE(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString msgStr;
	int offset = MSGSIZE;
	int len = msg.msg_data[2];
	char *tname = new char[len+1];

	memcpy(tname,&raw_data[0],len);
	tname[len] = 0;

	msgStr.Format("Client uploading texture %s...",tname);
	WriteMessage(IDS_CLIENT_STRING,msgStr);


	// search for available job results
	bool found = false;
	int nbClients = m_Server->GetNumClients();
	int i = 0;
	iosock_t<CSSocket> *connection;

	while ((i<nbClients) && (!found))
	{
		m_Server->GetClient(i,connection);

		if (
			(msg.msg_data[3] == connection->Port()) && 
			(msg.msg_data[4] == connection->IP())
			)
			found = true;
		i++;
	}

	bool res = m_fileManager->CreateFile(CString(tname),CFileManager::TEXTURE,4*msg.msg_data[0],msg.msg_data[1],(void*)(&raw_data[len]));

	//	sending image message first
	if (found)
	{
		MSGSTRUCT msgack;
		msgack.msg_header = MSG_START;
		msgack.msg_data[4] = OBJ_TEXTURE;
		if (res)
			msgack.msg_id = ACK_IMG;
		else
			msgack.msg_id = ACK_NONE;
		msgack.msg_size = 0;
		msgack.msg_tail = MSG_END;
		connection->Write(&msgack,MSGSIZE);
	}

	delete [] tname;
}

void CRaysServerApp::Process_OBJ_PLUGIN(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString msgStr;
	int offset = MSGSIZE;
	int len = msg.msg_data[2];
	char *tname = new char[len+1];

	memcpy(tname,&raw_data[0],len);
	tname[len] = 0;

	msgStr.Format("Client uploading plugin %s...",tname);
	WriteMessage(IDS_CLIENT_STRING,msgStr);


	// search for available job results
	bool found = false;
	int nbClients = m_Server->GetNumClients();
	int i = 0;
	iosock_t<CSSocket> *connection;

	while ((i<nbClients) && (!found))
	{
		m_Server->GetClient(i,connection);

		if (
			(msg.msg_data[3] == connection->Port()) && 
			(msg.msg_data[4] == connection->IP())
			)
			found = true;
		i++;
	}

	bool res = false;
	CFileManager::LPRAYSFILE plugin = m_fileManager->GetFile(CString(tname));
	if ( plugin != NULL)
	{
		if (msg.msg_data[1] > plugin->fileDate)
		{
			msgStr.Format("Server replacing old plugin with new version %s...",tname);
			WriteMessage(IDS_SERVER_STRING,msgStr);
			m_fileManager->RemoveFile(CString(tname));

			res = m_fileManager->CreateFile(CString(tname),CFileManager::PLUGIN,msg.msg_data[0],(void*)(&raw_data[len]));
		}
		else
			res = true;
	}
	else
		res = m_fileManager->CreateFile(CString(tname),CFileManager::PLUGIN,msg.msg_data[0],(void*)(&raw_data[len]));



	//	sending image message first
	if (found)
	{
		MSGSTRUCT msgack;
		msgack.msg_header = MSG_START;
		msgack.msg_data[4] = OBJ_PLUGIN;
		if (res)
			msgack.msg_id = ACK_IMG;
		else
			msgack.msg_id = ACK_NONE;
		msgack.msg_size = 0;
		msgack.msg_tail = MSG_END;
		connection->Write(&msgack,MSGSIZE);
	}

	delete [] tname;
}

void CRaysServerApp::Process_IMG_REQUEST(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString msgStr;
	int offset = MSGSIZE;
	int len = msg.msg_size;
	char *tname = new char[len+1];

	memcpy(tname,&raw_data[0],len);
	tname[len] = 0;

	msgStr.Format("Work unit downloading data : %s...",tname);
	WriteMessage(IDS_WORKUNIT_STRING,msgStr);

	// search for available work unit connections
	bool found = false;
	int nbClients = m_Server->GetNumClients();
	int i = 0;
	iosock_t<CSSocket> *connection;

	while ((i<nbClients) && (!found))
	{
		m_Server->GetClient(i,connection);

		if (
			(msg.msg_data[3] == connection->Port()) && 
			(msg.msg_data[4] == connection->IP())
			)
			found = true;
		i++;
	}

	//	sending image message first
	if (found)
	{
		MSGSTRUCT msgack;
		msgack.msg_header = MSG_START;

		//	Downloading a texture
		if (msg.msg_data[0] == OBJ_TEXTURE)
		{
			unsigned int image_width;
			unsigned int image_height;
			unsigned char *buffer = NULL;
			bool res = m_fileManager->GetFile(CString(tname),image_width,image_height,buffer);

			if (res)
			{
				int image_size = image_width * image_height;
				msgack.msg_id = OBJ_TEXTURE;
				msgack.msg_tail = MSG_DATA;
				msgack.msg_size = image_size + len;
				msgack.msg_data[0] = image_width / 4;
				msgack.msg_data[1] = image_height;
				msgack.msg_data[2] = len;
				msgack.msg_data[3] = msg.msg_data[3];
				msgack.msg_data[4] = msg.msg_data[4];

				unsigned char *image_buffer = new unsigned char[offset + image_size + len];
				memcpy(image_buffer,&msgack,offset);
				memcpy(image_buffer + offset,tname,len);
				memcpy(image_buffer + offset + len,buffer,image_size);

				connection->Write(image_buffer,offset + image_size + len);

				delete [] image_buffer;
				delete [] buffer;
			}
			else
			{
				msgack.msg_id = ACK_NONE;
				msgack.msg_tail = MSG_END;
				msgack.msg_size = 0;
				msgack.msg_data[0] = OBJ_TEXTURE;
				connection->Write(&msgack,offset);
			}
		}
		else if (msg.msg_data[0] == OBJ_PLUGIN)
		{
			if (!InstallPlugin(CString(tname),connection))
			{
				msgack.msg_id = ACK_NONE;
				msgack.msg_tail = MSG_END;
				msgack.msg_size = 0;
				msgack.msg_data[0] = OBJ_PLUGIN;
				connection->Write(&msgack,offset);
			}
		}
		else
		{
			msgack.msg_id = ACK_NONE;
			msgack.msg_tail = MSG_END;
			msgack.msg_size = 0;
			connection->Write(&msgack,offset);
		}
	}

	msgStr = "Work unit download complete";
	WriteMessage(IDS_WORKUNIT_STRING,msgStr);
}
