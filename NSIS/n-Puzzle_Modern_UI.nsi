;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "n-Puzzle"
  OutFile "n-Puzzle.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\n-Puzzle"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\n-Puzzle" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "license.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Installation" Installation

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\platforms

  ; Put file there
  File "puzzle\platforms\qwindows.dll"
  SetOutPath $INSTDIR\imageformats
  File "puzzle\imageformats\qjpeg.dll"
  SetOutPath $INSTDIR
  File "puzzle\Qt5Core.dll"
  File "puzzle\Qt5Gui.dll"
  File "puzzle\Qt5Network.dll"
  File "puzzle\Qt5Widgets.dll"
  File "puzzle\libgcc_s_dw2-1.dll"
  File "puzzle\libstdc++-6.dll"
  File "puzzle\libwinpthread-1.dll"
  File "puzzle\n-puzzle.exe"

  CreateDirectory "$SMPROGRAMS\n-Puzzle"
  CreateShortcut "$SMPROGRAMS\n-Puzzle\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\n-Puzzle\n-Puzzle.lnk" "$INSTDIR\n-puzzle.exe" "" "$INSTDIR\n-puzzle.exe" 0

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\n-Puzzle "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "DisplayName" "n-Puzzle"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Installation} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle"
  DeleteRegKey HKLM SOFTWARE\n-Puzzle

  ; Remove files and uninstaller
  Delete $INSTDIR\imageformats\qjpeg.dll
  RMDir "$INSTDIR\imageformats"
  Delete $INSTDIR\platforms\qwindows.dll
  RMDir "$INSTDIR\platforms"
  Delete "$INSTDIR\*.*"
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\n-Puzzle\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\n-Puzzle"
  RMDir "$INSTDIR"

SectionEnd