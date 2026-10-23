# BraveLite — By GrGeorge

**A fast, lightweight, debloated, privacy-first web browser for Linux and Windows.**

Built natively in **C++17** and **Qt6 WebEngine**, BraveLite strips away corporate telemetry, tracking, advertising bloat, and unwanted background services. It delivers built-in uBlock-grade shielding, zero-ad YouTube playback, local AI assistance, native SearXNG metasearch, multi-language UI, and direct Chrome Web Store extension support.

---

<p align="center">
  <img src="assets/bravelite.svg" alt="BraveLite Logo" width="120" height="120" />
</p>

<p align="center">
  <a href="https://github.com/GrGeorge06/braveLite/releases"><img src="https://img.shields.io/badge/version-v1.72%20(Complete%20Release)-brightgreen?style=for-the-badge" alt="Release v1.72"></a>
  <a href="https://github.com/GrGeorge06/braveLite/releases"><img src="https://img.shields.io/badge/status-Out%20of%20Beta-success?style=for-the-badge" alt="Out of Beta"></a>
  <img src="https://img.shields.io/badge/platform-Linux%20%7C%20Windows-blue?style=for-the-badge" alt="Platform Support">
  <a href="LICENSE.txt"><img src="https://img.shields.io/badge/license-MIT-blueviolet?style=for-the-badge" alt="License MIT"></a>
</p>

> 🎉 **Officially Out of Beta!**
> Welcome to **BraveLite Complete Release v1.72**. All beta experimental flags have graduated to stable release status, featuring robust multi-platform stability, fluid DPI scaling, seamless tab management, and complete Windows & Linux desktop integration.

---

## Table of Contents

