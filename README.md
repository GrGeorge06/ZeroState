# BraveLite - GrGeorge

A fast, lightweight, debloated web browser for Fedora Linux built natively with **C++** and **Qt6 WebEngine**. Inspired by the clean aesthetics of Brave and Chrome, BraveLite strips away corporate telemetry, crypto bloat, and unwanted AI services while delivering advanced privacy features, built-in network shielding, and seamless desktop integration.

![Version](https://img.shields.io/badge/version-v0.84--beta-orange)
![Platform](https://img.shields.io/badge/platform-Fedora%20Linux-blue)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Key Features

* **Built-in Ad & Telemetry Blocker:** Full network request interception blocking advertising networks, analytics scripts, social widgets, and device-maker telemetry (Xiaomi, Realme, Oppo, Huawei, Apple, Samsung) with zero setup.
* **Per-Site Privacy Grade (A–F):** Real-time letter grade displayed directly inside the address bar showing site tracking behavior, blocked scripts, and encryption status. Click the badge for a detailed breakdown report.
* **DuckDuckGo by Default:** Instant search integration via the URL bar and start page with no promotional redirects.
* **Clean, Minimalist UI:**
  * Dark mode theme matching modern desktop styling.
  * Sleek tab bar with hover-only red circular close buttons (`✕`).
  * Convenient top-docked new tab button (`+`).
* **SponsorBlock Video Skipper:** Automated timestamp-based sponsor skipping on YouTube with support for custom self-hosted endpoints.
* **DNS over Oblivious HTTP (ODoH):** Toggleable query splitting that separates your IP identity from DNS resolution requests.
* **Full Data Management in Dedicated Tabs:**
  * **Downloads Manager:** Real-time download progress, persistent history, and one-click access to local folders.
  * **History Hub:** Searchable, persistent browsing history with single-click clearance.
  * **Settings & Passwords:** Local encrypted login autofill with interactive save/edit prompts.
  * **Cross-Browser Import/Export:** One-click importing from Brave, Google Chrome, Chromium, Mozilla Firefox, Microsoft Edge, or manual file uploads (`.html`, `.json`, `.csv`).
* **Isolated Incognito Windows:** Memory-only private browsing sessions (`Ctrl+Shift+N`) that store no cookies, caches, or history.

---

## Prerequisites (Fedora Linux)

Ensure your system has the standard C++ compiler, build tools, and Qt6 WebEngine development libraries installed:

```bash
sudo dnf install -y gcc-c++ cmake ninja-build \
    qt6-qtbase-devel qt6-qtwebengine-devel \
    qt6-qtsvg-devel ImageMagick
```

---

## Installation & Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/<your-username>/mybrowser.git
cd mybrowser
```

### 2. Compile the Binary

Compile the source using CMake and the Ninja build system:

```bash
mkdir -p build && cd build
cmake .. -G Ninja
ninja
```

You can test-run the browser directly from the build directory:

```bash
./fedorabrowser
```

---

## Desktop Integration (Install as a System App)

To make BraveLite appear in your application launcher (GNOME App Grid / KDE Application Launcher) with its custom icon and automatic update support:

### 1. Link the Binary to Your PATH

Creating a symbolic link ensures that whenever you run `ninja`, your installed application updates automatically:

```bash
mkdir -p ~/.local/bin
ln -sf $(pwd)/fedorabrowser ~/.local/bin/bravelite
```

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

gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor
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

# Substitute your active username and update the database
sed -i "s/\$USER/$USER/g" ~/.local/share/applications/bravelite.desktop
update-desktop-database ~/.local/share/applications
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

All user configuration, persistent cookies, and bookmarks are kept locally within your user profile:

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

The next time you open BraveLite from your system menu or terminal, the updated build runs immediately.