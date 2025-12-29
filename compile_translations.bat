@echo off
echo Compiling translation files...

REM Try to find lrelease in common Qt installation paths
set LRELEASE=lrelease

REM Check if lrelease is in PATH
where %LRELEASE% >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo lrelease not found in PATH, trying to locate Qt installation...

    REM Common Qt paths (update these if needed)
    if exist "C:\Qt\6.5.0\mingw_64\bin\lrelease.exe" set LRELEASE="C:\Qt\6.5.0\mingw_64\bin\lrelease.exe"
    if exist "C:\Qt\5.15.2\mingw81_64\bin\lrelease.exe" set LRELEASE="C:\Qt\5.15.2\mingw81_64\bin\lrelease.exe"
)

echo Using: %LRELEASE%

REM Compile Romanian translation
%LRELEASE% translations\rtdmaster_ro.ts -qm translations\rtdmaster_ro.qm
if %ERRORLEVEL% EQU 0 (
    echo [OK] Romanian translation compiled
) else (
    echo [ERROR] Failed to compile Romanian translation
)

REM Compile German translation
%LRELEASE% translations\rtdmaster_de.ts -qm translations\rtdmaster_de.qm
if %ERRORLEVEL% EQU 0 (
    echo [OK] German translation compiled
) else (
    echo [ERROR] Failed to compile German translation
)

REM Compile French translation
%LRELEASE% translations\rtdmaster_fr.ts -qm translations\rtdmaster_fr.qm
if %ERRORLEVEL% EQU 0 (
    echo [OK] French translation compiled
) else (
    echo [ERROR] Failed to compile French translation
)

REM Compile Spanish translation
%LRELEASE% translations\rtdmaster_es.ts -qm translations\rtdmaster_es.qm
if %ERRORLEVEL% EQU 0 (
    echo [OK] Spanish translation compiled
) else (
    echo [ERROR] Failed to compile Spanish translation
)

REM Compile Russian translation
%LRELEASE% translations\rtdmaster_ru.ts -qm translations\rtdmaster_ru.qm
if %ERRORLEVEL% EQU 0 (
    echo [OK] Russian translation compiled
) else (
    echo [ERROR] Failed to compile Russian translation
)

echo.
echo Done! You can now build the project in Qt Creator.
pause
