#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib.sh"

DETECTED_ARCH=$(detect_arch)

about_notebook

echo ""
printf "${YELLOW}Detected OS: ${WHITE}macOS${RESET}\n"
printf "${YELLOW}Detected Architecture: ${WHITE}%s${RESET}\n" "$DETECTED_ARCH"
echo ""

read -p "Continue with installation? (Y/N): " CONFIRM
if [ "$CONFIRM" != "Y" ] && [ "$CONFIRM" != "y" ]; then
    printf "${YELLOW}Installation cancelled.${RESET}\n"
    exit 0
fi

echo ""
printf "${CYAN}[_] Installing Notebook on macOS...${RESET}\n"
echo ""

check_internet || exit 1

BINARY_NAME=$(get_binary_name "macos" "$DETECTED_ARCH")

TMP_DIR=$(mktemp -d)
BINARY_PATH="${TMP_DIR}/notebook"

URLS=("${BIN_URL}/${BINARY_NAME}")
DOWNLOADED=0
for URL in "${URLS[@]}"; do
    printf "${YELLOW}Downloading from: %s${RESET}\n" "$URL"
    if download_file "$URL" "$BINARY_PATH"; then
        DOWNLOADED=1
        break
    fi
    printf "${RED}Download failed, trying next source...${RESET}\n"
done

if [ "$DOWNLOADED" -eq 0 ]; then
    printf "${RED}Failed to download Notebook binary from all sources.${RESET}\n"
    rm -rf "$TMP_DIR"
    exit 1
fi

if ! verify_binary "$BINARY_PATH"; then
    rm -rf "$TMP_DIR"
    exit 1
fi

chmod +x "$BINARY_PATH"

if [ "$(id -u)" -ne 0 ]; then
    printf "${YELLOW}Root privileges required. Re-running with sudo...${RESET}\n"
    sudo cp "$BINARY_PATH" /usr/local/bin/notebook
else
    cp "$BINARY_PATH" /usr/local/bin/notebook
fi

printf "${GREEN}✓ Notebook installed to /usr/local/bin/notebook${RESET}\n"
which notebook > /dev/null 2>&1 && printf "${GREEN}✓ Global command 'notebook' is available${RESET}\n"

echo ""
printf "${GREEN}╔══════════════════════════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║                NOTEBOOK INSTALLATION COMPLETE               ║${RESET}\n"
printf "${GREEN}╠══════════════════════════════════════════════════════════════╣${RESET}\n"
printf "${GREEN}║ Version   : %-49s║${RESET}\n" "${VERSION}"
printf "${GREEN}║ Author    : %-49s║${RESET}\n" "${AUTHOR}"
printf "${GREEN}║ Location  : %-49s║${RESET}\n" "/usr/local/bin/notebook"
printf "${GREEN}║ Command   : %-49s║${RESET}\n" "notebook"
printf "${GREEN}║                                                              ║${RESET}\n"
printf "${GREEN}║ %-61s║${RESET}\n" "Type 'notebook' to start editing."
printf "${GREEN}╚══════════════════════════════════════════════════════════════╝${RESET}\n"

rm -rf "$TMP_DIR"
