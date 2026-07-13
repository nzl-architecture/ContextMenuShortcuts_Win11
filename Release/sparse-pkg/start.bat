@echo off
setlocal enabledelayedexpansion
set block=0
echo "%*" | findstr /C:".msix" >nul
set msixMatch=!errorlevel!
echo "%*" | findstr /C:".appx" >nul
set appxMatch=!errorlevel!
if !appxMatch!==0 (
    set block=1
)
if !msixMatch!==0 (
    set block=1
)
if !block!==1 (
echo "Welcome to use Appx Package Installer Community!"
set /p "appxfile=Enter your appx package to install (allowed .appx .msix .appxbundle .msixbundle) :"
)else (
set "appxfile=%*"
)
AddAppxPackage.bat %appxfile%
