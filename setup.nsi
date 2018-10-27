# define installer name
OutFile "setup.exe"
 
# set desktop as install directory
InstallDir $DESKTOP\JeredsProject
 
# default section start
Section
 
# define output path
SetOutPath $INSTDIR
 
# specify file to go in output path
File ..\ProtienLauncher\ProtienLauncher\bin\Debug\ProtienLauncher.exe
File x64\Release\simulation.exe
File x64\Release\sfml-window-2.dll
File x64\Release\sfml-system-2.dll
File x64\Release\sfml-graphics-2.dll
File x64\Release\openal32.dll
File x64\Release\libcurl-x64.dll
File x64\Release\libcurl.dll
File x64\Release\msvcp140d.dll
File x64\Release\libssl-1_1-x64.dll
File x64\Release\libcrypto-1_1-x64.dll

# define uninstaller name
WriteUninstaller $INSTDIR\uninstaller.exe
  
#-------
# default section end
SectionEnd
 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
Section "Uninstall"
 
# now delete installed file
RMDir /r $INSTDIR\
 
SectionEnd