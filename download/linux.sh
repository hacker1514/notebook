#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib.sh"

DETECTED_OS=$(detect_os)
DETECTED_ARCH=$(detect_arch)

require_root "$@"

about_notebook

echo ""
printf "${YELLOW}Detected OS: ${WHITE}%s${RESET}\n" "$DETECTED_OS"
printf "${YELLOW}Detected Architecture: ${WHITE}%s${RESET}\n" "$DETECTED_ARCH"
echo ""

case "$DETECTED_OS" in
    ubuntu|debian|linuxmint|pop|elementary|zorin)
        if command -v apt-get >/dev/null 2>&1; then
            DISTRO="debian"
        fi
        ;;
    fedora) DISTRO="fedora" ;;
    arch|manjaro|endeavouros|artix) DISTRO="arch" ;;
    alpine) DISTRO="alpine" ;;
    centos|rhel|rocky|almalinux) DISTRO="rhel" ;;
    opensuse|suse) DISTRO="opensuse" ;;
    raspbian) DISTRO="raspberrypi" ;;
    *)
        DISTRO="linux"
        ;;
esac

printf "${YELLOW}Distribution: ${WHITE}%s${RESET}\n" "$DISTRO"
echo ""

read -p "Continue with installation? (Y/N): " CONFIRM
if [ "$CONFIRM" != "Y" ] && [ "$CONFIRM" != "y" ]; then
    printf "${YELLOW}Installation cancelled.${RESET}\n"
    exit 0
fi

echo ""
printf "${CYAN}[_] Installing Notebook...${RESET}\n"
echo ""

check_internet || exit 1

BINARY_NAME=$(get_binary_name "linux" "$DETECTED_ARCH")
ARCH_SUFFIX=$(get_arch_suffix "$DETECTED_ARCH")

TMP_DIR=$(mktemp -d)
BINARY_PATH="${TMP_DIR}/notebook"

URLS=(
    "${RELEASES_URL}/${BINARY_NAME}"
    "${BIN_URL}/${BINARY_NAME}"
)

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
cp "$BINARY_PATH" "${INSTALL_DIR}/notebook"

if [ -f /etc/ld.so.conf.d/ ]; then
    ldconfig 2>/dev/null || true
fi

if [ ! -L /usr/local/bin/notebook ]; then
    ln -sf "${INSTALL_DIR}/notebook" /usr/local/bin/notebook 2>/dev/null || true
fi

printf "${GREEN}✓ Notebook installed to ${INSTALL_DIR}/notebook${RESET}\n"
printf "${GREEN}✓ Symlink created at /usr/local/bin/notebook${RESET}\n"

which notebook >/dev/null 2>&1 && printf "${GREEN}✓ Global command 'notebook' is available${RESET}\n"

echo ""
printf "${GREEN}"
printf "╔══════════════════════════════════════════════════════════════╗\n"
printf "║              NOTEBOOK INSTALLATION COMPLETE                  ║\n"
printf "╠══════════════════════════════════════════════════════════════╣\n"
printf "║ Version   : ${VERSION}                                       ║\n"
printf "║ Author    : ${AUTHOR}                                        ║\n"
printf "║ Location  : ${INSTALL_DIR}/notebook                          ║\n"
printf "║ Command   : notebook                                         ║\n"
printf "║                                                              ║\n"
printf "║ Type 'notebook' to start editing.                            ║\n"
printf "╚══════════════════════════════════════════════════════════════╝\n"
printf "${RESET}"

rm -rf "$TMP_DIR"

