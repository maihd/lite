# Checking if lite.exe is running
# tasklist /fi "ImageName eq lite.exe" /fo csv 2>NUL | find /i "lite.exe">NUL
# if %ErrorLevel%==0; then
#     echo - Lite is running, please close it before build, and run this script again
#     # goto :done
# fi

echo "- Compiling (windows - clang - x64)..."

WINDOW_SYSTEM=$1

# Define here to prevent first build failed
SDL_VERSION=SDL2-2.30.6

# if [ "$WINDOW_SYSTEM"=="native" ]; then
#     echo "- Selected Native API for window system"
#     # Libs for native (uncomment to use)
#     PLATFORM_LIBS=
# else
    
# fi

echo "- Selected SDL2 for window system"

if [ ! -d ".build" ]; then
    mkdir .build
fi

if [ "$(uname)" == "Darwin" ]; then
    NATIVE_LIBS="\
        -framework Cocoa                    \
        -framework CoreAudio                \
        -framework CoreMedia                \
        -framework CoreVideo                \
        -framework AVFoundation             \
        -framework Metal                    \
        -framework GameController           \
        -framework CoreHaptics              \
        -framework AppKit                   \
        -framework CoreFoundation           \
        -framework IOKit                    \
        -framework ForceFeedback            \
        -framework Carbon                   \
        -framework Foundation               \
        -framework QuartzCore               \
        -framework AudioToolbox             \
        -framework UniformTypeIdentifiers   \
    "
else
    NATIVE_LIBS=""
fi

PLATFORM_LIBS="-Ilibs/$SDL_VERSION/include -DLITE_SYSTEM_SDL2 -lSDL2 -Llibs/$SDL_VERSION/lib/mac_arm64"

clang src/*.c src/api/*.c src/lib/stb/*.c \
    -O3 -ffast-math -std=c11 -fno-strict-aliasing \
    -Isrc -DNDEBUG \
    -DLUA_USE_POPEN -D_CRT_SECURE_NO_WARNINGS \
    $NATIVE_LIBS \
    $PLATFORM_LIBS \
    -Ilibs/litelua_luajit_2.1.0-rolling_04302025/include \
    -llitelua_luajit -Llibs/litelua_luajit_2.1.0-rolling_04302025/prebuilt/mac_arm64 \
    -o .build/lite

# -Ilibs/luajit_2.1.0-beta3/src ^
# -llua51_static -Llibs/luajit_2.1.0-beta3/prebuilt/x64 ^

# if [ ! %ErrorLevel%==0 ]; then
#     echo Build failed, maybe lite still running, please close and recompile with build_clang.bat
#     # goto :done
# fi

# Copy lite
echo - Copying dist files...
rm -f lite
cp .build/lite lite

# Remove redundant files
# del lite.exp
# del lite.lib
rm -rf .build

# :done
# echo - Build done!
