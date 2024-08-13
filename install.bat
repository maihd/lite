@echo off

call build_clang.bat

echo:
call active_edit_with_lite.bat

echo:
setx Path "%Path%;%~dp0"
