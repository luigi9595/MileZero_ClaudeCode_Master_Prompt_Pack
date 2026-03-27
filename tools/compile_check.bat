@echo off
REM MileZero — Quick compile check (editor target, no cook)
REM Faster feedback loop than full build

setlocal

set UE_ROOT=
if exist "C:\Program Files\Epic Games\UE_5.7" set UE_ROOT=C:\Program Files\Epic Games\UE_5.7
if exist "C:\Program Files\Epic Games\UE_5.5" set UE_ROOT=C:\Program Files\Epic Games\UE_5.5
if exist "C:\Program Files\Epic Games\UE_5.4" set UE_ROOT=C:\Program Files\Epic Games\UE_5.4

if "%UE_ROOT%"=="" (
    echo ERROR: Could not find Unreal Engine installation.
    exit /b 1
)

set UBT="%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set PROJECT_PATH=%~dp0..\MileZero.uproject

echo Compiling MileZero (MileZeroEditor, Development, Win64)...
%UBT% MileZeroEditor Win64 Development -Project="%PROJECT_PATH%" -WaitMutex -FromMsBuild
echo Exit code: %ERRORLEVEL%
