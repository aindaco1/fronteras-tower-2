# Fronteras Tower 2 — Proximity Video Installation

An interactive video installation built with openFrameworks that responds to viewer proximity across 12 monitors.

## 🚀 Setup for Collaborators

### Prerequisites

- **macOS**: Homebrew, openFrameworks 0.12+
- **Windows 10**: Visual Studio 2022 (Community Edition is free), openFrameworks 0.12+
- Git & Git LFS (optional, for video files)

---

### 🍎 macOS Installation

1. **Install openFrameworks**

   ```bash
   cd ~/Downloads
   curl -L https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_osx_release.zip -o of_v0.12.0_osx_release.zip
   unzip of_v0.12.0_osx_release.zip
   mv of_v0.12.0_osx_release ~/openFrameworks
   ```

2. **Install dependencies**

   ```bash
   brew install glfw pkg-config
   ```

3. **Clone this repository into openFrameworks apps folder**

   ```bash
   cd ~/openFrameworks/apps/myApps
   git clone https://github.com/aindaco1/fronteras-tower-2.git
   cd fronteras-tower-2
   ```

4. **Download haarcascade file**

   ```bash
   cd ~/openFrameworks/apps/myApps/fronteras-tower-2
   cp ~/openFrameworks/examples/computer_vision/opencvHaarFinderExample/bin/data/haarcascade_frontalface_default.xml bin/data/
   ```

5. **Generate project files with projectGenerator**
   - Open `~/openFrameworks/projectGenerator/projectGenerator.app`
   - Click "Import" and browse to `~/openFrameworks/apps/myApps/fronteras-tower-2`
   - Make sure "ofxOpenCv" appears in the addons list
   - Click "Update" to generate Xcode project

6. **Configure Xcode project**

   Open the generated project:
   
   ```bash
   open ~/openFrameworks/apps/myApps/fronteras-tower-2/fronteras-tower-2.xcodeproj
   ```

   Then in Xcode:
   - Select the project in the navigator (top-level "fronteras-tower-2")
   - Go to **Build Phases** → **Compile Sources**
   - **Remove** `ofApp.cpp` and `ofApp.h` (select and click the `-` button)
   - Go to **Build Phases** → **Link Binary With Libraries**
   - Find and **remove** the `AGL.framework` entry (click the `-` button)
   - In the **Project Navigator** (left sidebar), right-click the **src** folder → **Add Files to "fronteras-tower-2"...**
   - Select all source files: `DisplayApp.cpp`, `DisplayApp.h`, `DisplayManager.cpp`, `DisplayManager.h`, `main.cpp`
   - Press `⌘B` to build and verify it compiles successfully

---

### 🪟 Windows 10 Installation

1. **Install Visual Studio 2022**
   - Download [Visual Studio Community 2022](https://visualstudio.microsoft.com/downloads/)
   - During install, select "Desktop development with C++"
   - Include: MSVC, Windows SDK, C++ CMake tools

2. **Download openFrameworks**
   - Go to [openFrameworks downloads](https://openframeworks.cc/download/)
   - Download **of_v0.12.0_vs2022_release.zip**
   - Extract to `C:\openFrameworks\`

3. **Clone this repository directly into openFrameworks**

   ⚠️ **Important:** Clone directly into the openFrameworks apps folder (not elsewhere):

   ```powershell
   cd C:\openFrameworks\apps\myApps
   git clone https://github.com/aindaco1/fronteras-tower-2.git
   cd fronteras-tower-2
   ```

   *Why:* OpenFrameworks projects must be inside the OF directory structure to properly link libraries.

4. **Copy haarcascade face detection file**

   ```powershell
   copy C:\openFrameworks\addons\ofxOpenCv\libs\opencv\etc\haarcascades\haarcascade_frontalface_default.xml bin\data\
   ```

5. **Create temporary files for projectGenerator**

   ProjectGenerator expects standard OF file names. Create these temporarily:

   ```powershell
   cd src
   New-Item -ItemType File ofApp.cpp
   New-Item -ItemType File ofApp.h
   cd ..
   ```

6. **Generate project files with projectGenerator**
   
   ```powershell
   C:\openFrameworks\projectGenerator\projectGenerator.exe
   ```

   - Click "Import" and browse to `C:\openFrameworks\apps\myApps\fronteras-tower-2`
   - Make sure **ofxOpenCv** is listed in addons
   - Click "Update" to generate Visual Studio solution
   - Close projectGenerator

7. **Open in Visual Studio and configure**
   
   ```powershell
   .\fronteras-tower-2.sln
   ```

   Once Visual Studio opens:
   - In **Solution Explorer**, right-click on `ofApp.cpp` → **Remove** (do NOT delete)
   - Right-click on `ofApp.h` → **Remove**
   - Right-click the **src** folder → **Add → Existing Item...**
   - Select `DisplayApp.cpp`, `DisplayApp.h`, `DisplayManager.cpp`, `DisplayManager.h`, `main.cpp`
   - Right-click the project → **Properties** → **Debugging** → set **Working Directory** to: `$(ProjectDir)bin`
   - Click **Apply** and **OK**

8. **Build and run**
   - Set build configuration to "Debug"
   - Press **Ctrl+Shift+B** to build
   - Press **F5** to run

### Adding Video Files

Place `.mp4` or `.mov` files in `bin/data/movies/`. These are ignored by git due to size—use Git LFS or share via cloud storage.

### Running

**macOS:**

```bash
open ~/openFrameworks/apps/myApps/fronteras-tower-2/fronteras-tower-2.xcodeproj
```

- Press `⌘R` to build and run

**Windows:**

- Open the generated `.sln` file in Visual Studio
- Press `F5` or click "Local Windows Debugger"

## 📁 Project Structure

```
fronteras-tower-2/
├── src/                  # C++ source files
├── bin/data/
│   ├── movies/          # Video files (not in git)
│   ├── shaders/         # GLSL shaders
│   └── haarcascade...   # Face detection model
├── addons.make          # Required OF addons
└── config.make          # Build configuration
```

## 🔄 Collaboration Workflow

**Important:** Each platform generates its own project files (`.xcodeproj` for macOS, `.sln` for Windows). These are ignored by git.

**When you make changes:**
1. Edit code in `~/openFrameworks/apps/myApps/fronteras-tower-2` (macOS) or `C:\openFrameworks\apps\myApps\fronteras-tower-2` (Windows)
2. Commit and push only source files (`src/`, `bin/data/`, `addons.make`, etc.)
3. Project files stay local to your machine

**When you pull changes:**
1. Pull the latest code
2. Your existing project files (`.xcodeproj` or `.sln`) automatically reference the updated source
3. No need to regenerate project files unless `addons.make` changes

**What gets shared via git:**
- ✅ Source code (`src/`)
- ✅ Shaders (`bin/data/shaders/`)
- ✅ Configuration files (`addons.make`, `config.make`)
- ✅ Documentation (`README.md`, `agents.md`)
- ❌ Build outputs (`bin/*.app`, `bin/*.exe`)
- ❌ Platform-specific project files (`.xcodeproj`, `.sln`)
- ❌ Video files (share separately via cloud storage)

## 🎨 Technical Details

See [agents.md](agents.md) for full architectural documentation.