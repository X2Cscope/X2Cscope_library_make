#!/usr/bin/env bash
# Installs a Microchip XC compiler (free mode) on Linux.
#
# Usage: install-xc.sh <xc32|xc-dsc> <version> [download_dir]
#   e.g. install-xc.sh xc32 v4.60
#
# The compiler is installed to /opt/microchip/<name>/<version>. The installer
# is kept in download_dir so CI can cache it between runs.
set -euo pipefail

name=${1:?compiler name (xc32 or xc-dsc) required}
ver=${2:?compiler version (e.g. v4.60) required}
dl=${3:-$HOME/xc-installers}

base=https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools
# XC-DSC has no license options in its installer (free mode is the default)
case $name in
  xc32)   file="xc32-${ver}-full-install-linux-x64-installer.run"
          license_opts=(--netservername localhost --LicenseType FreeMode) ;;
  xc-dsc) file="xc-dsc-${ver}-full-install-linux64-installer.run"
          license_opts=() ;;
  *) echo "unknown compiler: $name" >&2; exit 1 ;;
esac

prefix=/opt/microchip/$name/$ver
cc=$prefix/bin/$name-gcc

if [ -x "$cc" ]; then
  echo "$name $ver already installed in $prefix"
else
  mkdir -p "$dl"
  if [ ! -s "$dl/$file" ]; then
    echo "Downloading $base/$file"
    curl -fL --retry 3 -o "$dl/$file.part" "$base/$file"
    mv "$dl/$file.part" "$dl/$file"
  fi
  chmod +x "$dl/$file"

  sudo=
  [ "$(id -u)" -ne 0 ] && sudo=sudo
  $sudo "$dl/$file" --mode unattended --unattendedmodeui none \
    "${license_opts[@]}" --prefix "$prefix"
fi

"$cc" --version | head -n 1
