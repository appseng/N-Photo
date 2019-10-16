;--------------------------------

; The name of the installer
Name "n-Puzzle"

; The file to write
OutFile "n-Puzzle.exe"

; The default installation directory
InstallDir $TEMP\n-Puzzle

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
Section "n-Puzzle (required)"

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
  File "puzzle\n-puzzle.exe"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\n-Puzzle "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "DisplayName" "n-Puzzle"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\n-Puzzle" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\n-Puzzle"
  CreateShortcut "$SMPROGRAMS\n-Puzzle\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\n-Puzzle\n-Puzzle.lnk" "$INSTDIR\n-puzzle.exe" "" "$INSTDIR\n-puzzle.exe" 0

SectionEnd
; Uninstaller

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