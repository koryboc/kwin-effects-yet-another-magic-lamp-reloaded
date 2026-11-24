#!/usr/bin/env bash
# Builds and installs kwin-effects-yet-another-magic-lamp-reloaded from source.
#
# Usage: Run this script from the root of the repository.
#   ./install.sh
#
# Supported distributions:
#   Arch Linux  (pacman)  — CachyOS, EndeavourOS, Garuda Linux, Manjaro
#   Debian      (apt)     — KDE neon, Kubuntu, MX Linux, Nitrux
#   Fedora      (dnf)     — Bazzite, Kinoite, Nobara
#   NixOS       (nix)     — user-level install via flake into ~/.local
#   openSUSE    (zypper)  — Tumbleweed
#
# Requires Plasma 6.6 or later.

set -euo pipefail

# ── Colors ────────────────────────────────────────────────────────────────────

nc='\033[0m'
red='\033[0;31m'
green='\033[0;32m'
yellow='\033[1;33m'
white='\033[1;37m'
ol='\033[53m'
ul='\033[4m'

# ── Config ────────────────────────────────────────────────────────────────────

base="$(cd "$(dirname "$0")" && pwd)"
repo=kwin-effects-yet-another-magic-lamp-reloaded

SUDO=$([[ $EUID -eq 0 ]] || echo sudo)

os_name=$(. /etc/os-release 2>/dev/null && echo "${PRETTY_NAME:-$NAME}" || echo "unknown")
echo -e "${ul}${white}System${nc}"
echo -e "  ${green}[OS]${nc}    $os_name"
user_mode=${SUDO:+will use sudo}
echo -e "  ${green}[USER]${nc}  $(id -un) (${user_mode:-root})"

# ── Plasma version check ──────────────────────────────────────────────────────

if command -v plasmashell &>/dev/null; then
    plasma_ver=$(plasmashell --version 2>/dev/null | grep -oP '\d+\.\d+(\.\d+)?' | head -1)
    plasma_major=$(echo "$plasma_ver" | cut -d. -f1)
    plasma_minor=$(echo "$plasma_ver" | cut -d. -f2)
    if [[ "$plasma_major" -lt 6 ]] || { [[ "$plasma_major" -eq 6 ]] && [[ "$plasma_minor" -lt 6 ]]; }; then
        echo -e "${red}[ERROR]${nc} This effect requires Plasma 6.6 or later."
        exit 1
    else
        echo -e "${green}[OK]${nc} Plasma $plasma_ver detected."
    fi
fi

# ── NixOS ─────────────────────────────────────────────────────────────────────

if [[ -f /etc/NIXOS ]] || grep -qi 'ID=nixos' /etc/os-release 2>/dev/null; then
    echo -e "\n${ul}${white}NixOS detected — building via nix flake${nc}\n"

    if ! command -v nix &>/dev/null; then
        echo -e "${red}[ERROR]${nc} 'nix' not found."
        exit 1
    fi

    flake="$base/flake.nix"
    trap 'rm -f "$flake" "$base/flake.lock"; rm -rf "$base/result"' EXIT

    cat > "$flake" <<'EOF'
{
  description = "Yet Another Magic Lamp Reloaded";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      pname = "kwin-effects-yet-another-magic-lamp-reloaded";
      version = "unstable";
      src = ./.;
      dontWrapQtApps = true;
      nativeBuildInputs = with pkgs; [
        cmake ninja pkg-config
        kdePackages.extra-cmake-modules
      ];
      buildInputs = with pkgs; [
        qt6.qtbase
        kdePackages.kwin
        kdePackages.kconfig
        kdePackages.kconfigwidgets
        kdePackages.kcoreaddons
        kdePackages.kcmutils
        kdePackages.kwindowsystem
        libdrm
        vulkan-headers
      ];
      cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];
    };
  };
}
EOF

    nix --extra-experimental-features 'nix-command flakes' \
        build --print-build-logs "$base#default"

    dest="$HOME/.local"
    echo -e "\n${ul}${white}Installing to $dest${nc}\n"
    mkdir -p "$dest"
    cp -rL "$base/result/." "$dest/"
    chmod -R u+w "$dest/lib" "$dest/share" 2>/dev/null || true

    echo -e "\n${green}${ol}$(basename "$0")${white}${ol} done!${nc}"
    echo -e "${yellow}Restart KWin:${nc} kwin_wayland --replace  (or kwin_x11 --replace)\n"
    exit 0
