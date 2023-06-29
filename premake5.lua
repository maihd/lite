local ROOT_DIR = path.getabsolute(".")
local LIBS_DIR = path.join(ROOT_DIR, "libs")
local BUILD_DIR = path.join(ROOT_DIR, "build")
local PROJECT_DIR = path.join(ROOT_DIR, "project")

workspace "lite"
do
    language "C"
    location (PROJECT_DIR)

    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }

    filter {}
end

project "lite"
do
    kind "WindowedApp"
    -- filter { "configurations:Debug"}
    -- do
    --     kind "ConsoleApp"
    -- end

    -- filter { "configurations:Release"}
    -- do
    --     kind "WindowedApp"
    -- end

    files {
        path.join(ROOT_DIR, "src/**.h"),
        path.join(ROOT_DIR, "src/**.c"),
    }

    links {
        "lua51_static",
        "SDL2",
        "SDL2main",
    }

    defines {
        -- "LUA_BUILD_AS_DLL",
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs {
        path.join(ROOT_DIR, "src/"),
        path.join(ROOT_DIR, "src/api"),
        path.join(LIBS_DIR, "luajit_2.1.0-beta3/include"),
        path.join(LIBS_DIR, "SDL2-devel-2.0.16-VC/include"),
    }

    filter { "configurations:Debug" }
    do
        debugargs { "../data" }

        targetdir (BUILD_DIR)

        postbuildcommands {
            "xcopy \"" .. path.join(ROOT_DIR, "data") .. "\" \"$(OutDir)\\data\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter { "configurations:Release" }
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
            path.join(LIBS_DIR, "SDL2-devel-2.0.16-VC/lib/x86"),
        }

        postbuildcommands {
            "xcopy \"" .. path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x86/lua51.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
            "xcopy \"" .. path.join(LIBS_DIR, "SDL2-devel-2.0.16-VC/lib/x86/SDL2.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter { "platforms:x64" }
    do
        libdirs {
            path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x64"),
            path.join(LIBS_DIR, "SDL2-devel-2.0.16-VC/lib/x64"),
        }

        postbuildcommands {
            "xcopy \"" .. path.join(LIBS_DIR, "luajit_2.1.0-beta3/prebuilt/x64/lua51.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
            "xcopy \"" .. path.join(LIBS_DIR, "SDL2-devel-2.0.16-VC/lib/x64/SDL2.dll") .. "\" \"$(OutDir)\" /D /E /I /F /Y",
        }

        filter {}
    end

    filter {}
end
