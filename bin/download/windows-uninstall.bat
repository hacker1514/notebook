@echo off
setlocal enabledelayedexpansion

chcp 65001 >nul 2>&1

set "INSTALL_DIR=%LOCALAPPDATA%\Notebook"

echo.
echo  [31m╔══════════════════════════════════════════════════════════════╗[0m
echo  [31m║               UNINSTALLING NOTEBOOK                        ║[0m
echo  [31m╚══════════════════════════════════════════════════════════════╝[0m
echo.

set /p CONFIRM="Remove Notebook from your system? (Y/N): "
if /i not "!CONFIRM!"=="Y" (
    echo Uninstall cancelled.
    exit /b 0
)

if exist "%INSTALL_DIR%\notebook.exe" (
    del /f /q "%INSTALL_DIR%\notebook.exe" >nul 2>&1
    echo [OK] Notebook binary removed.
) else (
    echo [INFO] Notebook binary not found.
)

if exist "%INSTALL_DIR%" (
    rmdir /s /q "%INSTALL_DIR%" >nul 2>&1
    echo [OK] Installation directory removed.
)

for %%i in (
    "%APPDATA%\Microsoft\Windows\Start Menu\Programs\Notebook.lnk"
    "%USERPROFILE%\Desktop\Notebook.lnk"
    "%PUBLIC%\Desktop\Notebook.lnk"
) do (
    if exist "%%i" (
        del /f /q "%%i" >nul 2>&1
        echo [OK] Shortcut removed: %%i
    )
)

set "KEY=HKCU\Environment"
set "PATH_NAME=PATH"
setlocal enableDelayedExpansion
for /f "skip=2 tokens=2*" %%A in ('reg query "%KEY%" /v "%PATH_NAME%" 2^>nul') do set "CURRENT_PATH=%%B"
if "!CURRENT_PATH!"=="" for /f "tokens=2*" %%A in ('reg query "%KEY%" /v "%PATH_NAME%" 2^>nul') do set "CURRENT_PATH=%%B"
if not "!CURRENT_PATH!"=="" (
    set "NEW_PATH=!CURRENT_PATH:;%INSTALL_DIR%=!"
    if not "!NEW_PATH!"=="!CURRENT_PATH!" (
        setx PATH "!NEW_PATH!" >nul
        echo [OK] Notebook removed from PATH.
    ) else (
        echo [INFO] Notebook not found in PATH.
    )
)

echo.
echo ============================================
echo   NOTEBOOK UNINSTALL COMPLETE
echo ============================================
echo.
echo Notebook has been removed from your system.
echo.
echo Close and reopen any open Command Prompt
echo or PowerShell windows to update PATH.
echo.
echo ============================================
pause
exit /b 0

