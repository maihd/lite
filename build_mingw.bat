@echo off

:: download this:
:: https://www.msys2.org/
:: and install it to C:\msys64
:: then run this:
:: C:\msys64\mingw64.exe
:: and install clang:
:: pacman -S mingw-w64-x86_64-clang

echo Compiling (windows - mingw - x64)...

set CC=C:\msys64\mingw64\bin\clang.exe
@REM set CC=gcc.exe

windres res/res.rc -O coff -o res.res
%CC% src/*.c src/api/*.c src/lib/stb/*.c ^
    -O3 -std=c11 -fno-strict-aliasing ^
    -DNDEBUG=1^
    -Isrc -DLUA_USE_POPEN ^
    -Ilibs/SDL2-2.0.10/x86_64-w64-mingw32/include -DLITE_SYSTEM_SDL2^
    -lSDL2main -lSDL2 -Llibs/SDL2-2.0.10/x86_64-w64-mingw32/lib^
    -Ilibs/luajit_2.1.0-beta3/src^
    -lluajit_mingw -Llibs/luajit_2.1.0-beta3/prebuilt/x64^
    -mwindows res.res^
    -o lite.exe

echo done
