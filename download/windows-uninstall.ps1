$ErrorActionPreference = "SilentlyContinue"
$ProgressPreference = "SilentlyContinue"

$INSTALL_DIR = "$env:LOCALAPPDATA\Notebook"

Clear-Host
Write-Host @"
 ${RED}╔══════════════════════════════════════════════════════════════╗
 ${RED}║               UNINSTALLING NOTEBOOK                        ║
 ${RED}╚══════════════════════════════════════════════════════════════╝
"@ -ForegroundColor Red
Write-Host ""

$confirm = Read-Host "Remove Notebook from your system? (Y/N)"
if ($confirm -ne "Y") {
    Write-Host "Uninstall cancelled." -ForegroundColor Yellow
    exit 0
}

if (Test-Path "$INSTALL_DIR\notebook.exe") {
    Remove-Item "$INSTALL_DIR\notebook.exe" -Force
    Write-Host "[OK] Notebook binary removed." -ForegroundColor Green
} else {
    Write-Host "[INFO] Notebook binary not found." -ForegroundColor Yellow
}

if (Test-Path $INSTALL_DIR) {
    Remove-Item $INSTALL_DIR -Recurse -Force
    Write-Host "[OK] Installation directory removed." -ForegroundColor Green
}

$shortcutPaths = @(
    [Environment]::GetFolderPath("StartMenu") + "\Programs\Notebook.lnk",
    [Environment]::GetFolderPath("Desktop") + "\Notebook.lnk",
    "$env:PUBLIC\Desktop\Notebook.lnk"
)

foreach ($path in $shortcutPaths) {
    if (Test-Path $path) {
        Remove-Item $path -Force
        Write-Host "[OK] Shortcut removed: $path" -ForegroundColor Green
    }
}

$currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
if ($currentPath -like "*$INSTALL_DIR*") {
    $newPath = ($currentPath.Split(';') | Where-Object { $_ -ne $INSTALL_DIR }) -join ';'
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
    Write-Host "[OK] Notebook removed from PATH." -ForegroundColor Green
} else {
    Write-Host "[INFO] Notebook not found in PATH." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  NOTEBOOK UNINSTALL COMPLETE" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Notebook has been removed from your system." -ForegroundColor White
Write-Host ""
Write-Host "Close and reopen any open Command Prompt" -ForegroundColor White
Write-Host "or PowerShell windows to update PATH." -ForegroundColor White
Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan

