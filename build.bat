@echo off
echo ================================================
echo Schlomo's Bath VST - Build Script
echo Vocal Life Engine
echo ================================================
echo.

REM Check if JUCE exists
if not exist "JUCE" (
    echo JUCE not found. Cloning JUCE from GitHub...
    echo.
    git clone https://github.com/juce-framework/JUCE.git
    if errorlevel 1 (
        echo.
        echo ERROR: Failed to clone JUCE. Please install git or download JUCE manually.
        echo Download from: https://juce.com/get-juce/
        echo Extract to: %cd%\JUCE
        pause
        exit /b 1
    )
)

echo JUCE found!
echo.

REM Create build directory
if not exist "build" mkdir build
cd build

echo Configuring CMake...
echo.
cmake .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed.
    echo Make sure you have:
    echo   1. Visual Studio 2022 installed
    echo   2. CMake installed (download from cmake.org)
    echo.
    pause
    cd ..
    exit /b 1
)

echo.
echo Building plugin (Release mode)...
echo This may take a few minutes...
echo.
cmake --build . --config Release
if errorlevel 1 (
    echo.
    echo ERROR: Build failed. Check the errors above.
    pause
    cd ..
    exit /b 1
)

echo.
echo ================================================
echo Build successful!
echo ================================================
echo.
echo Your VST3 plugin is at:
echo %cd%\SchlomosBath_artefacts\Release\VST3\Schlomos Bath.vst3
echo.
echo To install, copy the .vst3 file to:
echo C:\Program Files\Common Files\VST3\
echo.
echo Or run: install.bat
echo.
pause
cd ..
