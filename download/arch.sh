#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib.sh"
DETECTED_ARCH=$(detect_arch)
if [ "$(id -u)" -ne 0 ]; then exec sudo "$0" "$@"; fi
about_notebook
printf "${YELLOW}Detected OS: ${WHITE}Arch Linux${RESET}\n"
printf "${YELLOW}Detected Architecture: ${WHITE}%s${RESET}\n" "$DETECTED_ARCH"
read -p "Continue with installation? (Y/N): " CONFIRM
if [ "$CONFIRM" != "Y" ] && [ "$CONFIRM" != "y" ]; then printf "${YELLOW}Installation cancelled.${RESET}\n"; exit 0; fi
printf "${CYAN}[_] Installing Notebook on Arch Linux...${RESET}\n"
check_internet || exit 1
BINARY_NAME=$(get_binary_name "linux" "$DETECTED_ARCH")
TMP_DIR=$(mktemp -d)
BINARY_PATH="${TMP_DIR}/notebook"
URLS=("${BIN_URL}/${BINARY_NAME}")
DOWNLOADED=0
for URL in "${URLS[@]}"; do
    printf "${YELLOW}Downloading from: %s${RESET}\n" "$URL"
    if download_file "$URL" "$BINARY_PATH"; then DOWNLOADED=1; break; fi
    printf "${RED}Download failed, trying next source...${RESET}\n"
done
if [ "$DOWNLOADED" -eq 0 ]; then printf "${RED}Failed to download Notebook binary.${RESET}\n"; rm -rf "$TMP_DIR"; exit 1; fi
if ! verify_binary "$BINARY_PATH"; then rm -rf "$TMP_DIR"; exit 1; fi
chmod +x "$BINARY_PATH"; cp "$BINARY_PATH" "${INSTALL_DIR}/notebook"; ln -sf "${INSTALL_DIR}/notebook" /usr/local/bin/notebook
printf "${GREEN}✓ Notebook installed to ${INSTALL_DIR}/notebook${RESET}\n"
printf "${GREEN}✓ Symlink created at /usr/local/bin/notebook${RESET}\n"
printf "${GREEN}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║                NOTEBOOK INSTALLATION COMPLETE                ║${RESET}\n"
printf "${GREEN}╠══════════════════════════════════════════════════════════════╣${RESET}\n"
printf "${GREEN}║ Version   : %-49s                                            ║${RESET}\n" "${VERSION}"
printf "${GREEN}║ Author    : %-49s                                            ║${RESET}\n" "${AUTHOR}"
printf "${GREEN}║ Location  : %-49s                                            ║${RESET}\n" "${INSTALL_DIR}/notebook"
printf "${GREEN}║ Command   : %-49s                                            ║${RESET}\n" "notebook"
printf "${GREEN}║                                                              ║${RESET}\n"
printf "${GREEN}║ %-61s                                                        ║${RESET}\n" "Type 'notebook' to start editing."
printf "${GREEN}╚══════════════════════════════════════════════════════════════╝${RESET}\n"
rm -rf "$TMP_DIR"

