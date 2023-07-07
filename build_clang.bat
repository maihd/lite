@echo off

echo compiling (windows - clang - x64)...

clang src/*.c src/api/*.c src/lib/stb/*.c^
    -Ofast -std=c11 -fno-strict-aliasing -Isrc -DNDEBUG^
    -DLUA_USE_POPEN -D_CRT_SECURE_NO_WARNINGS^
    -lKernel32 -lUser32^
    -Ilibs/SDL2-devel-2.0.16-VC/include^
    -DLITE_SYSTEM_SDL2 -lSDL2 -Llibs/SDL2-devel-2.0.16-VC/lib/x64^
    -Ilibs/luajit_2.1.0-beta3/src^
    -llua51_static -Llibs/luajit_2.1.0-beta3/prebuilt/x64^
    -mwindows res/res.res^
    -o lite.exe

echo done
