@echo off
set uvprojxName=%1
set uvprojxPath=%2
set config=%3

echo Working...
UV4 -j0 %config% %uvprojxPath%\%uvprojxName%.uvprojx -o %uvprojxPath%\log.txt
type %uvprojxPath%\log.txt
echo Done.
