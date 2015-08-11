# Microsoft Developer Studio Project File - Name="RaysClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RAYSCLIENT - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RaysClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RaysClient.mak" CFG="RAYSCLIENT - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RaysClient - Win32 Release" (based on "Win32 (x86) Application")
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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../Systemdll/Systemdll.lib ../SimdLib/simd.lib /nologo /subsystem:windows /machine:I386 /out:"RaysClient.exe"
# SUBTRACT LINK32 /debug
# Begin Target

# Name "RaysClient - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "RenderObjects"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\Camera.cpp
# End Source File
# Begin Source File

SOURCE=..\RenderObject.cpp
# End Source File
# End Group
# Begin Group "FrameWork"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\RaysClientDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\RaysClientView.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Parsing.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RaysClient.cpp
# End Source File
# Begin Source File

SOURCE=.\RaysClient.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "RenderObjects headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Camera.h
# End Source File
# Begin Source File

SOURCE=..\RenderObject.h
# End Source File
# End Group
# Begin Group "FrameWork headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ObjectView.h
# End Source File
# Begin Source File

SOURCE=.\RaysClientDoc.h
# End Source File
# Begin Source File

SOURCE=.\RaysClientView.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RaysClient.h
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

SOURCE=.\res\Clouds.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\plugin.ico
# End Source File
# Begin Source File

SOURCE=.\res\Rays.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysClient.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysClient.rc2
# End Source File
# Begin Source File

SOURCE=.\res\RaysClientDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\RaysDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\surfa.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\RaysClient.reg
# End Source File
# End Target
# End Project
