'
' Registry thinBASIC interface
'

#INCLUDE "implementation.inc"

' -----------------------------------------------------------------------------

FUNCTION Registry_LoadSymbols() AS LONG

  thinBasic_LoadSymbol "Registry_GetAllKeys"  , %thinBasic_ReturnString , CODEPTR(Exec_Registry_GetAllKeys),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_GetValue"    , %thinBasic_ReturnString , CODEPTR(Exec_Registry_GetValue  ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_GetDWord"    , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_GetDWord  ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_GetTxtNum"   , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_GetTxtNum ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_GetTxtBool"  , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_GetTxtBool),  %thinBasic_ForceOverWrite

  thinBasic_LoadSymbol "Registry_SetValue"    , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_SetValue  ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_SetDWord"    , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_SetDWord  ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_SetTxtNum"   , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_SetTxtNum ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_SetTxtBool"  , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_SetTxtBool),  %thinBasic_ForceOverWrite

  thinBasic_LoadSymbol "Registry_DelValue"    , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_DelValue  ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_DelKey"      , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_DelKey    ),  %thinBasic_ForceOverWrite

  thinBasic_LoadSymbol "Registry_KeyExists"   , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_KeyExists ),  %thinBasic_ForceOverWrite
  thinBasic_LoadSymbol "Registry_PathExists"  , %thinBasic_ReturnNumber , CODEPTR(Exec_Registry_PathExists),  %thinBasic_ForceOverWrite

END FUNCTION

' -----------------------------------------------------------------------------

FUNCTION Registry_ConvertHKey(BYVAL sHKey AS STRING) AS DWORD
  SELECT CASE AS CONST$ UCASE$(TRIM$(sHKey))
    CASE "HKEYCR"
      FUNCTION = %HKEY_CLASSES_ROOT
    CASE "HKEYCU"
      FUNCTION = %HKEY_CURRENT_USER
    CASE "HKEYLM"
      FUNCTION = %HKEY_LOCAL_MACHINE
    CASE "HKEYU"
      FUNCTION = %HKEY_USERS
    CASE "HKEYCC"
      FUNCTION = %HKEY_CURRENT_CONFIG
    CASE ELSE
      thinbasic_RunTimeError ( %ERR__MODULE_SPECIFIC, "Invalid hKey value, must be one of the following:" + $CRLF + "HKEYCR, HKEYCU, HKEYLM, HKEYU, HKEYCC" )
  END SELECT
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_GetValue() AS STRING
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_GetValue(HKey, MainKey, ValueName)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lMainKey    AS STRING
  LOCAL lValueName  AS STRING
  LOCAL lTmp        AS STRING
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckCloseParens() THEN
          tmpHKey = Registry_ConvertHKey(lHKey)
          IF tmpHKey <> 0 THEN
            lTmp = Registry_GetValue(tmpHKey, BYCOPY lMainKey, BYCOPY lValueName)
            FUNCTION = lTmp
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_KeyExists() AS EXT
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_KeyExists(HKey, MainKey, ValueName)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lMainKey    AS STRING
  LOCAL lValueName  AS STRING
  LOCAL lret        AS LONG
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckCloseParens() THEN
          tmpHKey = Registry_ConvertHKey(lHKey)
          IF tmpHKey <> 0 THEN
            lret = Registry_KeyExists(tmpHKey, BYCOPY lMainKey, BYCOPY lValueName)
            FUNCTION = lret
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_PathExists() AS EXT
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_PathExists(HKey, MainKey)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lMainKey    AS STRING
  LOCAL lret        AS LONG
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckCloseParens() THEN
        tmpHKey = Registry_ConvertHKey(lHKey)
        IF tmpHKey <> 0 THEN
          lret = Registry_PathExists(tmpHKey, BYCOPY lMainKey)
          FUNCTION = lret
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_GetDWord() AS EXT
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_GetDWord(HKey, MainKey, ValueName)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lMainKey    AS STRING
  LOCAL lValueName  AS STRING
  LOCAL lTmp        AS DWORD
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckCloseParens() THEN
          tmpHKey = Registry_ConvertHKey(lHKey)
          IF tmpHKey <> 0 THEN
            lTmp = Registry_GetDWord(tmpHKey, BYCOPY lMainKey, BYCOPY lValueName)
            FUNCTION = lTmp
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_GetTxtNum() AS EXT
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_GetTxtNum(HKey, MainKey, ValueName, DefaultValue)
'------------------------------------------------------------------------------
  LOCAL lHKey         AS STRING
  LOCAL lMainKey      AS STRING
  LOCAL lValueName    AS STRING
  LOCAL lDefaultValue AS EXT
  LOCAL lTmp          AS DWORD
  LOCAL tmpHKey       AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseNumber lDefaultValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              lTmp = Registry_GetTxtNum(tmpHKey, BYCOPY lMainKey, BYCOPY lValueName, BYCOPY lDefaultValue)
              FUNCTION = lTmp
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_GetTxtBool() AS EXT
'------------------------------------------------------------------------------
'Syntax: String = REGISTRY_GetTxtBool(HKey, MainKey, ValueName, DefaultValue)
'------------------------------------------------------------------------------
  LOCAL lHKey         AS STRING
  LOCAL lMainKey      AS STRING
  LOCAL lValueName    AS STRING
  LOCAL lDefaultValue AS EXT
  LOCAL lTmp          AS DWORD
  LOCAL tmpHKey       AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseNumber lDefaultValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              FUNCTION = Registry_GetTxtBool(tmpHKey, BYCOPY lMainKey, BYCOPY lValueName, BYCOPY lDefaultValue)
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_SetValue() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_SetValue(HKey, MainKey, Key, Value)
'------------------------------------------------------------------------------
  LOCAL lHKey     AS STRING
  LOCAL lMainKey  AS STRING
  LOCAL lKey      AS STRING
  LOCAL lTmp      AS EXT
  LOCAL lValue    AS STRING
  LOCAL tmpHKey   AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lKey
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseString lValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              lTmp = Registry_SetValue(tmpHKey, BYCOPY lMainKey, BYCOPY lKey, BYCOPY lValue)
              FUNCTION = lTmp
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_SetDWord() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_SetValue(HKey, MainKey, Key, Value)
'------------------------------------------------------------------------------
  LOCAL lHKey     AS STRING
  LOCAL lMainKey  AS STRING
  LOCAL lKey      AS STRING
  LOCAL lTmp      AS EXT
  LOCAL lValue    AS EXT
  LOCAL tmpHKey   AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lKey
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseNumber lValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              lTmp = Registry_SetDWord(tmpHKey, BYCOPY lMainKey, BYCOPY lKey, BYCOPY lValue)
              FUNCTION = lTmp
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_SetTxtNum() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_SetTxtNum(HKey, MainKey, Key, Value)
'------------------------------------------------------------------------------
  LOCAL lHKey     AS STRING
  LOCAL lMainKey  AS STRING
  LOCAL lKey      AS STRING
  LOCAL lTmp      AS EXT
  LOCAL lValue    AS EXT
  LOCAL tmpHKey   AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lKey
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseNumber lValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              lTmp = Registry_SetTxtNum(tmpHKey, BYCOPY lMainKey, BYCOPY lKey, BYCOPY lValue)
              FUNCTION = lTmp
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_SetTxtBool() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_SetTxtBool(HKey, MainKey, Key, Value)
'------------------------------------------------------------------------------
  LOCAL lHKey     AS STRING
  LOCAL lMainKey  AS STRING
  LOCAL lKey      AS STRING
  LOCAL lTmp      AS EXT
  LOCAL lValue    AS EXT
  LOCAL tmpHKey   AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lMainKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lKey
        IF thinBasic_CheckComma() THEN
          thinBasic_ParseNumber lValue
          IF thinBasic_CheckCloseParens() THEN
            tmpHKey = Registry_ConvertHKey(lHKey)
            IF tmpHKey <> 0 THEN
              lTmp = Registry_SetTxtBool(tmpHKey, BYCOPY lMainKey, BYCOPY lKey, BYCOPY lValue)
              FUNCTION = lTmp
            END IF
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_GetAllKeys() AS STRING
'------------------------------------------------------------------------------
'Syntax: List = REGISTRY_GetAllKeys(HKey, MainKey, Separator)
'------------------------------------------------------------------------------
  LOCAL sHKey       AS STRING
  LOCAL sMainKey    AS STRING
  LOCAL sSeparator  AS STRING

  LOCAL azMainKey AS ASCIIZ * 1024
  LOCAL dwHKey    AS DWORD

  LOCAL parensPresent AS BYTE

  parensPresent = thinBasic_CheckOpenParens_Optional

  thinBasic_ParseString sHKey
  IF thinBasic_CheckComma_Mandatory THEN
    thinBasic_ParseString sMainKey
    IF LEFT$(sMainKey, 1) = "\" THEN sMainKey = LTRIM$(sMainKey, "\")

    IF thinBasic_CheckComma_Optional THEN
      thinBasic_ParseString sSeparator
    ELSE
      sSeparator = $CRLF
    END IF

    dwHKey = Registry_ConvertHKey(sHKey)
    IF thinBasic_ErrorFree() THEN
      azMainKey = sMainKey
      FUNCTION = Registry_GetAllKeys(dwHKey, azMainKey, sSeparator)
    END IF
  END IF

  IF parensPresent THEN thinBasic_CheckCloseParens_Mandatory

