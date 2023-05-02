;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Version
  !define version 1.1.10.2

  ;Name and file
  Name "N-Photo ${version}"
  OutFile N-Photo-${version}.exe

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\N-Photo"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\N-Photo" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

  ;Brand
  BrandingText "/appseng/n-photo"
  
  ;Version information
  VIProductVersion ${version}
  VIAddVersionKey "ProductVersion" ${version}
  VIAddVersionKey "ProductName" "N-Photo"
  VIAddVersionKey "LegalCopyright" "© appseng"
  VIAddVersionKey "FileDescription" "N-Photo-${version}.exe"
  VIAddVersionKey "CompanyName" "appseng"
  VIAddVersionKey "FileVersion" ${version}

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
  ; Put file there
  SetOutPath $INSTDIR\platforms
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
  File "puzzle\n-photo.exe"

  CreateDirectory "$SMPROGRAMS\N-Photo"
  CreateShortcut "$SMPROGRAMS\N-Photo\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\N-Photo\N-Photo.lnk" "$INSTDIR\n-photo.exe" "" "$INSTDIR\n-photo.exe" 0

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "DisplayName" "N-Photo"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;Uninstaller Section

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo"
  DeleteRegKey HKLM SOFTWARE\N-Photo

  ; Remove files and uninstaller
  Delete $INSTDIR\imageformats\qjpeg.dll
  RMDir "$INSTDIR\imageformats"
  Delete $INSTDIR\platforms\qwindows.dll
  RMDir "$INSTDIR\platforms"
  Delete "$INSTDIR\*.*"
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\N-Photo\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\N-Photo"
  RMDir "$INSTDIR"

SectionEnd