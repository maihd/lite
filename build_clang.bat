@echo off

:: Checking if lite.exe is running
tasklist /fi "ImageName eq lite.exe" /fo csv 2>NUL | find /i "lite.exe">NUL
if %ErrorLevel%==0 (
    echo - Lite is running, please close it before build, and run this script again
    goto :done
)

echo - Compiling (windows - clang - x64)...

set WINDOW_SYSTEM=%1

:: Define here to prevent first build failed
set SDL_VERSION=SDL2-2.30.6

if "%WINDOW_SYSTEM%"=="win32" (
    echo - Selected Native Win32 API for window system
    :: Libs for Win32 (uncomment to use)
    set PLATFORM_LIBS=
) else (
    echo - Selected SDL2 for window system
    :: Libs for SDL (uncomment to use)
    set PLATFORM_LIBS=-Ilibs/%SDL_VERSION%/include -DLITE_SYSTEM_SDL2 -lSDL2-static -Llibs/%SDL_VERSION%/lib/x64
)

set NATIVE_LIBS=^
    -lKernel32 -lUser32 -lGdi32 -lShell32 -lWinmm -lOle32 -lVersion ^
    -lCfgMgr32 -lImm32 -lSetupapi -lAdvapi32 -lOleAut32

if not exist .build (
    mkdir .build
)

clang src/*.c src/api/*.c src/lib/stb/*.c ^
    -O3 -ffast-math -std=c11 -fno-strict-aliasing ^
    -Isrc -DNDEBUG ^
    -D_CRT_SECURE_NO_WARNINGS ^
    %NATIVE_LIBS% ^
    %PLATFORM_LIBS% ^
    -Ilibs/litelua_luajit_2.1.0-rolling_04302025/include ^
    -llitelua_luajit.lib -Llibs/litelua_luajit_2.1.0-rolling_04302025/prebuilt/x64 ^
    res/res.res^
    -o .build/lite.exe

:: -Ilibs/luajit_2.1.0-beta3/src ^
:: -llua51_static -Llibs/luajit_2.1.0-beta3/prebuilt/x64 ^

:: if %ErrorLevel% neq 0 (
if not exist .build\lite.exe (
    echo Build failed, maybe lite still running, please close and recompile with build_clang.bat
    echo Error code: %ErrorLevel%
    goto :done
)

:: Copy lite
echo - Copying dist files...
del lite.exe
copy .build\lite.exe lite.exe

:: Remove redundant files
:: del lite.exp
:: del lite.lib
rmdir .build /s /q

:done
echo - Build done!