- [Quick Installation Guide](#quick-installation-guide)
  - [Windows Installation](#-windows-installation)
  - [Linux Installation](#-linux-installation)
- [Building from Source](#building-from-source)
  - [Linux (Fedora, Ubuntu, Arch, openSUSE)](#linux-build)
  - [Windows (MSVC & Ninja)](#windows-build)
- [Linux Desktop Integration (App Menu & Icons)](#linux-desktop-integration)
- [Key Features](#key-features)
- [Default Keyboard Shortcuts](#default-keyboard-shortcuts)
- [File & Storage Locations](#file--storage-locations)
- [License](#license)

---

## Quick Installation Guide

### 🪟 Windows Installation

#### Option 1: Automated Installer (Recommended)
1. Go to the [**BraveLite Releases**](https://github.com/GrGeorge06/braveLite/releases) page.
2. Download the latest **`BraveLite-Setup-x64.exe`**.
3. Run the installer. It will:
   - Install BraveLite to your local application folder.
   - Automatically bundle all necessary Qt6 and WebEngine dependencies.
   - Create a desktop shortcut and a Start Menu entry.
4. Launch **BraveLite** directly from your desktop or Start Menu!

---

### 🐧 Linux Installation

You can build and install BraveLite in a couple of minutes on any modern Linux distribution.

#### 1. Install System Dependencies

Select your distribution below and run the command in your terminal:

* **Fedora / RHEL / CentOS Stream:**
  ```bash
  sudo dnf install -y gcc-c++ cmake ninja-build \
      qt6-qtbase-devel qt6-qtwebengine-devel \
      qt6-qtsvg-devel ImageMagick
  ```

* **Ubuntu / Debian / Linux Mint / Pop!_OS:**
  ```bash
  sudo apt update
  sudo apt install -y build-essential cmake ninja-build \
      qt6-base-dev qt6-webengine-dev qt6-webengine-dev-tools \
      libqt6svg6-dev imagemagick
  ```

* **Arch Linux / Manjaro / EndeavourOS:**
  ```bash
  sudo pacman -S --needed base-devel cmake ninja \
      qt6-base qt6-webengine qt6-svg imagemagick
  ```

* **openSUSE (Tumbleweed / Leap):**
  ```bash
  sudo zypper install -y gcc-c++ cmake ninja \
      qt6-base-devel qt6-webengine-devel \
      qt6-svg-devel ImageMagick
  ```

* **Alpine Linux:**
  ```bash
  sudo apk add g++ cmake ninja \
      qt6-qtbase-dev qt6-qtwebengine-dev \
      qt6-qtsvg-dev imagemagick
  ```

#### 2. Clone and Compile

```bash
# Clone the repository
git clone https://github.com/GrGeorge06/braveLite.git
cd braveLite

# Configure and compile with Ninja
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

#### 3. Test Run

Launch the compiled browser immediately:
```bash
./build/fedorabrowser
```

---

## Linux Desktop Integration

To install BraveLite permanently so it appears in your system application launcher (GNOME, KDE Plasma, XFCE, Cinnamon, etc.) with its official high-resolution icon:

Run this automated setup block inside the `braveLite` project directory:

```bash
# 1. Create executable symlink in ~/.local/bin
mkdir -p ~/.local/bin
ln -sf "$(pwd)/build/fedorabrowser" ~/.local/bin/bravelite

# 2. Install application icons across standard resolutions
mkdir -p ~/.local/share/icons/hicolor/scalable/apps
mkdir -p ~/.local/share/icons/hicolor/128x128/apps
mkdir -p ~/.local/share/icons/hicolor/256x256/apps
mkdir -p ~/.local/share/icons/hicolor/512x512/apps

cp assets/bravelite.svg ~/.local/share/icons/hicolor/scalable/apps/bravelite.svg
magick -background none -resize 128x128 assets/bravelite.svg ~/.local/share/icons/hicolor/128x128/apps/bravelite.png
magick -background none -resize 256x256 assets/bravelite.svg ~/.local/share/icons/hicolor/256x256/apps/bravelite.png
magick -background none -resize 512x512 assets/bravelite.svg ~/.local/share/icons/hicolor/512x512/apps/bravelite.png
gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor 2>/dev/null || true

# 3. Create Desktop Launcher entry
mkdir -p ~/.local/share/applications
cat << EOF > ~/.local/share/applications/bravelite.desktop
[Desktop Entry]
Name=BraveLite
GenericName=Web Browser
Comment=Fast, debloated, privacy-respecting browser
Exec=$HOME/.local/bin/bravelite %U
Terminal=false
Type=Application
Icon=bravelite
Categories=Network;WebBrowser;
MimeType=text/html;text/xml;application/xhtml+xml;x-scheme-handler/http;x-scheme-handler/https;
StartupNotify=true
StartupWMClass=bravelite
EOF

update-desktop-database ~/.local/share/applications 2>/dev/null || true
```

BraveLite is now registered as a native desktop application! You can launch it by searching for **BraveLite** in your applications menu.

---

## Building from Source

### Linux Build
```bash
git clone https://github.com/GrGeorge06/braveLite.git
cd braveLite
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

Whenever you pull code updates, simply re-run:
```bash
ninja -C build
```
Because `~/.local/bin/bravelite` links directly to your build binary, your installed desktop app updates instantly.

---

### Windows Build

#### Prerequisites:
- **Visual Studio 2022** (with *Desktop development with C++*)
- **CMake** (3.16+) and **Ninja**
- **Qt 6.7+** (with `qtwebengine`, `qtwebchannel`, `qtpositioning`)
- *(Optional)* **Inno Setup 6** (to produce `BraveLite-Setup-x64.exe`)

#### Compilation Steps:
1. Open the **x64 Native Tools Command Prompt for VS 2022**.
2. Clone the repository and navigate into it:
   ```cmd
   git clone https://github.com/GrGeorge06/braveLite.git
   cd braveLite
   ```
3. Configure CMake with your Qt directory:
   ```cmd
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64"
   ```
4. Build the executable:
   ```cmd
   ninja -C build
   ```
5. Deploy Qt runtime libraries:
   ```cmd
   mkdir dist
   copy build\fedorabrowser.exe dist\BraveLite.exe
   C:\Qt\6.7.3\msvc2019_64\bin\windeployqt.exe --release dist\BraveLite.exe
   ```
6. *(Optional)* Build the Inno Setup installer:
   ```cmd
   "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer.iss
   ```

---

## Key Features

- 🛡️ **Zero-Ad Experience & uBlock-Grade Shields:** Built-in network request interception and DOM shielding that eliminates advertisements, trackers, pop-unders, and preroll/midroll video ads on YouTube out of the box with zero third-party extensions required.
- 🔍 **Native Metasearch & Multi-Engine Switching:** Default local SearXNG metasearch integration (`http://localhost:8080/`) alongside 1-click toggling between **SearXNG**, **Google**, **Microsoft Bing**, **DuckDuckGo**, **Yahoo**, and **Yandex**.
- 🧩 **Chrome Web Store Native Installer:** Browse the [Chrome Web Store](https://chromewebstore.google.com/) and click "Add to Chrome" to install extensions directly into BraveLite, with full support for unpacked WebExtensions and custom userscripts (`.user.js`).
- 🤖 **Offline Local AI Assistant & Page Summarization:** Built-in side chat panel powered by local Ollama LLMs (`smollm2`, `qwen2.5`, `llama3.2`). Instant link AI summaries with `Ctrl` + hover, 100% private with no remote cloud telemetry.
- 🌐 **Comprehensive Multi-Language Support:** Full browser localization across 6 languages: **English**, **Greek (Ελληνικά)**, **Spanish (Español)**, **French (Français)**, **German (Deutsch)**, and **Chinese (中文)**. Dynamically synchronizes settings, start page clock, omnibox suggestions, and HTTP `Accept-Language` headers.
- 📑 **Modern Tab Bar & Window Mechanics:** Chrome-style dynamic tab scaling (from 185px to 28px) prevents tabs from ever disappearing. Supports detachable tabs, drag-and-drop merging between windows, split-view tabs, and complete session recovery ("Continue where you left off").
- 🪟 **Fluid Border Resizing:** Smooth click-and-drag window resizing from all 4 borders and corners (`WM_NCHITTEST` on Windows, native system resize on Linux).
- 🔑 **Self-Hosted Password Manager & Autofill:** Secure local credentials vault with automatic login form prompt-to-fill and submission detection.
- 🎵 **Spotify Web Playback & Widevine DRM:** Out-of-the-box Widevine CDM support for DRM-protected streaming on Spotify and Netflix (`libwidevinecdm.so` on Linux, `widevinecdm.dll` on Windows).
- 🎨 **Real-Time Instant Theme Engine:** Five built-in themes (BraveLite Dark, Midnight OLED, Cyberpunk Neon, Nordic Frost, Catppuccin Mocha) with instant switching without restarting.
- 🔎 **Ubiquitous Floating Find in Page (`Ctrl+F`):** Floating top-right search pill with real-time match counter (`1/3`), navigation arrows, and DOM auto-scrolling.
- 🔒 **Isolated Incognito Sessions:** Memory-only private browsing (`Ctrl+Shift+N`) leaving zero traces of cookies, cache, or history on disk.

---

## Default Keyboard Shortcuts

| Shortcut | Action |
| :--- | :--- |
| `Ctrl + T` | Open a new tab |
| `Ctrl + W` | Close the current tab |
| `Ctrl + N` | Open a new window |
| `Ctrl + Shift + N` | Open an Incognito window |
| `Ctrl + F` | Find in page |
| `Ctrl + H` | Open History |
| `Ctrl + J` | Open Downloads |
| `Ctrl + D` | Add bookmark |
| `Ctrl + Shift + O` | Open Bookmarks manager |
| `Ctrl + +` / `Ctrl + =` | Zoom in |
| `Ctrl + -` | Zoom out |
| `Ctrl + 0` | Reset zoom (100%) |
| `Ctrl + Shift + Del` | Quick clear cookies, cache, and history |
| `Alt + Left` | Back |
| `Alt + Right` | Forward |
| `Ctrl + R` / `F5` | Reload page |
| `F11` | Toggle Fullscreen |

---

## File & Storage Locations

BraveLite stores configuration and cache locally in standard OS profile directories:

### Linux
* **Settings & Bookmarks:** `~/.config/BraveLite/data.json`
* **Persistent Web Storage & Cookies:** `~/.config/BraveLite/Storage/`
* **Crash Logs:** `~/.config/BraveLite/CrashLogs/`
* **Desktop Launcher:** `~/.local/share/applications/bravelite.desktop`

### Windows
* **Settings & Storage:** `%LOCALAPPDATA%\BraveLite\` and `%APPDATA%\BraveLite\`
* **Installed Location:** `%LOCALAPPDATA%\Programs\BraveLite\` or `C:\Program Files\BraveLite\`

---

## License

This project is licensed under the [MIT License](LICENSE.txt).
Developed with ❤️ by **GrGeorge**.
