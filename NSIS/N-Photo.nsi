;--------------------------------

; The name of the installer
Name "N-Photo"

; The file to write
OutFile "N-Photo.exe"

; The default installation directory
InstallDir $TEMP\N-Photo

; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages

PageEx license
   LicenseData license.txt
   LicenseForceSelection checkbox
PageExEnd
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

 
; The stuff to install
Section "N-Photo (required)"

  SectionIn RO

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
  File "puzzle\n-photo.exe"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\N-Photo "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "DisplayName" "N-Photo"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\N-Photo"
  CreateShortcut "$SMPROGRAMS\N-Photo\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\N-Photo\N-Photo.lnk" "$INSTDIR\N-Photo.exe" "" "$INSTDIR\n-photo.exe" 0

SectionEnd
; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\N-Photo"
  DeleteRegKey HKLM SOFTWARE\n-Puzzle

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