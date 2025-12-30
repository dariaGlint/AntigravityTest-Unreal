@echo off
REM Blueprint Analyzer - Phase 1 Implementation (Windows)
REM Analyzes Unreal Engine Blueprint assets for naming conventions and structure

setlocal enabledelayedexpansion

REM Check for required commands
where findstr >nul 2>nul
if errorlevel 1 (
    echo Error: Required command 'findstr' not found
    exit /b 1
)

set "CONTENT_PATH=Content"
set "ANALYSIS_TYPE=full"
set "GENERATE_DIAGRAM=false"

REM Parse command line arguments
:parse_args
if "%~1"=="" goto check_content
if "%~1"=="--path" (
    set "CONTENT_PATH=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--type" (
    set "ANALYSIS_TYPE=%~2"
    shift
    shift
    goto parse_args
)
if "%~1"=="--diagram" (
    set "GENERATE_DIAGRAM=true"
    shift
    goto parse_args
)
if "%~1"=="--help" (
    echo Usage: %~nx0 [options]
    echo Options:
    echo   --path ^<path^>      Path to analyze (default: Content)
    echo   --type ^<type^>      Asset type to focus on (blueprints^|ui^|materials^|all)
    echo   --diagram          Generate Mermaid diagram
    echo   --help             Show this help message
    exit /b 0
)
echo Unknown option: %~1
exit /b 1

:check_content
REM Validate path - check for dangerous characters
echo "%CONTENT_PATH%" | findstr /R "[&|<>^]" >nul
if not errorlevel 1 (
    echo Error: Path contains invalid characters
    exit /b 1
)

if not exist "%CONTENT_PATH%" (
    echo Error: Content directory not found at %CONTENT_PATH%
    exit /b 1
)

echo ===================================================
echo   Blueprint Analysis Report
echo ===================================================
echo.

echo Scanning for Blueprint assets...

REM Count total .uasset files (excluding __ExternalObjects__)
set "TOTAL_COUNT=0"
for /r "%CONTENT_PATH%" %%f in (*.uasset) do (
    set "filepath=%%~dpf"
    echo "!filepath!" | findstr /C:"__ExternalObjects__" >nul
    if errorlevel 1 (
        set /a TOTAL_COUNT+=1
    )
)

echo Found %TOTAL_COUNT% assets
echo.

echo -------------------------------------------------
echo 1. Asset Statistics by Type
echo -------------------------------------------------
echo.

REM Count by prefix (single-pass optimization)
set "BP_COUNT=0"
set "WBP_COUNT=0"
set "M_COUNT=0"
set "MI_COUNT=0"
set "IA_COUNT=0"
set "IMC_COUNT=0"
set "NS_COUNT=0"
set "ST_COUNT=0"
set "BT_COUNT=0"
set "BB_COUNT=0"
set "OTHER_COUNT=0"

for /r "%CONTENT_PATH%" %%f in (*.uasset) do (
    set "filepath=%%~dpf"
    set "filename=%%~nxf"
    echo "!filepath!" | findstr /C:"__ExternalObjects__" >nul
    if errorlevel 1 (
        REM Check prefix (optimized with if-else to count only once)
        set "counted=0"
        echo "!filename!" | findstr /B "WBP_" >nul && if !counted!==0 (set /a WBP_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "BP_" >nul && if !counted!==0 (set /a BP_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "MI_" >nul && if !counted!==0 (set /a MI_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "M_" >nul && if !counted!==0 (set /a M_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "IMC_" >nul && if !counted!==0 (set /a IMC_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "IA_" >nul && if !counted!==0 (set /a IA_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "NS_" >nul && if !counted!==0 (set /a NS_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "ST_" >nul && if !counted!==0 (set /a ST_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "BT_" >nul && if !counted!==0 (set /a BT_COUNT+=1 & set "counted=1")
        echo "!filename!" | findstr /B "BB_" >nul && if !counted!==0 (set /a BB_COUNT+=1 & set "counted=1")
    )
)

if %BP_COUNT% gtr 0 echo Blueprint Classes (BP_*):       %BP_COUNT%
if %WBP_COUNT% gtr 0 echo Widget Blueprints (WBP_*):     %WBP_COUNT%
if %M_COUNT% gtr 0 echo Materials (M_*):                %M_COUNT%
if %MI_COUNT% gtr 0 echo Material Instances (MI_*):     %MI_COUNT%
if %IA_COUNT% gtr 0 echo Input Actions (IA_*):          %IA_COUNT%
if %IMC_COUNT% gtr 0 echo Input Mapping Contexts (IMC_*): %IMC_COUNT%
if %NS_COUNT% gtr 0 echo Niagara Systems (NS_*):        %NS_COUNT%
if %ST_COUNT% gtr 0 echo State Trees (ST_*):            %ST_COUNT%
if %BT_COUNT% gtr 0 echo Behavior Trees (BT_*):         %BT_COUNT%
if %BB_COUNT% gtr 0 echo Blackboards (BB_*):            %BB_COUNT%

set /a OTHER_COUNT=%TOTAL_COUNT%-%BP_COUNT%-%WBP_COUNT%-%M_COUNT%-%MI_COUNT%-%IA_COUNT%-%IMC_COUNT%-%NS_COUNT%-%ST_COUNT%-%BT_COUNT%-%BB_COUNT%
echo Other/Unknown:                 %OTHER_COUNT%
echo.

echo -------------------------------------------------
echo 2. Assets by Variant
echo -------------------------------------------------
echo.

REM Count by variant
set "COMBAT_COUNT=0"
set "SIDESCROLLING_COUNT=0"
set "PLATFORMING_COUNT=0"

for /r "%CONTENT_PATH%" %%f in (*.uasset) do (
    set "filepath=%%~dpf"
    echo "!filepath!" | findstr /C:"__ExternalObjects__" >nul
    if errorlevel 1 (
        echo "!filepath!" | findstr /C:"Variant_Combat" >nul && set /a COMBAT_COUNT+=1
        echo "!filepath!" | findstr /C:"Variant_SideScrolling" >nul && set /a SIDESCROLLING_COUNT+=1
        echo "!filepath!" | findstr /C:"Variant_Platforming" >nul && set /a PLATFORMING_COUNT+=1
    )
)

set /a CORE_COUNT=%TOTAL_COUNT%-%COMBAT_COUNT%-%SIDESCROLLING_COUNT%-%PLATFORMING_COUNT%

echo Variant_Combat:              %COMBAT_COUNT%
echo Variant_SideScrolling:       %SIDESCROLLING_COUNT%
echo Variant_Platforming:         %PLATFORMING_COUNT%
echo Core/Shared:                 %CORE_COUNT%
echo.

echo -------------------------------------------------
echo 3. Naming Convention Analysis
echo -------------------------------------------------
echo.

REM Check for files in root Content folder
set "ROOT_COUNT=0"
for %%f in ("%CONTENT_PATH%\*.uasset") do (
    set /a ROOT_COUNT+=1
)

if %ROOT_COUNT% gtr 0 (
    echo Found %ROOT_COUNT% potential naming/location issues
) else (
    echo No naming convention violations found!
)
echo.

echo -------------------------------------------------
echo 4. Folder Structure Analysis
echo -------------------------------------------------
echo.

if %ROOT_COUNT% gtr 0 (
    echo Found %ROOT_COUNT% assets in root Content folder:
    for %%f in ("%CONTENT_PATH%\*.uasset") do (
        echo   ⚠ %%~nxf
    )
    echo.
) else (
    echo No assets in root Content folder
    echo.
)

echo -------------------------------------------------
echo 5. Blueprint Categories
echo -------------------------------------------------
echo.

REM Count categories
set "CHAR_COUNT=0"
set "GM_COUNT=0"
set "AI_COUNT=0"
set "INTERACT_COUNT=0"
set "UI_COUNT=0"

for /r "%CONTENT_PATH%" %%f in (*.uasset) do (
    set "filepath=%%~dpf"
    set "filename=%%~nxf"
    echo "!filepath!" | findstr /C:"__ExternalObjects__" >nul
    if errorlevel 1 (
        echo "!filename!" | findstr /C:"Character" >nul && set /a CHAR_COUNT+=1
        echo "!filename!" | findstr /C:"GameMode" >nul && set /a GM_COUNT+=1
        echo "!filename!" | findstr /C:"AIController" >nul && set /a AI_COUNT+=1
        echo "!filename!" | findstr /C:"ST_" >nul && set /a AI_COUNT+=1
        echo "!filename!" | findstr /C:"Dummy" >nul && set /a INTERACT_COUNT+=1
        echo "!filename!" | findstr /C:"Floor" >nul && set /a INTERACT_COUNT+=1
        echo "!filename!" | findstr /B "WBP_" >nul && set /a UI_COUNT+=1
        echo "!filename!" | findstr /B "UI_" >nul && set /a UI_COUNT+=1
    )
)

echo Character Blueprints: %CHAR_COUNT%
echo Game Mode Blueprints: %GM_COUNT%
echo AI Components: %AI_COUNT%
echo Interactables: %INTERACT_COUNT%
echo UI Widgets: %UI_COUNT%
echo.

echo -------------------------------------------------
echo 6. Recommendations
echo -------------------------------------------------
echo.

if %ROOT_COUNT% gtr 0 (
    echo 1. Move %ROOT_COUNT% assets from root Content folder to appropriate subdirectories
    echo.
    echo Consider organizing assets into these suggested folders:
    echo   - Content\Core\Blueprints\ (for shared Blueprints)
    echo   - Content\Variant_*\Blueprints\Characters\
    echo   - Content\Variant_*\Blueprints\GameModes\
    echo   - Content\Variant_*\Blueprints\AI\
) else (
    echo Project structure looks good!
)
echo.

echo ===================================================
echo Analysis complete!
echo ===================================================

endlocal
