@echo off
REM MileZero — Open the project in Unreal Editor

setlocal

set UE_EDITOR=
if exist "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" set UE_EDITOR=C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe
if exist "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" set UE_EDITOR=C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe

if "%UE_EDITOR%"=="" (
    echo ERROR: Could not find UnrealEditor.exe.
    echo Try double-clicking MileZero.uproject instead.
    exit /b 1
)

echo Opening MileZero in Unreal Editor...
start "" "%UE_EDITOR%" "%~dp0..\MileZero.uproject"
