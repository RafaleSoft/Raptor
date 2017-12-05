# Microsoft Developer Studio Project File - Name="MiniRaysClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MINIRAYSCLIENT - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MiniRaysClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MiniRaysClient.mak" CFG="MINIRAYSCLIENT - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MiniRaysClient - Win32 Release" (based on "Win32 (x86) Application")
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
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../Systemdll/Systemdll.lib ../Raptor/bin/Raptor.lib /nologo /subsystem:windows /debug /machine:I386 /out:"MiniRaysClient.exe"
# Begin Target

# Name "MiniRaysClient - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Analyser.cpp
# End Source File
# Begin Source File

SOURCE=.\CltSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClient.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClient.rc
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientUpload.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgProcessing.cpp
# End Source File
# Begin Source File

SOURCE=..\Plugin.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Rays2_lex.cpp
# End Source File
# Begin Source File

SOURCE=.\Rays2_tree.cpp
# End Source File
# Begin Source File

SOURCE=.\Rays2_yacc.cpp
# End Source File
# Begin Source File

SOURCE=..\RaysClientSocket.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CltSocket.h
# End Source File
# Begin Source File

SOURCE=..\Messages.h
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClient.h
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientDlg.h
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientStatus.h
# End Source File
# Begin Source File

SOURCE=.\MiniRaysClientUpload.h
# End Source File
# Begin Source File

SOURCE=..\PlugIn.h
# End Source File
# Begin Source File

SOURCE=.\Rays2_lex.h
# End Source File
# Begin Source File

SOURCE=.\Rays2_tree.h
# End Source File
# Begin Source File

SOURCE=.\Rays2_yacc.h
# End Source File
# Begin Source File

SOURCE=..\RaysClientSocket.h
# End Source File
# Begin Source File

SOURCE=..\RaysCommandLine.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MiniRaysClient.ico
# End Source File
# Begin Source File

SOURCE=.\res\MiniRaysClient.rc2
# End Source File
# Begin Source File

SOURCE=.\Rays.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\Rays.lex
# Begin Custom Build
InputPath=..\Rays.lex

BuildCmds= \
	..\MicroLex -f $(InputPath) -o Rays2 -m -v

"Rays2_lex.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Rays2_lex.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Rays2_yacc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Rays2_yacc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Rays2_tree.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Rays2_tree.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# End Source File
# End Target
# End Project
