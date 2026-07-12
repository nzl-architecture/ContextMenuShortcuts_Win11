if %*= ""(
echo "Welcome to use Appx Package Installer Community!"
set /p "appxfile=Enter your appx package to install (allowed .appx .msix .appxbundle .msixbundle) :")
else (
set "appxfile=%*"
)
AddAppxPackage.bat %appxfile%
