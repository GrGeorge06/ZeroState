# BraveLite - GrGeorge

A fast, lightweight, debloated web browser for Linux and Windows built natively with **C++17** and **Qt6 WebEngine**. Inspired by the clean aesthetics of Brave and Chrome, BraveLite strips away corporate telemetry, crypto bloat, and unwanted background services while delivering advanced privacy features, built-in network shielding, and seamless desktop integration across all major Linux distributions and Windows.

![Version](https://img.shields.io/badge/version-v1.5-orange)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20(Fedora%20%7C%20Ubuntu%20%7C%20Arch%20%7C%20openSUSE%20%7C%20Win10%2F11)-blue)
![License](https://img.shields.io/badge/license-MIT-green)[cite: 7]

---

## Key Features

* **BraveLite Shield (Ad, YouTube & Spam Redirect Blocker):** Full network request interception and DOM shielding that blocks advertising networks, YouTube video ads, pop-under and spam redirects, analytics scripts, and device-maker telemetry (Xiaomi, Realme, Oppo, Huawei, Apple, Samsung) out of the box[cite: 7, 8, 11]. Features a visual indicator button (`🛡 ON` in orange / `🛡 OFF` in gray).
* **Per-Site Privacy Grade (A–F):** Real-time letter grade displayed directly inside the address bar showing site tracking behavior, blocked scripts, and encryption status[cite: 7, 11]. Click the badge for a detailed breakdown report[cite: 7, 11].
* **Start Page with Live Clock:** Centered layout with quick DuckDuckGo search, a real-time updating digital clock, and the tagline *"because privacy matters"*.
* **Self-Hosted Integrated Password Manager & Interactive Autofill:**
  * Prompts *"Do you want to fill credentials?"* when clicking username/email or password boxes on websites with saved logins.
  * Automatically detects form submissions to prompt saving or updating credentials locally[cite: 7, 11].
  * Built-in password manager dialog (`🔑` toolbar button and Settings tab) to view, add, edit, copy, search, and delete credentials[cite: 2, 7, 11].
* **Detachable & Re-Attachable Tabs:** Drag any tab downward off the tab bar to snap it out into an independent window, or drag it back onto another window's tab bar to merge it.
* **Smart Bookmarks Bar:**
  * Background favicon preloading via disk cache (`~/.config/BraveLite/favicons/`) so icons display immediately without clicking.
  * Displays bookmark titles directly next to icons, automatically truncating at the 16th character if too long.
  * Middle-click (scroll-wheel click) any bookmark to open it in a new tab.
  * Right-click context menu to edit name/URL or delete bookmarks[cite: 2].
* **Preserved Fullscreen Navigation:** Fixes YouTube and HTML5 video fullscreen mode while preserving window maximization state upon exit.
* **Persistent Sessions & Logins:** Uses disk-backed persistent storage and cookie policies (`~/.config/BraveLite/Storage`) to keep website logins active after browser restarts[cite: 7, 11].
* **Live Downloads Manager:** Real-time progress bars, live transfer speed (`MB/s` / `KB/s`), pause/resume, cancel, copy download link, and direct folder access.
* **Get Started & Startup Preferences:** Built-in settings page to set BraveLite as your system default browser and choose whether to open a fresh New Tab or continue where you left off.
* **Diagnostic Crash Logs:** Automatic POSIX/Windows signal and renderer process crash logger saving timestamped reports to `~/.config/BraveLite/CrashLogs/`.
* **SponsorBlock Integration:** Automated timestamp-based sponsor skipping on YouTube with support for custom self-hosted API endpoints[cite: 7, 11].
* **DNS over Oblivious HTTP (ODoH):** Toggleable query splitting that separates user IP identity from DNS resolution requests[cite: 7, 11].
* **Cross-Browser Import & Export:** One-click importing from Brave, Google Chrome, Chromium, Mozilla Firefox, Microsoft Edge, or manual file uploads (`.html`, `.json`, `.csv`)[cite: 7, 11].
* **Isolated Incognito Windows:** Memory-only private browsing sessions (`Ctrl+Shift+N`) that store no cookies, caches, or history[cite: 7, 11, 12].

---

## Prerequisites & Distro Dependencies

BraveLite compiles on any modern Linux distribution or Windows environment with standard C++17, CMake, and Qt6 (Widgets, WebEngine, Core, Gui, Network, WebChannel, Positioning)[cite: 4, 7].

### Fedora / RHEL / CentOS Stream
```bash
sudo dnf install -y gcc-c++ cmake ninja-build \
    qt6-qtbase-devel qt6-qtwebengine-devel \
    qt6-qtsvg-devel ImageMagick
```

### Ubuntu / Debian / Linux Mint / Pop!_OS
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build \
    qt6-base-dev qt6-webengine-dev qt6-webengine-dev-tools \
    libqt6svg6-dev imagemagick
```

### Arch Linux / Manjaro / EndeavourOS
```bash
sudo pacman -S --needed base-devel cmake ninja \
    qt6-base qt6-webengine qt6-svg imagemagick
```

### openSUSE (Tumbleweed / Leap)
```bash
sudo zypper install -y gcc-c++ cmake ninja \
    qt6-base-devel qt6-webengine-devel \
    qt6-svg-devel ImageMagick
```

### Alpine Linux
```bash
sudo apk add g++ cmake ninja \
    qt6-qtbase-dev qt6-qtwebengine-dev \
    qt6-qtsvg-dev imagemagick
```

---

## Installation & Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/<your-username>/mybrowser.git
cd mybrowser
```

### 2. Compile the Binary

Compile the source using CMake and Ninja:

```bash
mkdir -p build && cd build
cmake .. -G Ninja
ninja
```

You can test-run the browser directly from the build folder:

```bash
./fedorabrowser
```

---

## Desktop Integration (Install as a System App)

To make BraveLite appear in your application launcher (GNOME, KDE Plasma, XFCE, Cinnamon, or tiling window managers) with its custom icon and automatic update support:

### 1. Link the Binary to Your Local PATH

Creating a symbolic link ensures that whenever you run `ninja`, your installed application updates automatically without having to re-copy files:

```bash
mkdir -p ~/.local/bin
ln -sf $(pwd)/fedorabrowser ~/.local/bin/bravelite
```

*(Ensure `~/.local/bin` is in your `$PATH`. Most modern distros include it by default).*

### 2. Install Desktop Icons

Generate and register the icon across standard resolutions:

```bash
mkdir -p ~/.local/share/icons/hicolor/scalable/apps
mkdir -p ~/.local/share/icons/hicolor/128x128/apps
mkdir -p ~/.local/share/icons/hicolor/256x256/apps
mkdir -p ~/.local/share/icons/hicolor/512x512/apps

cp ../assets/bravelite.svg ~/.local/share/icons/hicolor/scalable/apps/bravelite.svg
magick -background none -resize 128x128 ../assets/bravelite.svg ~/.local/share/icons/hicolor/128x128/apps/bravelite.png
magick -background none -resize 256x256 ../assets/bravelite.svg ~/.local/share/icons/hicolor/256x256/apps/bravelite.png
magick -background none -resize 512x512 ../assets/bravelite.svg ~/.local/share/icons/hicolor/512x512/apps/bravelite.png

gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor 2>/dev/null || true
```

### 3. Create the Desktop Launcher Entry

```bash
mkdir -p ~/.local/share/applications

cat << 'EOF' > ~/.local/share/applications/bravelite.desktop
[Desktop Entry]
Name=BraveLite
GenericName=Web Browser
Comment=Fast, debloated, privacy-respecting browser
Exec=/home/$USER/.local/bin/bravelite %U
Terminal=false
Type=Application
Icon=bravelite
Categories=Network;WebBrowser;
MimeType=text/html;text/xml;application/xhtml+xml;x-scheme-handler/http;x-scheme-handler/https;
StartupNotify=true
StartupWMClass=bravelite
EOF

# Substitute active username and update desktop database
sed -i "s/\$USER/$USER/g" ~/.local/share/applications/bravelite.desktop
update-desktop-database ~/.local/share/applications 2>/dev/null || true
```

BraveLite is now installed and accessible from your system application menu.

---

## Default Keyboard Shortcuts

| Shortcut | Action |
| :--- | :--- |
| `Ctrl + T` | Open a new tab |
| `Ctrl + W` | Close the current tab |
| `Ctrl + N` | Open a new window |
| `Ctrl + Shift + N` | Open an Incognito window |
| `Ctrl + H` | Open the History tab |
| `Ctrl + J` | Open the Downloads tab |
| `Ctrl + +` / `Ctrl + =` | Zoom in |
| `Ctrl + -` | Zoom out |
| `Ctrl + 0` | Reset zoom to 100% |
| `Ctrl + Shift + Del` | Quick clear browsing cookies and history |

---

## File Storage Locations

All user configuration, persistent cookies, and bookmarks are kept locally within your user profile in standard XDG directories:

* **Configuration & Saved Data:** `~/.config/BraveLite/data.json`
* **Persistent Web Storage & Cookies:** `~/.config/BraveLite/Storage/`
* **Desktop App Entry:** `~/.local/share/applications/bravelite.desktop`
* **Application Icons:** `~/.local/share/icons/hicolor/*/apps/bravelite.*`

---

## Updating After Code Changes

Because your executable is symlinked to `~/.local/bin/bravelite`, any updates to the source code can be rebuilt in one step:

```bash
cd build
ninja
```

The next time you open BraveLite from your application menu or terminal, the updated build runs immediately.
