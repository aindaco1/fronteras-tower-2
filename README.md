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

3. **Clone this repository**

   ```bash
   git clone https://github.com/aindaco1/fronteras-tower-2.git
   cd fronteras-tower-2
   ```

4. **Symlink to openFrameworks apps folder**

   ```bash
   ln -s $(pwd) ~/openFrameworks/apps/myApps/fronteras-tower-2
   ```

5. **Download haarcascade file**

   ```bash
   cp ~/openFrameworks/data/haarcascades/haarcascade_frontalface_default.xml bin/data/
   ```

6. **Generate project files with projectGenerator**
   - Open `~/openFrameworks/projectGenerator`
   - Click "Import" and select this project folder
   - Click "Update" to generate Xcode project

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

3. **Clone this repository**

   ```bash
   git clone https://github.com/aindaco1/fronteras-tower-2.git
   cd fronteras-tower-2
   ```

4. **Copy project to openFrameworks folder**

   Open **Command Prompt** or **PowerShell** and navigate to where you cloned the repo, then run:

   ```bash
   xcopy /E /I . C:\openFrameworks\apps\myApps\fronteras-tower-2
   ```

   *What this does:* Copies the entire project into openFrameworks' apps folder where it can access OF libraries.
   
   **Alternative (File Explorer):**
   - Copy the `fronteras-tower-2` folder
   - Paste it into `C:\openFrameworks\apps\myApps\`
   - Result should be: `C:\openFrameworks\apps\myApps\fronteras-tower-2\`

5. **Download haarcascade face detection file**

   In **Command Prompt**, navigate to the project folder and run:

   ```bash
   cd C:\openFrameworks\apps\myApps\fronteras-tower-2
   copy C:\openFrameworks\data\haarcascades\haarcascade_frontalface_default.xml bin\data\
   ```

   *What this does:* Copies OpenCV's face detection model into your project's data folder so the webcam can detect faces.
   
   **Alternative (File Explorer):**
   - Navigate to `C:\openFrameworks\data\haarcascades\`
   - Copy `haarcascade_frontalface_default.xml`
   - Paste into `C:\openFrameworks\apps\myApps\fronteras-tower-2\bin\data\`

6. **Generate project files with projectGenerator**
   - Navigate to `C:\openFrameworks\projectGenerator\`
   - Run `projectGenerator.exe`
   - Click "Import" and browse to `C:\openFrameworks\apps\myApps\fronteras-tower-2`
   - Make sure "ofxOpenCv" is listed in addons
   - Click "Update" to generate Visual Studio solution

7. **Open and build**
   - Open the generated `.sln` file in Visual Studio
   - Set build configuration to "Release" or "Debug"
   - Press `F5` to build and run

### Adding Video Files

Place `.mp4` or `.mov` files in `bin/data/movies/`. These are ignored by git due to size—use Git LFS or share via cloud storage.

### Running

**macOS:**

```bash
cd ~/openFrameworks/apps/myApps/fronteras-tower-2
make RunRelease
```

**Windows:**

- Open the `.sln` file in Visual Studio
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

## 🎨 Technical Details

See [agents.md](agents.md) for full architectural documentation.