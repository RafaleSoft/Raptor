#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientDlg.h"
#include "MiniRaysClientStatus.h"


void CMiniRaysClientApp::Process_JOB_STATUS(MSGSTRUCT msg,unsigned char raw_data[])
{
	if (status != NULL)
	{
		time_t start = (time_t)msg.msg_data[1];
		time_t end = (time_t)msg.msg_data[2];

		CString id;
		id.Format("%d",jobID);

		struct tm *today = localtime( &start );
		CString st = asctime(today);
		st.SetAt(24,'\0');

		struct tm *finish = localtime( &end );
		CString et = asctime(finish);
		et.SetAt(24,'\0');

		CString p;
		p.Format("%f %%",((float)(msg.msg_data[4]))/100.0);

		status->SetCurrentJobInfo(id,st,et,p);

		if (msg.msg_size>0)
		{
			unsigned int offset = sizeof(unsigned int);
			unsigned int size;
			memcpy(&size,raw_data,sizeof(unsigned int));

			st.Format("There are %d available results with IDs:",size);
			output->Output(st);

			unsigned int *buffer = new unsigned int[size+1];
			buffer[0] = size;
			if (size > 0)
			{
				memcpy(&buffer[1],raw_data + offset,size * sizeof(unsigned int));
				for (unsigned int i=0;i<size;i++)
				{
					st.Format("     Job ID: %d",buffer[i+1]);
					output->Output(st);
					offset += sizeof(unsigned int);
				}
			}

			status->SetAvailableJobs(buffer);
			delete [] buffer;

			unsigned int nbTxt;
			memcpy(&nbTxt,raw_data + offset,sizeof(unsigned int));

			CStringArray strs;
			offset += sizeof(unsigned int);

			for (unsigned int i=0;i<nbTxt;i++)
			{
				CString str = CString(raw_data+offset);
				offset += str.GetLength() + 1;
				strs.Add(str);
			}
			status->SetAvailableTextures(strs);

			unsigned int nbPlug;
			memcpy(&nbPlug,raw_data + offset,sizeof(unsigned int));

			strs.RemoveAll();
			offset += sizeof(unsigned int);

			for (i=0;i<nbPlug;i++)
			{
				CString str = CString(raw_data+offset);
				offset += str.GetLength() + 1;
				strs.Add(str);
			}
			status->SetAvailablePlugins(strs);
		}
	}
}


void CMiniRaysClientApp::Process_ACKNOWLEDGE(MSGSTRUCT msg,unsigned char raw_data[])
{
	switch(msg.msg_id)
	{
		case ACK_JOB:
		{
			CString job;

			switch(msg.msg_data[1])
			{
				case JOB_RUN:
					job.Format("Job %d is beeing rendered...",jobID);
					break;
				case JOB_DELETE:
				{
					job.Format("Job %d has been deleted from server...",msg.msg_data[0]);
					QueryStatus();
					break;
				}
				default:
					job.Format("Job %d acknowledge unknown action !",msg.msg_data[0]);
					break;
			}
			output->Output(job);
			break;
		}
		case ACK_OBJ:
		{
			if ((config_sent) && (!config_ack))
			{
				config_ack = true;
				output->Output("Configuration object sent to server");
				analyser->SendPlugins(jobID);
				plugin_sent = true;
			}
			else if ((plugin_sent) && (!plugin_ack))
			{
				plugin_ack = true;
				output->Output("Plugins object sent to server");
				analyser->SendLights(jobID);
				light_sent = true;
			}
			else if ((light_sent) && (!light_ack))
			{
				light_ack = true;
				output->Output("Lights sent to server");
				analyser->SendSpheres(jobID);
				sphere_sent = true;
			}
			else if ((sphere_sent) && (!sphere_ack))
			{
				sphere_ack = true;
				output->Output("Spheres sent to server");
				analyser->SendCompositions(jobID);
				composition_sent = true;
			}
			else if ((composition_sent) && (!composition_ack))
			{
				composition_ack = true;
				output->Output("Compositions sent to server");
				analyser->SendFrames(jobID);
				frame_sent = true;
			}
			else if ((frame_sent) && (!frame_ack))
			{
				frame_ack = true;
				output->Output("Frames sent to server");
				analyser->RunJob(jobID);
			}
			else
				output->Output("Failed to send objects to server");

			break;
		}
		case JOB_ID:
		{
			jobID = msg.msg_data[0];
		
			CString id;
			id.Format("Server job ID is: %d",jobID);
			output->Output(id);

			config_sent = true;
			analyser->SendConfiguration(jobID);

			break;
		}
		case ACK_IMG:
		{
			switch(msg.msg_data[4])
			{
				case IMG_DATA:
				{
					if (image != NULL)
						delete [] image;

					image = new unsigned char[msg.msg_size];
					image_size = msg.msg_size;
					image_width = msg.msg_data[0];
					image_height = msg.msg_data[1];

					memcpy(image,raw_data,msg.msg_size);
			
					output->Output("Image data received !");

					CFileDialog *f;
					DWORD flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES ;
	
					f = new CFileDialog(FALSE,"*.tga",NULL, flags, "TGA Image Files (*.tga)|*.tga||",output );

					if (IDOK == f->DoModal())
					{
						if (Savejob(f->GetPathName()))
							output->Output("Job saved.");
					}

					delete f;		 
					break;
				}
				case OBJ_TEXTURE:
				{
					output->Output("Rays2 Texture file: " + upload + " uploaded on Rays2Server");
					break;
				}
				case OBJ_PLUGIN:
				{
					output->Output("Rays2 Plugin file: " + upload + " uploaded on Rays2Server");
					break;
				}
			}
			break;
		}
		case ACK_NONE:
		{
			CString err;

			err.Format("You are not the owner of job %d or this job does not exist",msg.msg_data[0]);
			output->Output(err);

			break;
		}
		default:
			output->Output("Unsupported acknowledge message");
			break;
	}
}