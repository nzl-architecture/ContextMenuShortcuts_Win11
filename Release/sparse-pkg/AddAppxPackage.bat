net session >nul 2>&1 || (powershell -NoP -C "Start-Process -FilePath 'cmd.exe' -ArgumentList '/c', '\"\"%~f0\" %*\"' -Verb RunAs" && exit)
echo "Add appx package %*?"
echo "Mode 1: DISM + Add-ProvisionedAppxPackage (system-level)"
echo "Mode 2: Powershell + Add-AppxPackage (per-user)"
echo "Mode 3: DISM +  + Custom Params (system-level)"
echo "Mode 4: Powershell + Add-AppxPackage + Custom Params (per-user)"
set /p "choice=Select a mode:"
set params=
if "%choice%"=="1" (
dism.exe -Online -Add-ProvisionedAppxPackage -PackagePath:"%*" -SkipLicense
)
if "%choice%"=="2" (
powershell.exe -Command "Add-AppxPackage -Path '%*' -ForceApplicationShutdown"

)
if "%choice%"=="3" (
set /p "params=Enter params for add appx package %*:"
dism.exe -Online -Add-ProvisionedAppxPackage -PackagePath:"%*" %params%
)
if "%choice%"=="4" (
set /p "params=Enter params for add appx package %*:"
powershell.exe -Command "Add-AppxPackage -Path "%*" %params%"
)
pause