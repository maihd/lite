@echo off

rem download this:
rem https://nuwen.net/mingw.html

echo compiling (windows - mingw - x64)...

gcc src/*.c src/api/*.c src/lib/stb/*.c^
    -O3 -s -std=gnu11 -fno-strict-aliasing -Isrc -DLUA_USE_POPEN^
    -Ilibs/SDL2-2.0.10/x86_64-w64-mingw32/include^
    -lmingw64 -lm -lSDL2main -lSDL2 -Llibs/SDL2-2.0.10/x86_64-w64-mingw32/lib^
    -Ilibs/luajit_2.1.0-beta3/src^
    -llua51 -Llibs/luajit_2.1.0-beta3/prebuilt/x64^
    -mwindows res/res.res^
    -o lite.exe

echo done
