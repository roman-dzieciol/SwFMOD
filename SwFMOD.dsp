# Microsoft Developer Studio Project File - Name="SwFMOD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SwFMOD - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SwFMOD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SwFMOD.mak" CFG="SwFMOD - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SwFMOD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SwFMOD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MT /W4 /GX /O2 /I "..\Core\Inc" /I "..\Engine\Inc" /I "..\Render\Src" /I "..\Extension\Inc" /I "..\DeusEx\Inc" /I ".\Inc" /I "D:\sdk\FMODex\api\inc" /D "NDEBUG" /D ThisPackage=SwFMOD /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "SW_HACK" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /out:"..\System\SwFMOD.dll" /libpath:"..\System\\" /libpath:"D:\sdk\FMODex\api\lib"
# SUBTRACT LINK32 /incremental:yes /debug
# Begin Target

# Name "SwFMOD - Win32 Release"
# Begin Group "vendor"

# PROP Default_Filter ""
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Core\Inc\Core.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Src\CorePrivate.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\DbgInfoCpp.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FCodec.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FConfigCacheIni.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFeedbackContextAnsi.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFeedbackContextWindows.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFileManagerAnsi.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFileManagerGeneric.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFileManagerLinux.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FFileManagerWindows.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FMallocAnsi.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FMallocDebug.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FMallocWindows.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FOutputDeviceAnsiError.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FOutputDeviceFile.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FOutputDeviceNull.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FOutputDeviceStdout.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FOutputDeviceWindowsError.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\FRiffChunk.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\SerialNumber.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UDebugInfo.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UExporter.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UFactory.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnArc.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnBits.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnBuild.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnCache.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnCId.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnClass.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnCoreNet.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnCorObj.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnFile.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnGnuG.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Src\UnLinker.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnMath.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnMem.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnName.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnNames.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnObjBas.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnObjVer.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnScript.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnStack.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnTemplate.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnType.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnUnix.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnVcWin32.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Core\Inc\UnVcWn32SSE.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Engine\Inc\AActor.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\ABrush.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\ACamera.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\AGameReplicationInfo.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\AInventory.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Src\Amd3d.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\AMover.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\APawn.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\APlayerPawn.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\APlayerReplicationInfo.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\AZoneInfo.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\Engine.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\EngineClasses.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Src\EnginePrivate.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\Palette.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\S3tc.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\ULevelSummary.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnActor.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnAudio.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnBunch.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnCamera.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnChan.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnCon.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnConn.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnDDraw.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnDemoPenLev.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnDemoRec.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnDynBsp.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnEngine.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnEngineGnuG.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnEngineWin.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnEventManager.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnGame.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnIn.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnLevel.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnMesh.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnModel.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnNet.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnNetDrv.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnObj.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Src\UnPath.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnPenLev.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnPlayer.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnPrim.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnReach.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnRender.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnRenderIterator.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnRenDev.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnScrTex.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnTex.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Engine\Inc\UnURL.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "FMOD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod.hpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod_codec.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod_dsp.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod_errors.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\FMODex\api\inc\fmod_output.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# Begin Group "cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\SwFMOD.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODToStr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SwFMODUpdate.cpp
# End Source File
# End Group
# Begin Group "h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Inc\SwFMOD.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SwFMODMacro.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SwFMODToStr.h
# End Source File
# Begin Source File

SOURCE=.\Inc\SwFMODUtil.h
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SwFMOD.rc
# End Source File
# End Group
# End Target
# End Project
