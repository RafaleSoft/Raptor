# Microsoft Developer Studio Project File - Name="RaysServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RaysServer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RaysServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RaysServer.mak" CFG="RaysServer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RaysServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../Systemdll/Systemdll.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"RaysServer.exe"
# Begin Target

# Name "RaysServer - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DeamonIP.cpp
# End Source File
# Begin Source File

SOURCE=.\DeamonManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DeamonSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogWUPath.cpp
# End Source File
# Begin Source File

SOURCE=.\FileManager.cpp
# End Source File
# Begin Source File

SOURCE=.\JobDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgProcessing.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=..\Plugin.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PostProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\RaysClientSocket.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RaysServer.cpp
# End Source File
# Begin Source File

SOURCE=.\RaysServer.rc
# End Source File
# Begin Source File

SOURCE=.\RaysServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DeamonIP.h
# End Source File
# Begin Source File

SOURCE=.\DeamonManager.h
# End Source File
# Begin Source File

SOURCE=.\DeamonSocket.h
# End Source File
# Begin Source File

SOURCE=.\DialogWUPath.h
# End Source File
# Begin Source File

SOURCE=.\FileManager.h
# End Source File
# Begin Source File

SOURCE=..\Messages.h
# End Source File
# Begin Source File

SOURCE=.\MsgManager.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=..\PlugIn.h
# End Source File
# Begin Source File

SOURCE=.\PostProcessor.h
# End Source File
# Begin Source File

SOURCE=..\RaysCommandLine.h
# End Source File
# Begin Source File

SOURCE=.\RaysServer.h
# End Source File
# Begin Source File

SOURCE=.\RaysServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SSocket.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\idr_rend.ico
# End Source File
# Begin Source File

SOURCE=.\res\Rays.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysServer.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysServer.rc2
# End Source File
# End Group
# End Target
# End Project
# Section RaysServer : {72ADFD7E-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DISK_SPACE:105
# 	1:19:CG_IDS_PHYSICAL_MEM:104
# 	1:25:CG_IDS_DISK_SPACE_UNAVAIL:106
# 	2:14:PhysicalMemory:CG_IDS_PHYSICAL_MEM
# 	2:9:DiskSpace:CG_IDS_DISK_SPACE
# 	2:16:SpaceUnavailable:CG_IDS_DISK_SPACE_UNAVAIL
# 	2:7:NewFunc:1
# 	2:10:SysInfoKey:1234
# End Section
