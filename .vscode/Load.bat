@echo off
set UV=%1
set UV_PRO_PATH=%2
set LOG_PATH=%3
md %LOG_PATH%\log
CLS
echo Start Loading ...
echo .>%LOG_PATH%\log\VSCode_load.log
%UV% -j0 -f %UV_PRO_PATH% -o %LOG_PATH%\log\VSCode_load.log
type %LOG_PATH%\log\VSCode_load.log
echo Done.