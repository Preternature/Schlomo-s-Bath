@echo off
echo ================================================
echo Schlomo's Bath VST - ONE-CLICK SETUP
echo ================================================
echo.
echo This will:
echo   1. Download JUCE (if needed)
echo   2. Build the plugin
echo   3. Install it to your VST3 folder
echo.
echo Press any key to continue, or Ctrl+C to cancel...
pause > nul

call build.bat
if errorlevel 1 (
    echo.
    echo Build failed. Please check the errors above.
    pause
    exit /b 1
)

echo.
echo.
echo Now installing...
echo.
call install.bat

echo.
echo ================================================
echo ALL DONE! Plugin is ready to use!
echo ================================================
pause
