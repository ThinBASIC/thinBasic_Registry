' **************************************************************************
' * Registry_GetAllValues
' * Retrieves all values for given key
' *
' * hKey        - registry key
' **************************************************************************
FUNCTION Registry_GetAllValues(hKey AS DWORD) AS STRING
  DIM returnValue AS LONG
  DIM azKeyName AS ASCIIZ * 32767, azKeyNameLen AS DWORD
  DIM azKeyValue AS ASCIIZ * 1048576, azKeyValueLen AS DWORD
  DIM KeyType AS DWORD

  DIM Reserved AS DWORD
  DIM lIndex AS DWORD

  DIM sKeyValue          AS STRING
  DIM items(1 TO 1024)   AS STRING
  DIM itemCount          AS LONG

  DO WHILE returnValue <> %ERROR_NO_MORE_ITEMS
    azKeyNameLen  = SIZEOF(azKeyName)
    azKeyValueLen = SIZEOF(azKeyValue)

    Reserved = %NULL
    returnValue = RegEnumValue(hKey, lIndex, azKeyName, BYVAL VARPTR(azKeyNameLen), BYVAL Reserved, BYVAL VARPTR(KeyType), azKeyValue, BYVAL VARPTR(azKeyValueLen))
    INCR lIndex

    IF returnValue = %ERROR_SUCCESS THEN
      SELECT CASE keyType
        CASE %REG_DWORD
          sKeyValue = FORMAT$(CVDWD(azKeyValue))

        CASE %REG_QWORD
          sKeyValue = FORMAT$(CVQ(azKeyValue))

        CASE ELSE
          sKeyValue = LEFT$(azKeyValue, azKeyValueLen)
      END SELECT

      INCR itemCount
      IF itemCount > UBOUND(items) THEN
        REDIM PRESERVE items(1 TO UBOUND(items) * 2)
      END IF
      items(itemCount) = TRIM$(azKeyName) + "=" + sKeyValue
    ELSEIF returnValue <> %ERROR_NO_MORE_ITEMS THEN
        DIM errorMessage AS STRING
        errorMessage = "RegEnumValue fails with error code " + FORMAT$(returnValue) + $CRLF + _
                       "Check error code meaning at https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx"

        thinbasic_RunTimeError ( %ERR__MODULE_SPECIFIC, errorMessage )
        EXIT FUNCTION
    END IF
  LOOP

  IF itemCount THEN
    REDIM PRESERVE items(1 TO itemCount)
    ARRAY SORT items()

    FUNCTION = JOIN$(items(), $CRLF)
  END IF
END FUNCTION

' **************************************************************************
' * Registry_GetAllKeys
' * Retrieves all keys for given registry path
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * lHive       - Name of a subkey. This parameter cannot be NULL
' * sSep        - Separator of the returned items
' **************************************************************************
FUNCTION Registry_GetAllKeys(BYVAL lHKey AS DWORD, lHive AS ASCIIZ, BYVAL sSep AS STRING) AS STRING
  DIM returnValue AS LONG
  DIM azSubKeyName AS ASCIIZ * 1024, lSubKeyNameLen AS DWORD
  DIM azClass AS ASCIIZ * 1024, lClassLen AS DWORD
  DIM Reserved AS DWORD
  DIM LastWriteTime AS FILETIME
  DIM lIndex AS DWORD
  DIM hKey AS DWORD

  DIM result AS STRING

  returnValue = RegOpenKeyEx(lHKey, lHive, 0, BYVAL %KEY_READ, hKey)

  IF returnValue <> %ERROR_SUCCESS THEN
      IF returnValue = %ERROR_FILE_NOT_FOUND THEN
         EXIT FUNCTION
      END IF

      DIM errorMessage AS STRING
      errorMessage = "RegOpenKeyEx fails for " + lHive + ", error code " + FORMAT$(returnValue) + $CRLF + _
                     "Check error code meaning at https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx"

      thinbasic_RunTimeError ( %ERR__MODULE_SPECIFIC, errorMessage )
  END IF

  result = Registry_GetAllValues(hKey)

  ' Enumerate all the subkeys for the current key.
  '...............................................
  DO WHILE returnValue <> %ERROR_NO_MORE_ITEMS
     IF returnValue = %ERROR_SUCCESS THEN
        lSubKeyNameLen = SIZEOF(azSubKeyName)
        lClassLen = SIZEOF(azClass)
        returnValue = RegEnumKeyEx(hKey, lIndex, azSubKeyName, BYVAL VARPTR(lSubKeyNameLen), Reserved, BYVAL VARPTR(azClass), BYVAL VARPTR(lClasslen), LastWriteTime)

        IF (returnValue <> %ERROR_NO_MORE_ITEMS) THEN
           IF returnValue = %ERROR_SUCCESS THEN
              result += IIF$(LEN(result), sSep, "") + azSubKeyName
           END IF
        END IF
     END IF
     INCR lIndex
  LOOP

  RegCloseKey hKey

  FUNCTION = result

