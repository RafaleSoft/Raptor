#include "stdafx.h"
#include "resource.h"
#include "RaysServer.h"
#include "RaysServerDlg.h"

extern CRaysServerApp Server;

void CRaysServerDlg::JobDraw(CPaintDC *dc,bool Redraw)
{
	//	Ressources must be locked to 
	//	assert concurrent access cannot happen
	Server.processLock.Lock();

	if (Redraw)
	{
		CString st;

		dc->SelectStockObject(GRAY_BRUSH);
		dc->Rectangle(100,80,460,180);
		dc->SelectObject(black_pen);	
		dc->MoveTo(100,80);
		dc->LineTo(100,180);

		dc->SetBkMode(TRANSPARENT);			//	Tansparent background for cool text display
		dc->SelectObject(font);				//	Set font

		for (int i=1;i<11;i++)
		{
			st.Format("%d",10*i);
			dc->TextOut(80,178-i*10,st);
			dc->MoveTo(98,180-i*10);
			dc->LineTo(460,180-i*10);
		}
	}

	// drawing current job
	if (Server.m_Jobs.GetSize()>0)
	{
		char buffer[20];
		wJobID->GetWindowText(buffer,20);
		int nJob = atoi(buffer);
		int maxJob = Server.m_Jobs.GetSize();
		if ((nJob>=maxJob)&&(maxJob>0))
		{
			nJob = maxJob-1;
			SetDlgItemInt(IDC_JOBID,nJob);
		}

		LPJOBSTRUCT lpJob = (LPJOBSTRUCT)(Server.m_Jobs.GetAt(nJob));
		int nWU = lpJob->nbWorkUnits,mnWU;
		if (nWU>5) mnWU = 5; else mnWU = nWU;
		float percent,gpercent=0.0f;
		
		dc->SelectObject(blue_pen);
		dc->SelectObject(green_brush);
		for (int i=0;i<mnWU;i++)
		{
			percent = lpJob->workUnits[i]->jobDone;
			if (percent == 100)
			{
				dc->SelectObject(blue_brush);
				dc->Rectangle(110+i*60,180-(int)(percent),160+i*60,180);
				dc->SelectObject(green_brush);
			}
			else
				dc->Rectangle(110+i*60,180-(int)(percent),160+i*60,180);
		}

		for (i=0;i<nWU;i++)
			gpercent+=lpJob->workUnits[i]->jobDone;

		sprintf(buffer,"%.2f",gpercent/nWU);
		wJobPercent->SetWindowText(buffer);
	}

	// draw global job done
	float gpercent=0.0f;
	int nb=0;
	CString text;

	dc->SetBkMode(TRANSPARENT);			//	Tansparent background for cool text display
	dc->SetTextColor(RGB(255,0,0));
	dc->SelectObject(font);				//	Set font

	for (int i=0;i<Server.m_Jobs.GetSize();i++)
	{
		LPJOBSTRUCT lpJob = (LPJOBSTRUCT)(Server.m_Jobs.GetAt(i));
		for (unsigned int j=0;j<lpJob->nbWorkUnits;j++,nb++)
			gpercent+=lpJob->workUnits[j]->jobDone;
	}
	CProgressCtrl *progress = (CProgressCtrl *)(this->GetDlgItem(IDC_PROGRESS));
	CRect Rect;
	progress->GetClientRect(&Rect);
	progress->InvalidateRect(&Rect,true);
	progress->SetPos((int)(100.0f*gpercent/nb));	
	text.Format("%.2f%%",gpercent/nb);
	dc->TextOut(230,205,text);

	Server.processLock.Unlock();
}