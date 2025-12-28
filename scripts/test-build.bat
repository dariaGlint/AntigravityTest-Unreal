@echo off
REM Test build the plugin in the linked UE5 project

echo ========================================
echo UE5 Plugin Build Test
echo ========================================
echo.

set UE5_ENGINE=C:\Program Files\Epic Games\UE_5.4
set PROJECT=C:\Users\daria\Documents\Antigravity\CodingAgentSandbox\AntigravityTest.uproject

echo Checking UE5 installation...
if not exist "%UE5_ENGINE%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
    echo ERROR: UE5 installation not found at %UE5_ENGINE%
    pause
    exit /b 1
)

echo Checking project file...
if not exist "%PROJECT%" (
    echo ERROR: Project file not found at %PROJECT%
    pause
    exit /b 1
)

echo.
echo Building project with plugin...
echo This may take a few minutes...
echo.

"%UE5_ENGINE%\Engine\Build\BatchFiles\Build.bat" AntigravityTestEditor Win64 Development "%PROJECT%" -WaitMutex -FromMsBuild

if errorlevel 1 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    echo Check the log above for errors
    pause
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL
echo ========================================
echo The plugin compiled successfully!
echo.
pause
