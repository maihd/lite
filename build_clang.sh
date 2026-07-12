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

# 1. Detect os and set arch
if [ "$(uname)" == "Darwin" ]; then
    echo "- System detected: macOS"
    ARCH_DIR="mac_arm64" # Change to mac_x64 if on Intel Mac

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

elif [ "$(uname)" == "Linux" ]; then
    echo "- System detected: Linux"
    
    # Dynamically handle Linux CPU architectures (x86_64 vs Aarch64/ARM)
    CPU_TYPE="$(uname -m)"
    if [ "$CPU_TYPE" == "x86_64" ]; then
        ARCH_DIR="linux64"
    else
        ARCH_DIR="linux_arm64"
    fi
    
    # Linux system libraries required for standard windowing/linking
    # -lm handles math functions (-ffast-math), -lrt for real-time extensions, -ldl for dynamic linking
    NATIVE_LIBS="-lm -lrt -ldl" 
else
    echo "- System detected: Unknown ($OS_TYPE). Defaulting paths to x64..."
    NATIVE_LIBS=""
fi

# 2. Configure Window System Libraries
if [ "$WINDOW_SYSTEM" == "native" ]; then
    echo "- Selected Native API for window system"
    PLATFORM_LIBS=""
else
    echo "- Selected SDL2 for window system"
    # Added dynamic $ARCH_DIR variable here
    PLATFORM_LIBS="-Ilibs/$SDL_VERSION/include -DLITE_SYSTEM_SDL2 -lSDL2 -Llibs/$SDL_VERSION/lib/$ARCH_DIR"
fi

echo "ARCH_DIR = $ARCH_DIR"

clang src/*.c src/api/*.c src/lib/stb/*.c \
    -O3 -ffast-math -std=c11 -fno-strict-aliasing \
    -D_DEFAULT_SOURCE \
    -Isrc -DNDEBUG \
    -DLUA_USE_POPEN -D_CRT_SECURE_NO_WARNINGS \
    $NATIVE_LIBS \
    $PLATFORM_LIBS \
    -Ilibs/litelua_luajit_2.1.0-rolling_04302025/include \
    -llitelua_luajit -Llibs/litelua_luajit_2.1.0-rolling_04302025/prebuilt/$ARCH_DIR \
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
