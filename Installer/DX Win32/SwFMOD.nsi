; -----------------------------------------------------------------------------
;  SwFMOD Deus Ex Win32 Installer
; -----------------------------------------------------------------------------

; -----------------------------------------------------------------------------
;  INCLUDES
; -----------------------------------------------------------------------------

!include "MUI.nsh"


; -----------------------------------------------------------------------------
;  GENERAL
; -----------------------------------------------------------------------------

!define Version "1.0.0.8"

Name "SwFMOD Deus Ex ${Version}"
OutFile "SwFMOD_DeusEx_${Version}.exe"
SetCompressor LZMA 
XPStyle on


; -----------------------------------------------------------------------------
;  onInit
; -----------------------------------------------------------------------------
Function .onInit

    Call MyProcessChecker
    Call MyDirFinder
   
FunctionEnd


; -----------------------------------------------------------------------------
;  onVerifyInstDir
; -----------------------------------------------------------------------------

Function .onVerifyInstDir

    IfFileExists "$INSTDIR\System\DeusEx.exe" Valid
    Abort
    
Valid:

FunctionEnd
  

; -----------------------------------------------------------------------------
;  MyProcessChecker
; -----------------------------------------------------------------------------

Function MyProcessChecker

Start:
    StrCpy $0 "DeusEx.exe"
    FindProcDLL::FindProc $0
    IntCmp $R0 1 Found

    StrCpy $0 "UCC.exe"
    FindProcDLL::FindProc $0
    IntCmp $R0 1 Found

    StrCpy $0 "UnrealEd.exe"
    FindProcDLL::FindProc $0
    IntCmp $R0 1 Found
    
    Return

Found:
    MessageBox MB_RETRYCANCEL|MB_TOPMOST|MB_ICONSTOP|MB_SETFOREGROUND "Please close $0" /SD IDCANCEL IDRETRY Start IDCANCEL 0
    Abort "$0 must be closed first."

FunctionEnd


; -----------------------------------------------------------------------------
;  MyDirFinder
; -----------------------------------------------------------------------------

Function MyDirFinder

    ReadRegStr $0 HKLM "SOFTWARE\Unreal Technology\Installed Apps\Deus Ex" "Folder"
    StrCmp $0 "" RegDirReadManual RegDirVerifyFull

RegDirReadManual:
    StrCpy $0 "Click Browse and select your Deus Ex folder"
    Goto RegDirFound

RegDirVerifyFull:
    IfFileExists "$0\System\DeusEx.exe" RegDirFound RegDirReadManual

RegDirFound:
    StrCpy $INSTDIR $0

FunctionEnd  




; -----------------------------------------------------------------------------
;  MyRun
; -----------------------------------------------------------------------------
Function MyRun

    Exec '"$INSTDIR\System\DeusEx.exe"'
    
FunctionEnd


; -----------------------------------------------------------------------------
;  MyReadme
; -----------------------------------------------------------------------------
Function MyReadme

    ExecShell "open" '"$INSTDIR\Help\SwFMOD.int.txt"'
    
FunctionEnd


; -----------------------------------------------------------------------------
;  EnableINI
; -----------------------------------------------------------------------------
Function EnableINI

    
    ; skip file if no AudioDevice
    ReadINIStr $2 "$INSTDIR\System\$1" Engine.Engine AudioDevice
    IfErrors stop
        
    loop:
        ClearErrors
        
        ; read AudioDevice
        ReadINIStr $2 "$INSTDIR\System\$1" Engine.Engine AudioDevice
        IfErrors done
        
        ; if SwFMOD skip file, else delete
        StrCmp $2 "SwFMOD.SwFMOD" stop
        DeleteINIStr "$INSTDIR\System\$1" Engine.Engine AudioDevice
        IfErrors done
        
        ; write commented AudioDevice
        WriteINIStr "$INSTDIR\System\$1" Engine.Engine //AudioDevice $2
        IfErrors done
        
        Goto loop
    done:
    
    ; write SwFMOD AudioDevice
    WriteINIStr "$INSTDIR\System\$1" Engine.Engine AudioDevice SwFMOD.SwFMOD
    DetailPrint "EnableINI $INSTDIR\System\$1"
        
    stop:
FunctionEnd


; -----------------------------------------------------------------------------
;  un.DisableINI
; -----------------------------------------------------------------------------
Function un.DisableINI
    
    ; remove SwFMOD config
    DeleteINISec "$INSTDIR\System\$1" SwFMOD.SwFMOD
    
    
    ClearErrors
    
    ; read AudioDevice
    ReadINIStr $2 "$INSTDIR\System\$1" Engine.Engine AudioDevice
    IfErrors uncomment
    
    ; if SwFMOD remove, else skip
    StrCmp $2 "SwFMOD.SwFMOD" 0 uncomment
    DeleteINIStr "$INSTDIR\System\$1" Engine.Engine AudioDevice
    IfErrors uncomment
    DetailPrint "DisableINI $INSTDIR\System\$1"
    
    uncomment:
        ClearErrors
        
        ; read commented AudioDevice
        ReadINIStr $2 "$INSTDIR\System\$1" Engine.Engine //AudioDevice
        IfErrors uncommentend
        
        ; remove it
        DeleteINIStr "$INSTDIR\System\$1" Engine.Engine //AudioDevice
        IfErrors uncommentend
        
        ; if not SwFMOD write uncommented
        StrCmp $2 "SwFMOD.SwFMOD" +2
        WriteINIStr "$INSTDIR\System\$1" Engine.Engine "AudioDevice" $2
        IfErrors uncommentend
        
        Goto uncomment
    uncommentend:
     
