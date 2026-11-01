@echo off
setlocal
pushd "%~dp0"
if errorlevel 1 exit /b 1

"vendor\bin\premake\premake5.exe" vs2026
set "HZ_PREMAKE_EXIT_CODE=%ERRORLEVEL%"

popd
pause
exit /b %HZ_PREMAKE_EXIT_CODE%
