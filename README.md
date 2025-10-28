# Fronteras Tower 2 — Proximity Video Installation

An interactive video installation built with openFrameworks that responds to viewer proximity across 12 monitors.

## 🚀 Setup for Collaborators

### Prerequisites
- macOS (or Linux/Windows with adjustments)
- Homebrew (macOS)
- openFrameworks 0.12+
- Git & Git LFS (optional, for video files)

### Installation

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

6. **Generate project files**
   Use openFrameworks projectGenerator or:
   ```bash
   cd ~/openFrameworks/apps/myApps/fronteras-tower-2
   make
   ```

### Adding Video Files

Place `.mp4` or `.mov` files in `bin/data/movies/`. These are ignored by git due to size—use Git LFS or share via cloud storage.

### Running

```bash
cd ~/openFrameworks/apps/myApps/fronteras-tower-2
make RunRelease
```

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