@echo off
setlocal enabledelayedexpansion

set "VERSION=1.0.0"
set "AUTHOR=Niranjan Kuamr K"
set "REPO=hacker1514/notebook"
set "GITHUB_URL=https://github.com/%REPO%"
set "BIN_URL=https://github.com/%REPO%/raw/main/bin"
set "RELEASES_URL=https://github.com/%REPO%/releases/latest/download"
set "INSTALL_DIR=%LOCALAPPDATA%\Notebook"
set "CMD=notebook"

chcp 65001 >nul 2>&1

call :about_notebook

echo.
echo This will install Notebook to: %INSTALL_DIR%
echo.
set /p CONFIRM="Continue with installation? (Y/N): "
if /i not "!CONFIRM!"=="Y" exit /b 0

echo.
echo [^>_^] Installing Notebook...

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

set "BINARY_URL=%RELEASES_URL%/notebook.exe"
set "BINARY_PATH=%INSTALL_DIR%\notebook.exe"

echo [*] Downloading Notebook...
powershell -Command "& {param($u,$o) [Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12; try{Invoke-WebRequest -Uri $u -OutFile $o -UseBasicParsing -ErrorAction Stop; exit 0}catch{exit 1}}" -u "%BINARY_URL%" -o "%BINARY_PATH%"
if errorlevel 1 (
    echo [!] Releases download failed, trying bin folder...
    powershell -Command "& {param($u,$o) [Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12; try{Invoke-WebRequest -Uri $u -OutFile $o -UseBasicParsing -ErrorAction Stop; exit 0}catch{exit 1}}" -u "%BIN_URL%/notebook.exe" -o "%BINARY_PATH%"
    if errorlevel 1 (
        echo [ERROR] Failed to download Notebook binary.
        echo [ERROR] Please check your internet connection and try again.
        pause
        exit /b 1
    )
)

if not exist "%BINARY_PATH%" (
    echo [ERROR] Download failed. Binary not found.
    pause
    exit /b 1
)

for %%I in ("%BINARY_PATH%") do set "FILE_SIZE=%%~zI"
if !FILE_SIZE! LSS 1000 (
    echo [ERROR] Downloaded file is corrupt (too small).
    del "%BINARY_PATH%"
    pause
    exit /b 1
)

echo [OK] Binary downloaded and verified.

set "PATH_CMD=%INSTALL_DIR%"
for /f "skip=2 tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "CURRENT_PATH=%%B"
if "!CURRENT_PATH!"=="" for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "CURRENT_PATH=%%B"
echo !CURRENT_PATH! | find /i "%PATH_CMD%" >nul
if errorlevel 1 (
    setx PATH "!CURRENT_PATH!;%PATH_CMD%" >nul
    echo [OK] Added Notebook to PATH.
) else (
    echo [OK] Notebook already in PATH.
)

powershell -Command "& {$s=(New-Object -ComObject WScript.Shell).CreateShortcut([Environment]::GetFolderPath('StartMenu')+'\Programs\Notebook.lnk');$s.TargetPath='%BINARY_PATH%';$s.WorkingDirectory='%INSTALL_DIR%';$s.Description='Notebook Terminal Text Editor';$s.Save()}" >nul 2>&1
echo [OK] Start Menu shortcut created.

powershell -Command "& {$s=(New-Object -ComObject WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Desktop')+'\Notebook.lnk');$s.TargetPath='%BINARY_PATH%';$s.WorkingDirectory='%INSTALL_DIR%';$s.Description='Notebook Terminal Text Editor';$s.Save()}" >nul 2>&1
echo [OK] Desktop shortcut created.

echo.
echo ============================================
echo   NOTEBOOK INSTALLATION COMPLETE
echo ============================================
echo   Version  : %VERSION%
echo   Author   : %AUTHOR%
echo   Location : %INSTALL_DIR%
echo   Command  : notebook
echo.
echo   Open a new Command Prompt and type: notebook
echo.
echo ============================================
pause
exit /b 0

:about_notebook
cls
echo.
echo  [36m╔══════════════════════════════════════════════════════════════╗[0m
echo  [36m║                      ╔══════════════╗                       ║[0m
echo  [36m║                      ║   NOTEBOOK   ║                       ║[0m
echo  [36m║                      ╚══════════════╝                       ║[0m
echo  [36m║                                                              ║[0m
echo  [36m║  A modern terminal text editor for developers               ║[0m
echo  [36m╚══════════════════════════════════════════════════════════════╝[0m
echo.
echo  [34m╔══════════════════════════════════════════════════════════════════════╗[0m
echo  [34m║                         ABOUT NOTEBOOK                             ║[0m
echo  [34m╠══════════════════════════════════════════════════════════════════════╣[0m
echo  [34m║[0m Project              : [32mNotebook[0m                              [34m║[0m
echo  [34m║[0m Version              : [32m%VERSION%[0m                                [34m║[0m
echo  [34m║[0m Developer            : [32m%AUTHOR%[0m                    [34m║[0m
echo  [34m║[0m License              : [32mMIT[0m                                      [34m║[0m
echo  [34m║[0m GitHub               : [36m%GITHUB_URL%[0m            [34m║[0m
echo  [34m║[0m Global Command       : [33m%CMD%[0m                                  [34m║[0m
echo  [34m║[0m Language             : [37mC (C99)[0m                                 [34m║[0m
echo  [34m╠══════════════════════════════════════════════════════════════════════╣[0m
echo  [34m║[0m Description                                                    [34m║[0m
echo  [34m║[0m A lightweight, modern, cross-platform terminal text editor.    [34m║[0m
echo  [34m╠══════════════════════════════════════════════════════════════════════╣[0m
echo  [34m║[0m Features                                                       [34m║[0m
echo  [34m║[0m  [32m✦[0m Multi-buffer editing                                         [34m║[0m
echo  [34m║[0m  [32m✦[0m Syntax highlighting                                          [34m║[0m
echo  [34m║[0m  [32m✦[0m Search and replace                                            [34m║[0m
echo  [34m║[0m  [32m✦[0m Undo / Redo                                                   [34m║[0m
echo  [34m║[0m  [32m✦[0m Plugin support                                                [34m║[0m
echo  [34m║[0m  [32m✦[0m Themes                                                        [34m║[0m
echo  [34m║[0m  [32m✦[0m Cross-platform                                                [34m║[0m
echo  [34m╠══════════════════════════════════════════════════════════════════════╣[0m
echo  [34m║[0m Supported Platforms                                              [34m║[0m
echo  [34m║[0m  Windows Linux macOS BSD Termux                               [34m║[0m
echo  [34m╠══════════════════════════════════════════════════════════════════════╣[0m
echo  [34m║[0m                 [32mThank you for using Notebook![0m                     [34m║[0m
echo  [34m╚══════════════════════════════════════════════════════════════════════╝[0m
echo.
goto :eof

