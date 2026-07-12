net session >nul 2>&1 || (powershell -NoP -C "Start-Process -FilePath 'cmd.exe' -ArgumentList '/c', '\"\"%~f0\" %*\"' -Verb RunAs" && exit)
echo "Register appx package from %*?"
echo "Mode 1: Development Mode"
echo "Mode 2: Disable Development Mode"
echo "Mode 3: Custom Params"
set /p "choice=Select a mode:"
set params=
if "%choice%"=="1" (
powershell.exe -Command "Add-AppxPackage -Path '%*' -Register"
)
if "%choice%"=="2" (
powershell.exe -Command "Add-AppxPackage -Path '%*'  -Register -DisableDevelopmentMode"

)
if "%choice%"=="3" (
set /p "params=Enter params for add appx package %*:"
powershell.exe -Command "Add-AppxPackage -Path '%*' -Register %params%"
)
pause