#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -f "${SCRIPT_DIR}/lib.sh" ]; then source "${SCRIPT_DIR}/lib.sh"; fi
RESET="\033[0m"; RED="\033[31m"; GREEN="\033[32m"; YELLOW="\033[33m"
if [ "$(id -u)" -ne 0 ]; then exec sudo "$0" "$@"; fi
printf "${RED}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${RED}║               UNINSTALLING NOTEBOOK                          ║${RESET}\n"
printf "${RED}╚══════════════════════════════════════════════════════════════╝${RESET}\n"; echo ""
read -p "Remove Notebook from your system? (Y/N): " CONFIRM
if [ "$CONFIRM" != "Y" ] && [ "$CONFIRM" != "y" ]; then printf "${YELLOW}Uninstall cancelled.${RESET}\n"; exit 0; fi
for f in "${INSTALL_DIR}/notebook" "/usr/local/bin/notebook"; do
    if [ -f "$f" ] || [ -L "$f" ]; then rm -f "$f"; printf "${GREEN}✓ Removed: %s${RESET}\n" "$f"; fi
done
printf "${GREEN}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║              NOTEBOOK UNINSTALL COMPLETE                     ║${RESET}\n"
printf "${GREEN}╚══════════════════════════════════════════════════════════════╝${RESET}\n"

