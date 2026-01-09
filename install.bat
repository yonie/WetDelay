@echo off
echo ================================================
echo  WetDelay VST3 Plugin - Install Script
echo ================================================
echo.

set VST3_PATH=%COMMONPROGRAMFILES%\VST3
set PLUGIN_PATH=WetDelay\build\VST3\Release\WetDelay.vst3

if not exist "%PLUGIN_PATH%" (
    echo ERROR: Plugin not found at %PLUGIN_PATH%
    echo Please run build.bat first!
    echo.
    pause
    exit /b 1
)

echo Installing plugin to: %VST3_PATH%
echo.

REM Create VST3 directory if it doesn't exist
if not exist "%VST3_PATH%" (
    echo Creating VST3 directory...
    mkdir "%VST3_PATH%"
)

REM Clean existing installation first
if exist "%VST3_PATH%\WetDelay.vst3" (
    echo Removing old installation...
    rmdir /S /Q "%VST3_PATH%\WetDelay.vst3"
)

REM Copy the plugin
echo Copying plugin files...
xcopy /E /Y /I "%PLUGIN_PATH%" "%VST3_PATH%\WetDelay.vst3\"
if errorlevel 1 (
    echo ERROR: Failed to copy plugin!
    echo You may need to run this script as Administrator.
    echo.
    pause
    exit /b 1
)

echo.
echo ================================================
echo  Installation successful!
echo ================================================
echo.
echo Plugin installed to: %VST3_PATH%\WetDelay.vst3
echo.
echo You can now use the plugin in your DAW.
echo Restart your DAW if it's currently running.
echo.
pause