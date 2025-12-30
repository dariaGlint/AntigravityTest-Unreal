@echo off
REM UE Asset Analyzer - Windows Batch Version
REM For full functionality, use Git Bash or WSL with the .sh version

setlocal enabledelayedexpansion

echo ================================================================
echo   UE Asset Analyzer (Windows)
echo ================================================================
echo.
echo This is a simplified Windows version.
echo For full functionality, please use Git Bash or WSL:
echo   bash scripts/ue-asset-analyzer.sh
echo.
echo ================================================================
echo.

REM Check if Git Bash is available
where bash >nul 2>&1
if %errorlevel% equ 0 (
    echo Git Bash detected. Running full version...
    echo.
    bash "%~dp0ue-asset-analyzer.sh" %*
    exit /b %errorlevel%
)

REM Fallback: Basic Windows implementation
echo Git Bash not found. Running basic analysis...
echo.

set CONTENT_PATH=Content
if not "%1"=="" (
    if "%1"=="--path" (
        set CONTENT_PATH=%2
    )
)

if not exist "%CONTENT_PATH%" (
    echo Error: Content directory not found at %CONTENT_PATH%
    exit /b 1
)

echo Scanning assets in %CONTENT_PATH%...
echo.

REM Count .uasset files
set /a COUNT=0
for /r "%CONTENT_PATH%" %%f in (*.uasset) do (
    set /a COUNT+=1
)

echo Found %COUNT% assets
echo.

echo For detailed analysis, please use Git Bash:
echo   bash scripts/ue-asset-analyzer.sh --help
echo.

exit /b 0
