; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMiniRaysClientDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MiniRaysClient.h"

ClassCount=5
Class1=CMiniRaysClientApp
Class2=CMiniRaysClientDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MINIRAYSCLIENT_DIALOG
Class4=MiniRaysClientStatus
Resource4=IDD_STATUS
Class5=MiniRaysClientUpload
Resource5=IDD_UPLOAD

[CLS:CMiniRaysClientApp]
Type=0
HeaderFile=MiniRaysClient.h
ImplementationFile=MiniRaysClient.cpp
Filter=N

[CLS:CMiniRaysClientDlg]
Type=0
HeaderFile=MiniRaysClientDlg.h
ImplementationFile=MiniRaysClientDlg.cpp
Filter=D
LastObject=CMiniRaysClientDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=MiniRaysClientDlg.h
ImplementationFile=MiniRaysClientDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MINIRAYSCLIENT_DIALOG]
Type=1
Class=CMiniRaysClientDlg
ControlCount=15
Control1=IDOK,button,1342242817
Control2=IDC_IPADDRESS,SysIPAddress32,1342242816
Control3=IDC_CONNECT,button,1342242816
Control4=IDC_PORT,edit,1350639745
Control5=IDC_DISCONNECT,button,1476460544
Control6=IDC_STATIC,button,1342177287
Control7=IDC_OUTPUT,listbox,1352728833
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SCENE,static,1342312448
Control11=IDC_CHOOSESCENE,button,1342242816
Control12=IDC_RENDER,button,1342242816
Control13=IDC_STATUS,button,1342242816
Control14=IDC_INSTALL,button,1342242816
Control15=IDC_OPTIONS,button,1342242816

[DLG:IDD_STATUS]
Type=1
Class=MiniRaysClientStatus
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_JOBID,static,1342308864
Control8=IDC_STARTTIME,static,1342308864
Control9=IDC_JOBDONE,static,1342308864
Control10=IDC_ENDTIME,static,1342308864
Control11=IDC_JOBSDONE,combobox,1344339970
Control12=IDC_STATIC,button,1342177287
Control13=IDC_GETJOB,button,1342242816
Control14=IDC_DELJOB,button,1342242816
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_LISTTEXTURES,listbox,1352728835
Control19=IDC_LISTPLUGINS,listbox,1352728835
Control20=IDC_REFRESH,button,1342242816

[CLS:MiniRaysClientStatus]
Type=0
HeaderFile=MiniRaysClientStatus.h
ImplementationFile=MiniRaysClientStatus.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_DELJOB

[DLG:IDD_UPLOAD]
Type=1
Class=MiniRaysClientUpload
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_UPLOAD,static,1342312448
Control4=IDC_CHOOSESCENE,button,1342242816
Control5=IDC_INFO,static,1342312448
Control6=IDC_DOUPLOAD,button,1342242816

[CLS:MiniRaysClientUpload]
Type=0
HeaderFile=MiniRaysClientUpload.h
ImplementationFile=MiniRaysClientUpload.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

