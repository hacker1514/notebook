#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -f "${SCRIPT_DIR}/lib.sh" ]; then
    source "${SCRIPT_DIR}/lib.sh"
fi

RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
CYAN="\033[36m"

if [ "$(id -u)" -ne 0 ]; then
    printf "${YELLOW}This uninstaller requires root privileges. Re-running with sudo...${RESET}\n"
    exec sudo "$0" "$@"
fi

INSTALL_DIR="/usr/local/bin"

printf "${RED}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${RED}║               UNINSTALLING NOTEBOOK                        ║${RESET}\n"
printf "${RED}╚══════════════════════════════════════════════════════════════╝${RESET}\n"
echo ""

read -p "Remove Notebook from your system? (Y/N): " CONFIRM
if [ "$CONFIRM" != "Y" ] && [ "$CONFIRM" != "y" ]; then
    printf "${YELLOW}Uninstall cancelled.${RESET}\n"
    exit 0
fi

if [ -f "${INSTALL_DIR}/notebook" ]; then
    rm -f "${INSTALL_DIR}/notebook"
    printf "${GREEN}✓ Removed: ${INSTALL_DIR}/notebook${RESET}\n"
else
    printf "${YELLOW}⚠ Binary not found: ${INSTALL_DIR}/notebook${RESET}\n"
fi

if [ -L "/usr/local/bin/notebook" ]; then
    rm -f "/usr/local/bin/notebook"
    printf "${GREEN}✓ Removed symlink: /usr/local/bin/notebook${RESET}\n"
fi

if command -v update-alternatives >/dev/null 2>&1; then
    update-alternatives --remove notebook "${INSTALL_DIR}/notebook" 2>/dev/null || true
fi

printf "${GREEN}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║              NOTEBOOK UNINSTALL COMPLETE                   ║${RESET}\n"
printf "${GREEN}╚══════════════════════════════════════════════════════════════╝${RESET}\n"
printf "${YELLOW}Notebook has been removed from your system.${RESET}\n"

