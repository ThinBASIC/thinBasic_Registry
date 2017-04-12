#COMPILE DLL
#REGISTER NONE
#DIM ALL

'------------------------------------------------------------------
#INCLUDE ".\thinBasic_registry_resources.inc"
#RESOURCE VERSIONINFO
#RESOURCE FILEVERSION %THINBASIC_REGISTRY_VERSIONMAJOR, %THINBASIC_REGISTRY_VERSIONMINOR, %THINBASIC_REGISTRY_VERSIONBUILD, %THINBASIC_REGISTRY_VERSIONREVISION
#RESOURCE PRODUCTVERSION %THINBASIC_REGISTRY_VERSIONMAJOR, %THINBASIC_REGISTRY_VERSIONMINOR, %THINBASIC_REGISTRY_VERSIONBUILD, %THINBASIC_REGISTRY_VERSIONREVISION

#RESOURCE STRINGINFO "0409", "04E4"

#RESOURCE VERSION$ "CompanyName",      $THIN_COMMON_COMPANYNAME
#RESOURCE VERSION$ "FileDescription",  $THINBASIC_REGISTRY_FILEDESCRIPTION
#RESOURCE VERSION$ "FileVersion",      $THINBASIC_REGISTRY_VERSIONLITERAL
#RESOURCE VERSION$ "InternalName",     $THINBASIC_REGISTRY_INTERNALNAME
#RESOURCE VERSION$ "OriginalFilename", $THINBASIC_REGISTRY_ORIGINALFILENAME
#RESOURCE VERSION$ "LegalCopyright",   $THIN_COMMON_LEGALCOPYRIGHT
#RESOURCE VERSION$ "ProductName",      $THINBASIC_REGISTRY_PRODUCTNAME
#RESOURCE VERSION$ "ProductVersion",   $THINBASIC_REGISTRY_VERSIONLITERAL
#RESOURCE VERSION$ "Comments",         $THIN_COMMON_COMMENT
'------------------------------------------------------------------
#INCLUDE "WIN32API.INC"
#INCLUDE ".\thinBasic_registry_interface.INC"
'------------------------------------------------------------------

'----------------------------------------------------------------------------
FUNCTION LoadLocalSymbols ALIAS "LoadLocalSymbols" (OPTIONAL BYVAL sPath AS STRING) EXPORT AS LONG

  ' -- exposes the functionality to thinBasic
  Registry_LoadSymbols()

  FUNCTION = 0&
END FUNCTION

'----------------------------------------------------------------------------
FUNCTION UnLoadLocalSymbols ALIAS "UnLoadLocalSymbols" () EXPORT AS LONG
' This function is automatically called by thinCore whenever this DLL is unloaded.
' This function CAN be present but it is not necessary.
' Use this function to perform uninitialize process, if needed.
'----------------------------------------------------------------------------

  FUNCTION = 0&
END FUNCTION

FUNCTION LIBMAIN ALIAS "LibMain" (BYVAL hInstance   AS LONG, _
                                  BYVAL fwdReason   AS LONG, _
                                  BYVAL lpvReserved AS LONG) EXPORT AS LONG
  SELECT CASE fwdReason
    CASE %DLL_PROCESS_ATTACH

      FUNCTION = 1
      EXIT FUNCTION
    CASE %DLL_PROCESS_DETACH

      FUNCTION = 1
      EXIT FUNCTION
    CASE %DLL_THREAD_ATTACH

      FUNCTION = 1
      EXIT FUNCTION
    CASE %DLL_THREAD_DETACH

      FUNCTION = 1
      EXIT FUNCTION
  END SELECT

END FUNCTION