fi

# ── Dependencies ──────────────────────────────────────────────────────────────

if command -v pacman &>/dev/null; then
    pm=pacman; pm_label="Arch Linux (pacman)"
    pkgs=(base-devel cmake extra-cmake-modules kwin
          kconfig kconfigwidgets kcmutils kcoreaddons kwindowsystem
          qt6-base libdrm vulkan-headers)
    is_installed() { pacman -Qq "$1" &>/dev/null; }
    do_install()   { $SUDO pacman -S --needed --noconfirm "${missing[@]}"; }
elif command -v dnf &>/dev/null; then
    pm=dnf; pm_label="Fedora (dnf)"
    pkgs=(cmake extra-cmake-modules kwin-devel
          kf6-kconfig-devel kf6-kconfigwidgets-devel kf6-kcmutils-devel
          kf6-kcoreaddons-devel kf6-kwindowsystem-devel
          qt6-qtbase-devel libdrm-devel libepoxy-devel vulkan-headers)
    is_installed() { rpm -q "$1" &>/dev/null; }
    do_install()   { $SUDO dnf install -y "${missing[@]}"; }
elif command -v zypper &>/dev/null; then
    pm=zypper; pm_label="openSUSE (zypper)"
    pkgs=(gcc-c++ cmake kf6-extra-cmake-modules kwin6-devel
          kf6-kconfig-devel kf6-kconfigwidgets-devel kf6-kcmutils-devel
          kf6-kcoreaddons-devel kf6-kwindowsystem-devel
          qt6-base-devel qt6-declarative-devel libdrm-devel libepoxy-devel
          vulkan-headers)
    is_installed() { rpm -q "$1" &>/dev/null; }
    do_install()   { $SUDO zypper --non-interactive install "${missing[@]}"; }
elif command -v apt &>/dev/null; then
    pm=apt; pm_label="Debian/Ubuntu (apt)"
    pkgs=(build-essential cmake extra-cmake-modules kwin-dev
          libkf6config-dev libkf6configwidgets-dev libkf6coreaddons-dev
          libkf6kcmutils-dev libkf6windowsystem-dev
          qt6-base-dev libdrm-dev libvulkan-dev)
    is_installed() { dpkg -s "$1" &>/dev/null 2>&1; }
    do_install()   { $SUDO apt install -y "${missing[@]}"; }
else
    echo -e "${red}[ERROR]${nc} No supported package manager found (pacman, dnf, zypper, apt)."
    exit 1
fi

echo -e "\n${green}[PM]${nc}    $pm_label"
echo -e "\n${ul}${white}Checking dependencies${nc}\n"

missing=()
for pkg in "${pkgs[@]}"; do
    if is_installed "$pkg"; then
        echo -e "  ${green}[OK]${nc} $pkg"
    else
        echo -e "  ${yellow}[MISSING]${nc} $pkg"
        missing+=("$pkg")
    fi
done

if [[ "${#missing[@]}" -gt 0 ]]; then
    echo -e "\n${yellow}Installing missing packages...${nc}\n"
    do_install
fi

# ── Build ─────────────────────────────────────────────────────────────────────

echo -e "\n${ul}${white}Building $repo${nc}\n"

rm -rf "$base/build"
cmake -S "$base" -B "$base/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$base/build" --parallel "$(nproc)"

$SUDO cmake --install "$base/build"

echo -e "\n${green}${ol}$(basename "$0")${white}${ol} done!${nc}\n"
