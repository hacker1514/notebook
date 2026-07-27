VERSION="1.0.0"
AUTHOR="Niranjan Kuamr K"
REPO="hacker1514/notebook"
GITHUB_URL="https://github.com/${REPO}"
BIN_URL="https://github.com/${REPO}/raw/main/bin"
RELEASES_URL="https://github.com/${REPO}/releases/latest/download"
LICENSE="MIT"
DESCRIPTION="A lightweight, modern, cross-platform terminal text editor."
FEATURES="Multi-buffer editing|Syntax highlighting|Search & replace|Undo / Redo|Plugin support|Themes|Cross-platform"
PLATFORMS="Windows  Linux  macOS  BSD  Termux"
ARCHS="x86_64  ARM64  ARMv7  i386"
LANG="C (C99)"
BUILD="Static binary"
INSTALL_DIR="/usr/local/bin"
CMD="notebook"
WEBSITE="https://notebook-editor.dev"

RESET="\033[0m"
BOLD="\033[1m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
MAGENTA="\033[35m"
CYAN="\033[36m"
WHITE="\033[37m"

detect_arch() {
    case "$(uname -m)" in
        x86_64|amd64) echo "x86_64" ;;
        aarch64|arm64) echo "arm64" ;;
        armv7l|armhf) echo "armv7" ;;
        i386|i686) echo "i386" ;;
        *) echo "unknown" ;;
    esac
}

detect_os() {
    case "$(uname -s)" in
        Linux)
            if [ -f /etc/os-release ]; then
                . /etc/os-release
                echo "$ID"
            else
                echo "linux"
            fi
            ;;
        Darwin) echo "macos" ;;
        FreeBSD) echo "freebsd" ;;
        OpenBSD) echo "openbsd" ;;
        NetBSD) echo "netbsd" ;;
        *) echo "unknown" ;;
    esac
}

about_notebook() {
    local logo="                          "
    logo="${logo}╔══════════════════════════════════════════════════════════════╗\n"
    logo="${logo}║                      ╔══════════════╗                       ║\n"
    logo="${logo}║                      ║   NOTEBOOK   ║                       ║\n"
    logo="${logo}║                      ╚══════════════╝                       ║\n"
    logo="${logo}║                                                              ║\n"
    logo="${logo}║  A modern terminal text editor for developers                ║\n"
    logo="${logo}╚══════════════════════════════════════════════════════════════╝"

    printf "${CYAN}${logo}${RESET}\n"
    printf "${BLUE}╔══════════════════════════════════════════════════════════════════════╗${RESET}\n"
    printf "${BLUE}║                         ABOUT NOTEBOOK                             ║${RESET}\n"
    printf "${BLUE}╠══════════════════════════════════════════════════════════════════════╣${RESET}\n"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${GREEN}%s${RESET}  ${BLUE}║${RESET}\n" "Project" "Notebook"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${GREEN}%s${RESET}  ${BLUE}║${RESET}\n" "Version" "${VERSION}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${GREEN}%s${RESET}  ${BLUE}║${RESET}\n" "Developer" "${AUTHOR}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${GREEN}%s${RESET}  ${BLUE}║${RESET}\n" "License" "${LICENSE}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${CYAN}%s${RESET}  ${BLUE}║${RESET}\n" "GitHub" "${GITHUB_URL}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${CYAN}%s${RESET}  ${BLUE}║${RESET}\n" "Website" "${WEBSITE}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${YELLOW}%s${RESET}  ${BLUE}║${RESET}\n" "Global Command" "${CMD}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${WHITE}%s${RESET}  ${BLUE}║${RESET}\n" "Language" "${LANG}"
    printf "${BLUE}║${RESET} ${BOLD}%-20s${RESET} : ${WHITE}%s${RESET}  ${BLUE}║${RESET}\n" "Build" "${BUILD}"
    printf "${BLUE}╠══════════════════════════════════════════════════════════════════════╣${RESET}\n"
    printf "${BLUE}║${RESET} ${BOLD}Description${RESET}                                               ${BLUE}║${RESET}\n"
    printf "${BLUE}║${RESET} ${WHITE}%s${RESET}  ${BLUE}║${RESET}\n" "${DESCRIPTION}"
    printf "${BLUE}╠══════════════════════════════════════════════════════════════════════╣${RESET}\n"
    printf "${BLUE}║${RESET} ${BOLD}Features${RESET}                                                  ${BLUE}║${RESET}\n"
    IFS='|' read -ra F <<< "$FEATURES"
    for f in "${F[@]}"; do
        printf "${BLUE}║${RESET}  ${GREEN}✦${RESET} %-57s ${BLUE}║${RESET}\n" "$f"
    done
    printf "${BLUE}╠══════════════════════════════════════════════════════════════════════╣${RESET}\n"
    printf "${BLUE}║${RESET} ${BOLD}Supported Platforms${RESET}                                         ${BLUE}║${RESET}\n"
    printf "${BLUE}║${RESET}  ${CYAN}%s${RESET}  ${BLUE}║${RESET}\n" "${PLATFORMS}"
    printf "${BLUE}║${RESET} ${BOLD}Supported Architectures${RESET}                                       ${BLUE}║${RESET}\n"
    printf "${BLUE}║${RESET}  ${CYAN}%s${RESET}  ${BLUE}║${RESET}\n" "${ARCHS}"
    printf "${BLUE}╠══════════════════════════════════════════════════════════════════════╣${RESET}\n"
    printf "${BLUE}║${RESET}                            ${GREEN}Thank you for using Notebook!${RESET}            ${BLUE}║${RESET}\n"
    printf "${BLUE}╚══════════════════════════════════════════════════════════════════════╝${RESET}\n"
}

