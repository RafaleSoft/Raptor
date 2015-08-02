# Microsoft Developer Studio Project File - Name="WorkUnit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WorkUnit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WorkUnit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WorkUnit.mak" CFG="WorkUnit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WorkUnit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /Od /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../Systemdll/Systemdll.lib ../simdlib/simd.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /out:"WorkUnit.exe"
# Begin Target

# Name "WorkUnit - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Environment.cpp
# End Source File
# Begin Source File

SOURCE=.\GenericLight.cpp
# End Source File
# Begin Source File

SOURCE=.\GenericRay.cpp
# End Source File
# Begin Source File

SOURCE=.\GenericRenderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgProcessing.cpp
# End Source File
# Begin Source File

SOURCE=.\Octree.cpp
# End Source File
# Begin Source File

SOURCE=.\PhotonMap.cpp
# End Source File
# Begin Source File

SOURCE=..\Plugin.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Quadric.cpp
# End Source File
# Begin Source File

SOURCE=.\Ray.cpp
# End Source File
# Begin Source File

SOURCE=..\RaysClientSocket.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Raytracer.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SSE_BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SSE_CMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\SSE_COctree.cpp
# End Source File
# Begin Source File

SOURCE=.\SSE_CRenderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SSE_CTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Surfacer.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Triangle.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkUnit.rc
# End Source File
# Begin Source File

SOURCE=.\WorkUnitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WUSocket.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\Environment.h
# End Source File
# Begin Source File

SOURCE=.\GenericLight.h
# End Source File
# Begin Source File

SOURCE=.\GenericRay.h
# End Source File
# Begin Source File

SOURCE=.\GenericRenderObject.h
# End Source File
# Begin Source File

SOURCE=.\Mesh.h
# End Source File
# Begin Source File

SOURCE=..\Messages.h
# End Source File
# Begin Source File

SOURCE=.\Octree.h
# End Source File
# Begin Source File

SOURCE=.\PhotonMap.h
# End Source File
# Begin Source File

SOURCE=..\PlugIn.h
# End Source File
# Begin Source File

SOURCE=.\Quadric.h
# End Source File
# Begin Source File

SOURCE=.\Ray.h
# End Source File
# Begin Source File

SOURCE=.\Raytracer.h
# End Source File
# Begin Source File

SOURCE=.\RenderObject.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SSE_BoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\SSE_CMesh.h
# End Source File
# Begin Source File

SOURCE=.\SSE_COctree.h
# End Source File
# Begin Source File

SOURCE=.\SSE_CRenderObject.h
# End Source File
# Begin Source File

SOURCE=.\SSE_CTriangle.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Surfacer.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\Triangle.h
# End Source File
# Begin Source File

SOURCE=.\WorkUnit.h
# End Source File
# Begin Source File

SOURCE=.\WorkUnitDlg.h
# End Source File
# Begin Source File

SOURCE=.\WUSocket.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\idr_rend.ico
# End Source File
# Begin Source File

SOURCE=.\res\WorkUnit.ico
# End Source File
# Begin Source File

SOURCE=.\res\WorkUnit.rc2
# End Source File
# End Group
# End Target
# End Project
