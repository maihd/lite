if [ ! -d ".vendored_libs/SDL3" ]; then
    git clone https://github.com/libsdl-org/sdl --recursive --depth=1 .vendored_libs/SDL3
fi