END FUNCTION

' **************************************************************************
' * Registry_SetValue
' * Set a string registry value
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * szSubKey    - Name of a subkey. This parameter cannot be NULL
' * szValueName - Name of the value to set
' * szValue     - Data to be stored
' **************************************************************************
FUNCTION Registry_SetValue(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, szValue AS ASCIIZ) AS LONG
  LOCAL returnValue      AS LONG
  LOCAL hOpenKey  AS LONG

  IF RegCreateKeyEx(hKey, szSubKey, %NULL, "", %REG_OPTION_NON_VOLATILE, _
                    %KEY_WRITE, BYVAL %NULL, hOpenKey, returnValue) <> %ERROR_SUCCESS THEN
    EXIT FUNCTION
  END IF

  returnValue = RegSetValueEx(hOpenKey, szValueName, %NULL, %REG_SZ, szValue, LEN(szValue) + 1)
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_SetDWord
' * Set a binary registry value - very usefull to save UDTs to the registry
' *
' * hKey         - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * szSubKey     - Name of a subkey. This parameter cannot be NULL
' * szValueName  - Name of the value to set
' * dwValue      - Dword value to store
' **************************************************************************
FUNCTION Registry_SetDWord(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, BYVAL dwValue AS DWORD) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG

  IF RegCreateKeyEx( hKey, szSubKey, %NULL, "", %REG_OPTION_NON_VOLATILE, _
                    %KEY_WRITE, BYVAL %NULL, hOpenKey, returnValue) <> %ERROR_SUCCESS THEN
    EXIT FUNCTION
  END IF

  returnValue = RegSetValueEx(hOpenKey, szValueName, %NULL, %REG_DWORD, dwValue, SIZEOF(dwValue))
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_SetBin
' * Set a binary registry value - very usefull to save UDTs to the registry
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to set
' * BinValuePtr - Pointer to a buffer containing the data to be stored
' * lBufferSize - Specifies the size, in bytes, of the buffer
' **************************************************************************
FUNCTION Registry_SetBin(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, BYVAL BinValuePtr AS LONG, BYVAL lBufferSize AS LONG) AS LONG
  LOCAL returnValue      AS LONG
  LOCAL hOpenKey  AS LONG

  IF RegCreateKeyEx(hKey, szSubKey, %NULL, "", %REG_OPTION_NON_VOLATILE, _
                    %KEY_WRITE, BYVAL %NULL, hOpenKey, returnValue) <> %ERROR_SUCCESS THEN
    EXIT FUNCTION
  END IF

  returnValue = RegSetValueEx(hOpenKey, szValueName, %NULL, %REG_BINARY, BYVAL BinValuePtr, lBufferSize)
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_SetTxtNum
' * Set a registry value
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to query
' **************************************************************************
FUNCTION Registry_SetTxtNum(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, BYVAL lValue AS LONG) AS LONG
  LOCAL returnValue AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL szTmp       AS ASCIIZ * 64

  FUNCTION = %FALSE
  IF RegCreateKeyEx( hKey, szSubKey, %NULL, "", %REG_OPTION_NON_VOLATILE, _
                    %KEY_WRITE, BYVAL %NULL, hOpenKey, returnValue) <> %ERROR_SUCCESS THEN
    EXIT FUNCTION
  END IF

  szTmp = FORMAT$(lValue)
  returnValue = RegSetValueEx(hOpenKey, szValueName, %NULL, %REG_SZ, szTmp, LEN(szTmp)+1)
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_SetTxtBool
' * Set a registry value
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to query
' **************************************************************************
FUNCTION Registry_SetTxtBool(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, BYVAL lValue AS LONG) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL szTmp       AS ASCIIZ * 64

  FUNCTION = %FALSE
  IF RegCreateKeyEx( hKey, szSubKey, %NULL, "", %REG_OPTION_NON_VOLATILE, _
                    %KEY_WRITE, BYVAL %NULL, hOpenKey, returnValue) <> %ERROR_SUCCESS THEN
    EXIT FUNCTION
  END IF

  szTmp = IIF$(lValue = %TRUE, "Yes", "No")
  returnValue = RegSetValueEx(hOpenKey, szValueName, %NULL, %REG_SZ, szTmp, LEN(szTmp)+1)
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_GetValue
' * Get a string registry value
' *
' * hKey       - One of the following equates:
' *               %HKEY_CLASSES_ROOT,
' *               %HKEY_CURRENT_CONFIG
' *               %HKEY_CURRENT_USER
' *               %HKEY_LOCAL_MACHINE
' *               %HKEY_USERS
' * zSubKey    - Name of a subkey. This parameter cannot be NULL
' * zValueName - Name of the value to query
' **************************************************************************
FUNCTION Registry_GetValue( _
                               BYVAL hLocation   AS DWORD, _
                               BYVAL sSubKeys    AS STRING, _
                               BYVAL sValueName  AS STRING, _
                      OPTIONAL BYVAL sDefault    AS STRING _
                     ) AS STRING

  LOCAL hKey    AS DWORD
  LOCAL zRegVal AS ASCIIZ * 1024
  LOCAL dwType  AS DWORD
  LOCAL dwSize  AS DWORD

  IF hLocation = 0 THEN hLocation = %HKEY_CURRENT_USER

  zRegVal = sDefault
  IF (RegOpenKeyEx(hLocation, TRIM$(sSubKeys, "\"), 0, %KEY_READ, hKey) = %ERROR_SUCCESS) THEN
     dwType = %REG_SZ
     dwSize = SIZEOF(zRegVal)
     RegQueryValueEx(hKey, BYCOPY sValueName, 0, dwType, zRegVal, dwSize)
     RegCloseKey hKey
  END IF

  FUNCTION = zRegVal
END FUNCTION

' **************************************************************************
' * Registry_KeyExists
' * Check if registry key exists
' *
' * hKey       - One of the following equates:
' *               %HKEY_CLASSES_ROOT,
' *               %HKEY_CURRENT_CONFIG
' *               %HKEY_CURRENT_USER
' *               %HKEY_LOCAL_MACHINE
' *               %HKEY_USERS
' * zSubKey    - Name of a subkey. This parameter cannot be NULL
' * zValueName - Name of the value to query
' **************************************************************************
FUNCTION Registry_KeyExists(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ) AS LONG
  LOCAL returnValue        AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL sBuffer     AS STRING
  LOCAL lBufferSize AS LONG

  returnValue = RegOpenKeyEx(hKey, szSubKey, %Null, %KEY_QUERY_VALUE, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    returnValue = RegQueryValueEx(hOpenKey, szValueName, %Null, %REG_SZ, BYVAL %Null, lBufferSize)
    IF returnValue = %ERROR_SUCCESS THEN
      FUNCTION = %TRUE
    ELSE
      FUNCTION = -2&
    END IF
    RegCloseKey(hOpenKey)
  ELSE
    FUNCTION = -1&
  END IF
END FUNCTION

' **************************************************************************
' * Registry_PathExists
' * Check if registry key exists
' *
' * hKey       - One of the following equates:
' *               %HKEY_CLASSES_ROOT,
' *               %HKEY_CURRENT_CONFIG
' *               %HKEY_CURRENT_USER
' *               %HKEY_LOCAL_MACHINE
' *               %HKEY_USERS
' * zSubKey    - Name of a subkey. This parameter cannot be NULL
' * zValueName - Name of the value to query
' **************************************************************************
FUNCTION Registry_PathExists(BYVAL hKey AS LONG, szSubKey AS ASCIIZ) AS LONG
  LOCAL returnValue        AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL sBuffer     AS STRING
  LOCAL lBufferSize AS LONG

  returnValue = RegOpenKeyEx(hKey, szSubKey, %Null, %KEY_QUERY_VALUE, hOpenKey)
  RegCloseKey(hOpenKey)

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_GetDWord
' * Get a Dword registry value
' *
' * hKey       - One of the following equates:
' *               %HKEY_CLASSES_ROOT,
' *               %HKEY_CURRENT_CONFIG
' *               %HKEY_CURRENT_USER
' *               %HKEY_LOCAL_MACHINE
' *               %HKEY_USERS
' * zSubKey    - Name of a subkey. This parameter cannot be NULL
' * zValueName - Name of the value to query
' **************************************************************************
FUNCTION Registry_GetDWord(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ) AS DWORD
  LOCAL returnValue        AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL dwTmp       AS DWORD
  LOCAL lBufferSize AS LONG

  returnValue = RegOpenKeyEx(hKey, szSubKey, %NULL, %KEY_QUERY_VALUE, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    lBufferSize = SIZEOF(dwTmp)
    returnValue = RegQueryValueEx(hOpenKey, szValueName, %NULL, %REG_DWORD, dwTmp, lBufferSize)
    RegCloseKey(hOpenKey)
    FUNCTION = dwTmp
  END IF

END FUNCTION

' **************************************************************************
' * Registry_GetBin
' * Get a binary registry value - very usefull to get UDTs from the registry
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to query
' * BinValuePtr - Pointer to a buffer that receives the value's data
' * lBufferSize - Specifies the size, in bytes, of the buffer
' **************************************************************************
FUNCTION Registry_GetBin(BYVAL hKey AS LONG, szSubKey AS ASCIIZ, szValueName AS ASCIIZ, BYVAL BinValuePtr AS LONG, BYVAL lBufferSize AS LONG) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG

  FUNCTION = %FALSE

  returnValue = RegOpenKeyEx(hKey, szSubKey, %NULL, %KEY_QUERY_VALUE, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    returnValue = RegQueryValueEx( hOpenKey, szValueName, %NULL, %REG_BINARY, BYVAL BinValuePtr, lBufferSize)
    IF returnValue = %ERROR_SUCCESS THEN
      FUNCTION = %TRUE
    END IF
    RegCloseKey(hOpenKey)
  END IF
END FUNCTION

' **************************************************************************
' * Registry_GetTxtNum
' * Get a number from a registry value stored as text
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to query
' **************************************************************************
FUNCTION Registry_GetTxtNum(BYVAL hKey AS LONG, zSubKey AS ASCIIZ, zValueName AS ASCIIZ, BYVAL lDefVal AS LONG) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL szTmp       AS ASCIIZ * 64
  LOCAL lBufferSize AS LONG

  returnValue = RegOpenKeyEx(hKey, zSubKey, %NULL, %KEY_QUERY_VALUE, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    lBufferSize = SIZEOF(szTmp)
    returnValue = RegQueryValueEx(hOpenKey, zValueName, %NULL, %REG_SZ, szTmp, lBufferSize)
    IF LEN(szTmp) > 0 THEN
      FUNCTION = VAL(szTmp)
    ELSE
      FUNCTION = lDefVal
    END IF
    RegCloseKey(hOpenKey)
  ELSE
    FUNCTION = lDefVal
  END IF

END FUNCTION

' **************************************************************************
' * Registry_GetTxtBool
' * Get a number from a registry value stored as text
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to query
' **************************************************************************
FUNCTION Registry_GetTxtBool(BYVAL hKey AS LONG, zSubKey AS ASCIIZ, zValueName AS ASCIIZ, BYVAL lDefVal AS LONG) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG
  LOCAL zText       AS ASCIIZ * 2048
  LOCAL lBufferSize AS LONG

  returnValue = RegOpenKeyEx(hKey, zSubKey, %NULL, %KEY_QUERY_VALUE, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    lBufferSize = SIZEOF(zText)
    returnValue = RegQueryValueEx( hOpenKey, zValueName, %NULL, %REG_SZ, zText, lBufferSize)
    IF LEN(zText) > 0 THEN
      IF UCASE$(zText) = "YES" OR UCASE$(zText) = "TRUE" THEN
        FUNCTION = %TRUE
      END IF
    ELSE
      FUNCTION = lDefVal
    END IF
  ELSE
    FUNCTION = lDefVal
  END IF

  RegCloseKey hOpenKey

END FUNCTION

' **************************************************************************
' * Registry_DelValue
' * Delete a registry value
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey. This parameter cannot be NULL
' * zValueName  - Name of the value to remove
' **************************************************************************
FUNCTION Registry_DelValue(BYVAL hKey AS LONG, zSubKey AS ASCIIZ, zValueName AS ASCIIZ) AS LONG
  LOCAL returnValue     AS LONG
  LOCAL hOpenKey    AS LONG

  FUNCTION = %FALSE

  returnValue = RegOpenKeyEx(hKey, zSubKey, %NULL, %KEY_ALL_ACCESS, hOpenKey)
  IF returnValue = %ERROR_SUCCESS THEN
    returnValue = RegDeleteValue(hOpenKey, zValueName)
  END IF

  RegCloseKey hOpenKey

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION

' **************************************************************************
' * Registry_DelKey
' * Delete a registry key
' *
' * hKey        - One of the following equates:
' *                %HKEY_CLASSES_ROOT,
' *                %HKEY_CURRENT_CONFIG
' *                %HKEY_CURRENT_USER
' *                %HKEY_LOCAL_MACHINE
' *                %HKEY_USERS
' * zSubKey     - Name of a subkey to remove. This parameter cannot be NULL
' **************************************************************************
FUNCTION Registry_DelKey(BYVAL hKey AS LONG, zSubKey AS ASCIIZ) AS LONG
  LOCAL returnValue   AS LONG
  LOCAL hOpenKey  AS LONG

  FUNCTION = %FALSE

  IF RegOpenKeyEx(hKey, zSubKey, %NULL, %KEY_ALL_ACCESS, hOpenKey) <> %ERROR_SUCCESS THEN EXIT FUNCTION
  returnValue = RegDeleteKey(hKey, zSubKey)
  RegCloseKey hOpenKey

  FUNCTION = (returnValue = %ERROR_SUCCESS)
END FUNCTION
'------------------------------------------------------------------------------
