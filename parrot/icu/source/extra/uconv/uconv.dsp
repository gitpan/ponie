# Microsoft Developer Studio Project File - Name="uconv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=uconv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uconv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uconv.mak" CFG="uconv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uconv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "uconv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "uconv - Win64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "uconv - Win64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uconv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\common" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "UCONVMSG_LINK" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 icuuc.lib icuin.lib uconvmsg.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\..\lib" /libpath:"Release"
# Begin Custom Build
ProjDir=.
TargetPath=.\Release\uconv.exe
InputPath=.\Release\uconv.exe
InputName=uconv
SOURCE="$(InputPath)"

"$(ProjDir)\..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "uconv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Exclude_From_Build 1
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "UCONVMSG_LINK" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuucd.lib icuind.lib uconvmsg.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"Debug"

!ELSEIF  "$(CFG)" == "uconv - Win64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /Op /I "..\..\..\include" /I "..\..\common" /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "UCONVMSG_LINK" /D "_IA64_" /D "WIN32" /D "_AFX_NO_DAO_SUPPORT" /FD /QIA64_fmaopt /Wp64 /Zm600 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:IX86 /machine:IA64
# ADD LINK32 icuuc.lib icuin.lib uconvmsg.lib /nologo /subsystem:console /machine:IX86 /libpath:"..\..\..\lib" /libpath:"Release" /machine:IA64
# Begin Custom Build
ProjDir=.
TargetPath=.\Release\uconv.exe
InputPath=.\Release\uconv.exe
InputName=uconv
SOURCE="$(InputPath)"

"$(ProjDir)\..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "uconv - Win64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /Op /I "..\..\..\include" /I "..\..\common" /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "UCONVMSG_LINK" /D "_IA64_" /D "WIN32" /D "_AFX_NO_DAO_SUPPORT" /FD /GZ /QIA64_fmaopt /Wp64 /Zm600 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:IX86 /pdbtype:sept /machine:IA64
# ADD LINK32 icuucd.lib icuind.lib uconvmsg.lib /nologo /subsystem:console /incremental:no /debug /machine:IX86 /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"Debug" /machine:IA64
# Begin Custom Build
ProjDir=.
TargetPath=.\Debug\uconv.exe
InputPath=.\Debug\uconv.exe
InputName=uconv
SOURCE="$(InputPath)"

"$(ProjDir)\..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ENDIF 

# Begin Target

# Name "uconv - Win32 Release"
# Name "uconv - Win32 Debug"
# Name "uconv - Win64 Release"
# Name "uconv - Win64 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\uconv.cpp
# End Source File
# Begin Source File

SOURCE=.\uwmsg.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\unicode\uwmsg.h
# End Source File
# End Group
# Begin Group "Resource Bundles"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\resources\fr.txt
# End Source File
# Begin Source File

SOURCE=.\resources\root.txt
# End Source File
# End Group
# Begin Group "Build Scripts"

# PROP Default_Filter "mak;mk;bat"
# Begin Source File

SOURCE=.\makedata.mak

!IF  "$(CFG)" == "uconv - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\makedata.mak
InputName=makedata

"Release/uconvmsg.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake /nologo /f $(InputName).mak icup=$(ProjDir)\..\..\.. CFG=Release

# End Custom Build

!ELSEIF  "$(CFG)" == "uconv - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\makedata.mak
InputName=makedata

"Debug/uconvmsg.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake /nologo /f $(InputName).mak icup=$(ProjDir)\..\..\.. CFG=Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "uconv - Win64 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\makedata.mak
InputName=makedata

"Release/uconvmsg.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake /nologo /f $(InputName).mak icup=$(ProjDir)\..\..\.. CFG=Release

# End Custom Build

!ELSEIF  "$(CFG)" == "uconv - Win64 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\makedata.mak
InputName=makedata

"Debug/uconvmsg.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake /nologo /f $(InputName).mak icup=$(ProjDir)\..\..\.. CFG=Debug

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resfiles.mk
# End Source File
# End Group
# End Target
# End Project