END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_DelValue() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_DelValue(HKey, Key, ValueName)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lKey        AS STRING
  LOCAL lTmp        AS EXT
  LOCAL lValueName  AS STRING
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lKey
      IF thinBasic_CheckComma() THEN
        thinBasic_ParseString lValueName
        IF thinBasic_CheckCloseParens() THEN
          tmpHKey = Registry_ConvertHKey(lHKey)
          IF tmpHKey <> 0 THEN
            lTmp = Registry_DelValue(tmpHKey, BYCOPY lKey, BYCOPY lValueName)
            FUNCTION = lTmp
          END IF
        END IF
      END IF
    END IF
  END IF
END FUNCTION

'------------------------------------------------------------------------------
FUNCTION Exec_Registry_DelKey() AS EXT
'------------------------------------------------------------------------------
'Syntax: Number = REGISTRY_DelKey(HKey, SubKey)
'------------------------------------------------------------------------------
  LOCAL lHKey       AS STRING
  LOCAL lSubKey     AS STRING
  LOCAL lTmp        AS EXT
  LOCAL lValueName  AS STRING
  LOCAL tmpHKey     AS DWORD

  IF thinBasic_CheckOpenParens() THEN
    thinBasic_ParseString lHKey
    IF thinBasic_CheckComma() THEN
      thinBasic_ParseString lSubKey
      IF thinBasic_CheckCloseParens() THEN
        tmpHKey = Registry_ConvertHKey(lHKey)
        IF tmpHKey <> 0 THEN
          lTmp = Registry_DelKey(tmpHKey, BYCOPY lSubKey)
          FUNCTION = lTmp
        END IF
      END IF
    END IF
  END IF
END FUNCTION
