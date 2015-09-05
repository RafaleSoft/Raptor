; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRaysINetBridgeDlg
LastTemplate=CSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "RaysINetBridge.h"

ClassCount=3
Class1=CRaysINetBridgeApp
Class2=CRaysINetBridgeDlg

ResourceCount=2
Resource1=IDR_MAINFRAME
Class3=CInetSocket
Resource2=IDD_RAYSINETBRIDGE_DIALOG

[CLS:CRaysINetBridgeApp]
Type=0
HeaderFile=RaysINetBridge.h
ImplementationFile=RaysINetBridge.cpp
Filter=N

[CLS:CRaysINetBridgeDlg]
Type=0
HeaderFile=RaysINetBridgeDlg.h
ImplementationFile=RaysINetBridgeDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON1



[DLG:IDD_RAYSINETBRIDGE_DIALOG]
Type=1
Class=CRaysINetBridgeDlg
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_IPADDRESS1,SysIPAddress32,1342242816
Control4=IDC_PORT1,edit,1350639745
Control5=IDC_STATIC,button,1342177287
Control6=IDC_IPADDRESS2,SysIPAddress32,1342242816
Control7=IDC_PORT2,edit,1350639745
Control8=IDC_OUTPUT,listbox,1344340225
Control9=IDC_BUTTON1,button,1342242816
Control10=IDC_INPUT,edit,1350631552
Control11=IDC_SEND,button,1342242816

[CLS:CInetSocket]
Type=0
HeaderFile=Inetsocket.h
ImplementationFile=Inetsocket.cpp
BaseClass=CSocket
Filter=N
VirtualFilter=uq
LastObject=CInetSocket

