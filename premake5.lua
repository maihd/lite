local ROOT_DIR = path.getabsolute(".")
local LIBS_DIR = path.join(ROOT_DIR, "libs")
local BUILD_DIR = path.join(ROOT_DIR, ".build")
local PROJECT_DIR = path.join(ROOT_DIR, ".project")

workspace "lite"
do
    language "C"
    location (PROJECT_DIR)

    configurations {
        "Debug", "Release",
        "DebugSDL2", "ReleaseSDL2",
        "DebugSDL2OpenGL", "ReleaseSDL2OpenGL",
    }
    platforms { "x86", "x64" }

    filter {}
end

project "lite"
do
    kind "WindowedApp"
    filter { "configurations:Debug" }
    do
        kind "ConsoleApp"
    end

    filter { "configurations:Release" }
    do
        kind "WindowedApp"
    end

    files {
        path.join(ROOT_DIR, "src/*.h"),
        path.join(ROOT_DIR, "src/*.c"),
        path.join(ROOT_DIR, "src/api/**.h"),
        path.join(ROOT_DIR, "src/api/**.c"),
        path.join(ROOT_DIR, "src/lib/**.h"),
        path.join(ROOT_DIR, "src/lib/**.c"),
    }

    links {
        "lua51_static",
    }

    defines {
        -- "LUA_BUILD_AS_DLL",
    }

    includedirs {
        path.join(ROOT_DIR, "src/"),
        path.join(ROOT_DIR, "src/api"),
        path.join(LIBS_DIR, "luajit_2.1.0-beta3/include"),
    }

    filter { "configurations:Debug*" }
    do
        debugargs { "../data", "../fallback" }

        targetdir (BUILD_DIR)

        postbuildcommands {
            "xcopy \"" .. path.join(ROOT_DIR, "data") .. "\" \"$(OutDir)\\data\" /D /E /I /F /Y",
            "xcopy \"" .. path.join(ROOT_DIR, "fallback") .. "\" \"$(OutDir)\\fallback\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter { "configurations:Release*" }
    do
        debugargs { "../data" }

        targetdir (ROOT_DIR)

        defines {
            "NDEBUG"
        }

        filter {}
    end

    filter { "system:windows" }
    do
        files { "res/res.rc", "res/**.ico" }
        filter {}
    end

    filter { "platforms:x86" }
    do
        libdirs {
            path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x86"),
        }

        postbuildcommands {
            "xcopy \"" .. path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x86/lua51.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter { "platforms:x64" }
    do
        libdirs {
            path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x64"),
        }

        postbuildcommands {
            "xcopy \"" .. path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x64/lua51.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter { "configurations:*SDL2*" }
    do
        links {
            "SDL2-static",
            "Gdi32",
            "Shell32",
            "Winmm",
            "Ole32",
            "Version",
            "CfgMgr32",
            "Imm32",
            "Setupapi",
            "Advapi32",
            "OleAut32"
        }

        defines {
            "LITE_SYSTEM_SDL2"
        }

        includedirs {
            path.join(LIBS_DIR, "SDL2-2.30.6/include"),
        }

        filter { "platforms:x86" }
        do
            libdirs {
                path.join(LIBS_DIR, "SDL2-2.30.6/lib/x86"),
            }
        end

        filter { "platforms:x64" }
        do
            libdirs {
                path.join(LIBS_DIR, "SDL2-2.30.6/lib/x64"),
            }
        end

        filter {}
    end

    filter { "configurations:*OpenGL" }
    do
        links {
            "OpenGL32"
        }

        defines {
            "LITE_RENDERER_OPENGL"
        }

        includedirs {
            path.join("libs/glad")
        }

        files {
            path.join("libs/glad/glad.h"),
            path.join("libs/glad/glad.c"),
        }

        filter {}
    end

    filter {}
end
