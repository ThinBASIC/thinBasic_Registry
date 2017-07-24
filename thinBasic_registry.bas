#COMPILE DLL
#REGISTER NONE
#DIM ALL

'------------------------------------------------------------------
#Resource RES "thinBasic_registry.RES"
'------------------------------------------------------------------
#INCLUDE "WIN32API.INC"
#INCLUDE "..\module_core\thinCore.INC"
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
