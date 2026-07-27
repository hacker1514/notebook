param(
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

$VERSION = "1.0.0"
$AUTHOR = "Niranjan Kuamr K"
$REPO = "hacker1514/notebook"
$GITHUB_URL = "https://github.com/$REPO"
$BIN_URL = "https://hacker514.github.io/nootebook/bin"
$INSTALL_DIR = "$env:LOCALAPPDATA\Notebook"
$BINARY_PATH = "$INSTALL_DIR\notebook.exe"

$Host.UI.RawUI.ForegroundColor = "Cyan"

function Write-About {
    Clear-Host
    Write-Host @"
 ${CYAN}╔══════════════════════════════════════════════════════════════╗
 ${CYAN}║                      ╔══════════════╗                       ║
 ${CYAN}║                      ║   NOTEBOOK   ║                       ║
 ${CYAN}║                      ╚══════════════╝                       ║
 ${CYAN}║                                                              ║
 ${CYAN}║  A modern terminal text editor for developers               ║
 ${CYAN}╚══════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

    Write-Host @"
 ${BLUE}╔══════════════════════════════════════════════════════════════════════╗
 ${BLUE}║                         ABOUT NOTEBOOK                             ║
 ${BLUE}╠══════════════════════════════════════════════════════════════════════╣
 ${BLUE}║${RESET} Project              : ${GREEN}Notebook${RESET}                              ${BLUE}║
 ${BLUE}║${RESET} Version              : ${GREEN}$VERSION${RESET}                                ${BLUE}║
 ${BLUE}║${RESET} Developer            : ${GREEN}$AUTHOR${RESET}                    ${BLUE}║
 ${BLUE}║${RESET} License              : ${GREEN}MIT${RESET}                                      ${BLUE}║
 ${BLUE}║${RESET} GitHub               : ${CYAN}$GITHUB_URL${RESET}            ${BLUE}║
 ${BLUE}║${RESET} Global Command       : ${YELLOW}notebook${RESET}                                  ${BLUE}║
 ${BLUE}╠══════════════════════════════════════════════════════════════════════╣
 ${BLUE}║${RESET} Description                                                    ${BLUE}║
 ${BLUE}║${RESET} A lightweight, modern, cross-platform terminal text editor.    ${BLUE}║
 ${BLUE}╠══════════════════════════════════════════════════════════════════════╣
 ${BLUE}║${RESET} Features                                                       ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Multi-buffer editing                                         ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Syntax highlighting                                          ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Search and replace                                            ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Undo / Redo                                                   ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Plugin support                                                ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Themes                                                        ${BLUE}║
 ${BLUE}║${RESET}  ${GREEN}✦${RESET} Cross-platform                                                ${BLUE}║
 ${BLUE}╠══════════════════════════════════════════════════════════════════════╣
 ${BLUE}║${RESET} Supported Platforms                                              ${BLUE}║
 ${BLUE}║${RESET}  Windows Linux macOS BSD Termux                               ${BLUE}║
 ${BLUE}╠══════════════════════════════════════════════════════════════════════╣
 ${BLUE}║${RESET}                 ${GREEN}Thank you for using Notebook!${RESET}                     ${BLUE}║
 ${BLUE}╚══════════════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Blue
    Write-Host ""
}

function Show-ProgressBar {
    param($Current, $Total, $Label)
    $width = 50
    $percent = [math]::Round($Current / $Total * 100)
    $filled = [math]::Floor($Current / $Total * $width)
    $empty = $width - $filled
    $bar = "█" * $filled + "─" * $empty
    Write-Progress -Activity $Label -Status "$percent% Complete" -PercentComplete $percent
    Write-Host "`r${CYAN}[${RESET}${bar}${CYAN}]${RESET} ${GREEN}${percent}%${RESET} $Label" -NoNewline
}

try {
    Write-About

    $confirm = Read-Host "Continue with installation? (Y/N)"
    if ($confirm -ne "Y") { exit 0 }

    Write-Host "`n[Installing Notebook...]`n" -ForegroundColor Yellow

    if (-not (Test-Path $INSTALL_DIR)) {
        New-Item -ItemType Directory -Path $INSTALL_DIR -Force | Out-Null
    }

    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

    $urls = @("$BIN_URL/notebook.exe")
    $downloaded = $false
    $binaryPath = $BINARY_PATH

    foreach ($url in $urls) {
        try {
            Write-Host "Downloading from: $url" -ForegroundColor Yellow
            Invoke-WebRequest -Uri $url -OutFile $binaryPath -UseBasicParsing -ErrorAction Stop
            $downloaded = $true
            break
        } catch {
            Write-Host "Download failed, please check your internet connection." -ForegroundColor Red
            continue
        }
    }

    if (-not $downloaded) {
        throw "Failed to download Notebook binary from all sources."
    }

    $fileInfo = Get-Item $binaryPath
    if ($fileInfo.Length -lt 1000) {
        Remove-Item $binaryPath -Force
        throw "Downloaded file is corrupt (size: $($fileInfo.Length) bytes)."
    }

    Write-Host "[OK] Binary downloaded and verified. ($($fileInfo.Length) bytes)" -ForegroundColor Green

    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    if ($currentPath -notlike "*$INSTALL_DIR*") {
        [Environment]::SetEnvironmentVariable("PATH", "$currentPath;$INSTALL_DIR", "User")
        Write-Host "[OK] Added Notebook to PATH." -ForegroundColor Green
    } else {
        Write-Host "[OK] Notebook already in PATH." -ForegroundColor Cyan
    }

    $wshell = New-Object -ComObject WScript.Shell
    $startMenuPath = [Environment]::GetFolderPath("StartMenu") + "\Programs\Notebook.lnk"
    $shortcut = $wshell.CreateShortcut($startMenuPath)
    $shortcut.TargetPath = $binaryPath
    $shortcut.WorkingDirectory = $INSTALL_DIR
    $shortcut.Description = "Notebook Terminal Text Editor"
    $shortcut.Save()
    Write-Host "[OK] Start Menu shortcut created." -ForegroundColor Green

    $desktopPath = [Environment]::GetFolderPath("Desktop") + "\Notebook.lnk"
    $shortcut = $wshell.CreateShortcut($desktopPath)
    $shortcut.TargetPath = $binaryPath
    $shortcut.WorkingDirectory = $INSTALL_DIR
    $shortcut.Description = "Notebook Terminal Text Editor"
    $shortcut.Save()
    Write-Host "[OK] Desktop shortcut created." -ForegroundColor Green

    Write-Host ""
    Write-Host "============================================" -ForegroundColor Cyan
    Write-Host "  NOTEBOOK INSTALLATION COMPLETE" -ForegroundColor Green
    Write-Host "============================================" -ForegroundColor Cyan
    Write-Host "  Version  : $VERSION" -ForegroundColor White
    Write-Host "  Author   : $AUTHOR" -ForegroundColor White
    Write-Host "  Location : $INSTALL_DIR" -ForegroundColor White
    Write-Host "  Command  : notebook" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  Open a new Command Prompt or PowerShell" -ForegroundColor White
    Write-Host "  window and type: notebook" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "============================================" -ForegroundColor Cyan

} catch {
    Write-Host "[ERROR] $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "`nInstallation failed. Please check your internet connection and try again." -ForegroundColor Red
    exit 1
}

