# Microsoft Developer Studio Project File - Name="Main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=Main - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Main.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Main.mak" CFG="Main - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Main - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# Begin Target

# Name "Main - Win32 Release"
# Begin Source File

SOURCE=.\RunRays.bat
# Begin Custom Build
InputPath=.\RunRays.bat

BuildCmds= \
	copy ..\Workunit\Workunit.exe . \
	copy ..\RaysServer\RaysServer.exe . \
	copy ..\RaysInetBridge\RaysInetBridge.exe . \
	copy ..\RaysDeamon\RaysDeamon.exe . \
	copy ..\ModuleTester\ModuleTester.exe . \
	copy ..\ModuleTemplate\ModuleTemplate.dll . \
	copy ..\ModuleNoise\ModuleNoise.dll . \
	copy ..\ModuleDOF\ModuleDOF.dll . \
	copy ..\MiniRaysClient\MiniRaysClient.exe . \
	copy ..\Raptor\bin\Raptor.dll . \
	copy ..\Systemdll\Systemdll.dll . \
	

"Workunit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"RaysServer.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"RaysInetBridge.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"RaysDeamon.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ModuleTester.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ModuleTemplate.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ModuleNoise.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ModuleDOF.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MiniRaysClient.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Raptor.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Systemdll.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# End Source File
# End Target
# End Project