FunctionEnd


; -----------------------------------------------------------------------------
;  INTERFACE SETTINGS
; -----------------------------------------------------------------------------

!define MUI_ABORTWARNING

!define MUI_FINISHPAGE_RUN 
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_FUNCTION MyRun

!define MUI_FINISHPAGE_SHOWREADME
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION MyReadme

!define MUI_FINISHPAGE_LINK "http://sourceforge.net/projects/swfmod/"
!define MUI_FINISHPAGE_LINK_LOCATION "http://sourceforge.net/projects/swfmod/"

!define MUI_ICON "..\Install.ico"
!define MUI_UNICON "..\Install.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "..\Header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "..\Header.bmp"






; -----------------------------------------------------------------------------
;  PAGES
; -----------------------------------------------------------------------------

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


; -----------------------------------------------------------------------------
;  Languages
; -----------------------------------------------------------------------------
 
!insertmacro MUI_LANGUAGE "English"


; -----------------------------------------------------------------------------
;  Version
; ----------------------------------------------------------------------------- 

VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Requires Deus Ex v1112fm"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "http://sourceforge.net/projects/swfmod/"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "©2007 Roman Dzieciol"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "SwFMOD Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${Version}"

VIProductVersion "${Version}"



; -----------------------------------------------------------------------------
;  SECTIONS
; -----------------------------------------------------------------------------

Section "" Main

    SetOutPath "$INSTDIR"
    
    ; check DeusEx version
    ReadRegStr $R0 HKLM "SOFTWARE\Unreal Technology\Installed Apps\Deus Ex" "Version"
    DetailPrint "Detected Deus Ex Version: $R0" 
    StrCmp $R0 "1112fm" +2
    MessageBox MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION|MB_SETFOREGROUND "Unsupported Deus Ex version detected: $R0$\r$\nSwFMOD requires Deus Ex version 1112fm!" /SD IDOK IDOK 0
    
    ; copy files
    File "/oname=$OUTDIR\System\fmodex.dll" "Files\System\fmodex.dll"
    File "/oname=$OUTDIR\System\SwFMOD.dll" "Files\System\SwFMOD.dll"
    File "/oname=$OUTDIR\System\SwFMOD.int" "Files\System\SwFMOD.int"
    File "/oname=$OUTDIR\Help\SwFMOD.int.txt" "Files\Help\SwFMOD.int.txt"
    
    ; update ini's
    ClearErrors
    FindFirst $0 $1 $INSTDIR\System\*.ini
    loop:
        IfErrors done
        Call EnableINI
        ClearErrors
        FindNext $0 $1
        Goto loop
    done:
        
    ; create shortcuts
    CreateDirectory "$SMPROGRAMS\SwFMOD"
    CreateShortCut "$SMPROGRAMS\SwFMOD\SwFMOD Readme.lnk" "$INSTDIR\Help\SwFMOD.int.txt" "" "" 0 SW_SHOWNORMAL "" "SwFMOD Readme"
    CreateShortCut "$SMPROGRAMS\SwFMOD\SwFMOD Uninstall.lnk" "$INSTDIR\Uninstall SwFMOD.exe" "" "" 0 SW_SHOWNORMAL "" "SwFMOD Uninstall"

    
    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall SwFMOD.exe"
        
SectionEnd


;--------------------------------
;Uninstaller Section

Section "Uninstall"

    ; delete SwFMOD
    Delete "$INSTDIR\System\fmodex.dll"
    Delete "$INSTDIR\System\SwFMOD.dll"
    Delete "$INSTDIR\System\SwFMOD.int"
    Delete "$INSTDIR\Help\SwFMOD.int.txt"
    
    ; restore ini's
    ClearErrors
    FindFirst $0 $1 $INSTDIR\System\*.ini
    loop:
        IfErrors done
        Call un.DisableINI
        ClearErrors
        FindNext $0 $1
        Goto loop
    done:
    
    ; Delete shortcuts
    Delete "$SMPROGRAMS\SwFMOD\SwFMOD Readme.lnk"
    Delete "$SMPROGRAMS\SwFMOD\SwFMOD Uninstall.lnk"
    RMDir "$SMPROGRAMS\SwFMOD\"
    
    ; delete uninstaller
    Delete "$INSTDIR\Uninstall SwFMOD.exe"
    

SectionEnd


; -----------------------------------------------------------------------------