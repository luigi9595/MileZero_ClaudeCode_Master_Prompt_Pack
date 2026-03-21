# BUILD_RUNBOOK

## Environment
- Host OS: Windows 11 Pro 10.0.22631
- Unreal Engine version: 5.5 (target; **NOT INSTALLED**)
- Engine install path: TBD — typically C:\Program Files\Epic Games\UE_5.5
- Visual Studio: **NOT INSTALLED**
- Repo root: C:\Users\luigi.quaranta\Downloads\MileZero_ClaudeCode_Master_Prompt_Pack\milezero_master_prompt_pack
- Shell: bash (Git Bash / MSYS2)

## EXACT INSTALL STEPS (this machine)

### 1. Install Epic Games Launcher
Download from https://store.epicgames.com/download and install.

### 2. Install Unreal Engine 5.5
- Open Epic Games Launcher
- Go to "Unreal Engine" tab → "Library"
- Click "+" next to Engine Versions
- Select **5.5.x** (latest)
- Click Install
- Default path: `C:\Program Files\Epic Games\UE_5.5`
- Wait for download + install (30-60 GB)

### 3. Install Visual Studio 2022
Download Visual Studio 2022 Community from https://visualstudio.microsoft.com/
Select these workloads:
- **Game development with C++**
  - Ensure "Windows 10/11 SDK (latest)" is checked
  - Ensure "C++ profiling tools" is checked
- **Desktop development with C++** (optional but helpful)
  - Ensure "MSVC v143 build tools" is checked
Also install:
- **.NET 6.0 SDK** (required by UnrealBuildTool)

### 4. Associate .uproject with UE 5.5
Right-click `MileZero.uproject` → Open With → choose Unreal Engine 5.5
Or: right-click → "Switch Unreal Engine version..." → select 5.5

### 5. Generate project files and compile
```
cd <repo_root>
tools\generate_project_files.bat
tools\compile_check.bat
```

If `tools\generate_project_files.bat` can't find UBT, right-click `MileZero.uproject` in Explorer → "Generate Visual Studio project files".

## Known commands

### Regenerate project files
- Command: `tools\generate_project_files.bat` or right-click MileZero.uproject → "Generate Visual Studio project files"
- Notes: creates MileZero.sln for VS 2022

### Compile check (fast feedback loop)
- Command: `tools\compile_check.bat`
- Notes: **this is the first build gate — use this, not build.bat**

### Open editor
- Command: `tools\open_editor.bat` or double-click MileZero.uproject
- Notes: first launch compiles shaders (5-15 min)

### Automated asset setup
- Command: in editor Python console: `exec(open("tools/setup_editor_assets.py").read())`
- Notes: creates input actions, mapping context, data assets, test map

### Full build (cook + stage)
- Command: `tools\build.bat`
- Notes: **do not use until PIE works first**

### Data validation
- Command: in-editor: Edit → Project Settings → Editor → Data Validation → Validate Data
- Notes: validates all UPrimaryDataAsset subclasses

### Smoke run
- Command: open L_MZ_TestTrack, PIE (Alt+P)
- Notes: verify vehicle spawns, drives, resets

## Common failure signatures
- "Module MileZero not found" → run generate_project_files first
- "ChaosVehiclesPlugin not found" → enable plugin in .uproject or Editor Plugin Browser
- Linker errors on ChaosVehiclesCore → remove from MileZero.Build.cs PrivateDependency (see KI-003)
- BuildSettingsVersion.V5 not found → change to V4 if UE 5.4 (see KI-002)
- WheeledVehiclePawn.h not found → try #include "ChaosVehicles/WheeledVehiclePawn.h" (see KI-004)

## Fixes that worked
- (none yet — pending first compile)
