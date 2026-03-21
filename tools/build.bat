@echo off
REM MileZero — Build the project (Development Editor target)
REM Requires: Unreal Engine 5.5 installed and Visual Studio

setlocal

set UE_ROOT=
if exist "C:\Program Files\Epic Games\UE_5.5" set UE_ROOT=C:\Program Files\Epic Games\UE_5.5
if exist "C:\Program Files\Epic Games\UE_5.4" set UE_ROOT=C:\Program Files\Epic Games\UE_5.4

if "%UE_ROOT%"=="" (
    echo ERROR: Could not find Unreal Engine installation.
    echo Set UE_ROOT environment variable to your UE5 install directory.
    exit /b 1
)

set UAT="%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat"
set PROJECT_PATH=%~dp0..\MileZero.uproject

echo Building MileZero (Development Editor, Win64)...
call %UAT% BuildCookRun -project="%PROJECT_PATH%" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -prereqs -utf8output
echo Done. Exit code: %ERRORLEVEL%
