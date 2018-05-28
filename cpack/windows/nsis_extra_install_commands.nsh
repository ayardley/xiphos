#
#
######################################
#
# CPACK_NSIS_EXTRA_INSTALL_COMMANDS
#
#######################################
#
#

  ; Install fonts

  StrCpy $FONT_DIR $FONTS

  ; Libertine font
  !insertmacro InstallTTFFont "${PATH_FONT}\LinLibertine_Re.ttf" \
    "Linux Libertine (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\LinLibertine_Bd.ttf" \
    "Linux Libertine Bold (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\LinLibertine_It.ttf" \
    "Linux Libertine Italic (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\LinLibertine_BI.ttf" \
    "Linux Libertine Bold Italic (TrueType)"

  ; GNU Unifont - contains all unicode characters
  !insertmacro InstallTTFFont "${PATH_FONT}\unifont-5.1.20080907.ttf" \
    "GNU Unifont (TrueType)"

  ; FreeSans font
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSans.ttf" \
    "Free Sans (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSansBold.ttf" \
    "Free Sans Bold (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSansOblique.ttf" \
    "Free Sans Oblique (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSansBoldOblique.ttf" \
    "Free Sans Bold Oblique (TrueType)"

  ; FreeSerif font
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSerif.ttf" \
    "Free Serif (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSerifBold.ttf" \
    "Free Serif Bold (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSerifItalic.ttf" \
    "Free Serif Italic (TrueType)"
  !insertmacro InstallTTFFont "${PATH_FONT}\FreeSerifBoldItalic.ttf" \
    "Free Serif Bold Italic (TrueType)"

  ; Notify Windows applications about new fonts
  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000


  ; Shared folder for Sword modules must exist
  SetShellVarContext all
  ; Shared Sword Folder
  SetOutPath '$APPDATA\${INSTPATH_SWORD}'
  ; set to default value
  SetShellVarContext current


  !define APP_BINARY_NAME "xiphos.exe"

  ; Xiphos as default handler of url  sword://
  ;WriteRegStr HKCR "sword" "" "URL:sword Protocol"
  ;WriteRegStr HKCR "sword" "URL Protocol" ""
  ;WriteRegStr HKCR "sword\DefaultIcon" "" \
    ;'"$INSTDIR\bin\${APP_BINARY_NAME}"'
  ;WriteRegStr HKCR "sword\shell\open\command" "" \
    ;'"$INSTDIR\bin\${APP_BINARY_NAME}" "%1"'

  WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY} "" "URL:sword Protocol"
  WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY} "URL Protocol" ""
  WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY_ICON} "" \
    '"$INSTDIR\bin\${APP_BINARY_NAME}"'
  WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY_COMMAND} "" \
    '"$INSTDIR\bin\${APP_BINARY_NAME}" "%1"'

#
#
######################################