spinner() {
    local pid=$1
    local delay=0.1
    local spinstr='|/-\'
    while ps -p "$pid" > /dev/null 2>&1; do
        local temp=${spinstr#?}
        printf " [%c]  " "$spinstr"
        local spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b\b\b"
    done
    printf "    \b\b\b\b"
}

progress_bar() {
    local current=$1
    local total=$2
    local width=50
    local percent=$((current * 100 / total))
    local filled=$((current * width / total))
    local empty=$((width - filled))
    printf "\r${CYAN}[${RESET}"
    printf "%${filled}s" | tr ' ' '█'
    printf "%${empty}s" | tr ' ' '─'
    printf "${CYAN}]${RESET} ${GREEN}%3d%%${RESET}" "$percent"
}

download_file() {
    local url=$1
    local output=$2
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$url" -o "$output" 2>/dev/null
        return $?
    elif command -v wget >/dev/null 2>&1; then
        wget -q "$url" -O "$output" 2>/dev/null
        return $?
    fi
    return 1
}

get_binary_name() {
    local os=$1
    local arch=$2
    case "$os" in
        linux|ubuntu|debian|fedora|arch|alpine|centos|rhel|opensuse|raspberrypi) echo "notebook-linux.bin" ;;
        macos|macos-intel|macos-apple-silicon) echo "notebook-mac.bin" ;;
        freebsd|openbsd|netbsd) echo "notebook-${os}.bin" ;;
        termux) echo "notebook-termux.bin" ;;
        windows) echo "notebook.exe" ;;
        *) echo "notebook.bin" ;;
    esac
}

get_arch_suffix() {
    local arch=$1
    case "$arch" in
        x86_64|amd64) echo "amd64" ;;
        aarch64|arm64) echo "arm64" ;;
        armv7l|armv7|armhf) echo "armv7" ;;
        i386|i686) echo "i386" ;;
        *) echo "unknown" ;;
    esac
}

verify_binary() {
    local file=$1
    if [ ! -f "$file" ]; then
        printf "${RED}✗ Binary not found: %s${RESET}\n" "$file"
        return 1
    fi
    local size
    size=$(stat -c%s "$file" 2>/dev/null || stat -f%z "$file" 2>/dev/null)
    if [ "$size" -lt 1000 ]; then
        printf "${RED}✗ Binary appears corrupt (too small: %d bytes)${RESET}\n" "$size"
        return 1
    fi
    if command -v file >/dev/null 2>&1; then
        local filetype
        filetype=$(file "$file")
        case "$filetype" in
            *ELF*|*Mach-O*|*executable*|*PE*) ;;
            *)
                printf "${YELLOW}⚠ Warning: Unrecognized file type: %s${RESET}\n" "$filetype"
                ;;
        esac
    fi
    printf "${GREEN}✓ Binary verified: %s (%d bytes)${RESET}\n" "$file" "$size"
    return 0
}

check_internet() {
    if ! ping -c 1 -W 2 github.com >/dev/null 2>&1; then
        printf "${RED}✗ No internet connection. Cannot download Notebook.${RESET}\n"
        return 1
    fi
    return 0
}

require_root() {
    if [ "$(id -u)" -ne 0 ]; then
        printf "${YELLOW}⚠ This installer requires root privileges. Re-running with sudo...${RESET}\n"
        exec sudo "$0" "$@"
    fi
}

