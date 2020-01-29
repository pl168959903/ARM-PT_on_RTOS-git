@echo off
set UV=%1
set UV_PRO_PATH=%2
set LOG_PATH=%3
md %LOG_PATH%\log
CLS
echo Start Building ...
echo .>%LOG_PATH%\log\VSCode_build.log
%UV% -j0 -b %UV_PRO_PATH% -o %LOG_PATH%\log\VSCode_build.log
type %LOG_PATH%\log\VSCode_build.log
echo Done.