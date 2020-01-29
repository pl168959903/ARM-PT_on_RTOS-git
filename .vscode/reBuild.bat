@echo off
set UV=%1
set UV_PRO_PATH=%2
set LOG_PATH=%3
md %LOG_PATH%\log
CLS
echo Start reBuilding ...
echo .>%LOG_PATH%\log\VSCode_rebuild.log
%UV% -j0 -cr %UV_PRO_PATH% -o %LOG_PATH%\log\VSCode_rebuild.log
type %LOG_PATH%\log\VSCode_rebuild.log
echo Done.