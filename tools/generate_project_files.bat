@echo off
REM MileZero — Generate Visual Studio project files
REM Requires: Unreal Engine 5.5 installed, UE5 association set

setlocal

REM Try to find UnrealBuildTool via registry or common paths
set UBT_PATH=

REM Check common Epic Games Launcher install paths
if exist "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
    set UBT_PATH=C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
)
if exist "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
    set UBT_PATH=C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
)

if "%UBT_PATH%"=="" (
    echo ERROR: Could not find UnrealBuildTool. Set UE5_ROOT or install Unreal Engine 5.x.
    echo You can also right-click MileZero.uproject in Explorer and choose "Generate Visual Studio project files".
    exit /b 1
)

echo Generating project files...
"%UBT_PATH%" -projectfiles -project="%~dp0..\MileZero.uproject" -game -engine -progress
echo Done.
