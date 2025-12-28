@echo off
REM UE5 Plugin Symlink Creator
REM This script creates a symbolic link from this worktree's plugin to your UE5 project

setlocal enabledelayedexpansion

echo ========================================
echo UE5 Plugin Symlink Creator
echo ========================================
echo.

REM Get the current worktree path
set "WORKTREE_DIR=%~dp0.."
set "WORKTREE_DIR=%WORKTREE_DIR:\=/%"

REM Ask for plugin name
set /p PLUGIN_NAME="Enter plugin name (e.g., ScreenTransitionSystem): "

REM Ask for UE5 project path
set /p UE5_PROJECT="Enter your UE5 project path (e.g., C:\MyProject): "

REM Remove trailing slash if exists
if "%UE5_PROJECT:~-1%"=="\" set "UE5_PROJECT=%UE5_PROJECT:~0,-1%"

REM Set paths
set "SOURCE_PATH=%WORKTREE_DIR%\Plugins\%PLUGIN_NAME%"
set "TARGET_PATH=%UE5_PROJECT%\Plugins\%PLUGIN_NAME%"

echo.
echo Source: %SOURCE_PATH%
echo Target: %TARGET_PATH%
echo.

REM Check if source exists
if not exist "%SOURCE_PATH%" (
    echo ERROR: Plugin "%PLUGIN_NAME%" not found in worktree
    echo Path: %SOURCE_PATH%
    pause
    exit /b 1
)

REM Create Plugins directory in UE5 project if it doesn't exist
if not exist "%UE5_PROJECT%\Plugins" (
    echo Creating Plugins directory in UE5 project...
    mkdir "%UE5_PROJECT%\Plugins"
)

REM Check if target already exists
if exist "%TARGET_PATH%" (
    echo WARNING: Target already exists!
    echo Do you want to remove it and create a new symlink?
    set /p CONFIRM="(y/n): "
    if /i not "!CONFIRM!"=="y" (
        echo Cancelled.
        pause
        exit /b 0
    )

    REM Remove existing directory/symlink
    rmdir "%TARGET_PATH%" 2>nul
    if exist "%TARGET_PATH%" (
        rd /s /q "%TARGET_PATH%"
    )
)

echo.
echo Creating symbolic link...
echo NOTE: This requires administrator privileges
echo.

REM Create symbolic link
mklink /D "%TARGET_PATH%" "%SOURCE_PATH%"

if errorlevel 1 (
    echo.
    echo ERROR: Failed to create symbolic link
    echo Please run this script as Administrator
    echo.
    echo Alternative: Right-click this file and select "Run as administrator"
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS!
echo ========================================
echo Symbolic link created successfully!
echo.
echo You can now open your UE5 project and the plugin will be available.
echo Any changes made in this worktree will be immediately reflected in your UE5 project.
echo.
pause
