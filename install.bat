@echo off
echo ================================================
echo Schlomo's Bath VST - Installer
echo ================================================
echo.

set VST3_PATH=C:\Program Files\Common Files\VST3
set PLUGIN_FILE=build\SchlomosBath_artefacts\Release\VST3\Schlomos Bath.vst3\Contents\x86_64-win\Schlomos Bath.vst3

if not exist "%PLUGIN_FILE%" (
    echo ERROR: Plugin not found at:
    echo %PLUGIN_FILE%
    echo.
    echo Please build the plugin first by running: build.bat
    pause
    exit /b 1
)

echo Installing to: %VST3_PATH%
echo.
echo This requires administrator privileges...
echo.

REM Remove old version if exists
if exist "%VST3_PATH%\SchlomosBath.vst3" (
    echo Removing old version...
    rmdir /s /q "%VST3_PATH%\SchlomosBath.vst3"
)

REM Copy new version
echo Copying plugin...
xcopy /E /I /Y "%PLUGIN_FILE%" "%VST3_PATH%\SchlomosBath.vst3"

if errorlevel 1 (
    echo.
    echo ERROR: Installation failed!
    echo Make sure you're running this as Administrator.
    echo.
    echo Right-click install.bat and select "Run as administrator"
    pause
    exit /b 1
)

echo.
echo ================================================
echo Installation complete!
echo ================================================
echo.
echo Schlomo's Bath is now installed and ready to use!
echo Restart your DAW to see the plugin.
echo.
pause
