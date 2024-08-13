@echo off

call build_clang.bat

echo:
call active_edit_with_lite.bat

echo:
echo - Add lite directory to environment path...
setx Path "%Path%;%~dp0"
