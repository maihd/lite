@echo off

rem download this:
rem https://nuwen.net/mingw.html

echo compiling (windows)...

windres res.rc -O coff -o res.res
gcc src/*.c src/api/*.c src/lib/stb/*.c^
    -O3 -s -std=gnu11 -fno-strict-aliasing -Isrc -DLUA_USE_POPEN^
    -Ilibs/SDL2-2.0.10/x86_64-w64-mingw32/include^
    -lmingw32 -lm -lSDL2main -lSDL2 -Llibs/SDL2-2.0.10/x86_64-w64-mingw32/lib^
    -Ilibs/luajit_2.1.0/src^
    -llua51 -Llibs/luajit_2.1.0/src^
    -mwindows res.res^
    -o lite.exe

echo done
