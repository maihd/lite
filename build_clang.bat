@echo off

echo Compiling (windows - clang - x64)...

:: Libs for SDL (uncomment to use)
:: set PLATFORM_LIBS=-Ilibs/SDL2-2.28.3/include -DLITE_SYSTEM_SDL2 -lSDL2-static -Llibs/SDL2-2.28.3/lib/x64

:: Libs for Win32 (uncomment to use)
set PLATFORM_LIBS=

clang src/*.c src/api/*.c src/lib/stb/*.c ^
    -Ofast -std=c11 -fno-strict-aliasing ^
    -Isrc -DNDEBUG ^
    -DLUA_USE_POPEN -D_CRT_SECURE_NO_WARNINGS ^
    -lKernel32 -lUser32 -lGdi32 -lShell32 -lWinmm -lOle32 -lVersion ^
    -lCfgMgr32 -lImm32 -lSetupapi -lAdvapi32 -lOleAut32 ^
    %PLATFORM_LIBS% ^
    -Ilibs/luajit_2.1.0-beta3/src ^
    -llua51_static -Llibs/luajit_2.1.0-beta3/prebuilt/x64 ^
    -mwindows res/res.res^
    -o lite.exe

echo done
