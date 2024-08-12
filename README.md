# lite
![screenshot](https://user-images.githubusercontent.com/3920290/81471642-6c165880-91ea-11ea-8cd1-fae7ae8f0bc4.png)

A lightweight text editor written in Lua

* **[Get lite](https://github.com/rxi/lite/releases/latest)** — Download
  for Windows and Linux
* **[Get started](doc/usage.md)** — A quick overview on how to get started
* **[Get plugins](https://github.com/rxi/lite-plugins)** — Add additional
  functionality
* **[Get color themes](https://github.com/rxi/lite-colors)** — Add additional colors
  themes

## Overview
lite is a lightweight text editor written mostly in Lua — it aims to provide
something practical, pretty, *small* and fast, implemented as simply as
possible; easy to modify and extend, or to use without doing either.

## Note
This fork is attempt to speed up the application, and adding features for personal use.
And importantly, this fork use LuaJIT, some plugins must be port (table.unpack -> unpack), and FFI is not disabled, use at your risk.
The *simple* will be keep in the interfaces, *implementation* will be seperated into two categories: *simple* and *optimized*.
This editor don't and won't support C++ editting. For C++, I will use full IDE for the jobs.

Production ready Lite forks (recommended):
- Lite XL Editor: https://lite-xl.com/
- Pragtical Editor: https://pragtical.dev/

## Changes
See [changelog.md](/changelog.md)

## TODOs
ToDo list, order by priority.

- Native Runtime (focusing):
    - Make app more robust.
    - Fast or flexible, friendly experience on IO operations. (Maybe add async IO)
    - Display launching message box with style and helper.
    - Add more render backends: OpenGL, Vulkan, Dear ImGui.
    - Better font rendering.
    - LiteFx: Framework to make desktop application with C (or other system languages) and Lua/Luau
        - Simple and robust C framework
        - Can be customize Lua runtime: Lua52 (or 53, 54), LuaJIT, Luau
        - Fennel simple showcase for Emacs-like scripting experience
        - Can run LiteXL Lua data (Use Lua54, but there are no compat layer for LuaJIT)
        - Can run Pragtical Lua data

- Features (add when needed):
    - Line wrapping
    - Multi cursors
    - Ignore folders, files (blacklist)
    - Fix bug: cannot open doc when focus TreeView (maybe other lock view)
    - Add polyfill `table.unpack`. (Lua52 compat, may help work with other plugins)
    - Make typing work with UniKey (Vietnamese typing method).
    - Open binary file in preview-mode.
    - Use fast string algorithms (code editing are working on string heavily)
    - Key bindings docs (for Mai usage and MaiStyle). See more https://github.com/maihd/maienv/tree/main/keybinds
    - Recent files in `Open File From Profect` command
    - Mouse next/previous button (good for reading code)
    - VCS status display

- Fix bugs:
    - Extra space for end of file is too large
    - TreeView have no scroll rect
    - Toggle comment not work when:
        - Prefix have no space between content
        - Prefix does not start at begin of the line
    - Cannot open file with command when focus TreeView (or locked view)
    - Modal tabbing (Vi-like) does not trim

- Syntax highlights:
    - Simple syntax highlights are enough.
    - When have some syntax highlight is wrong, and cannot ignore, just fix it.
    - When plugins can solve, use it.
    - No need something complex like lsp or intellisense. You should remember the API.
    - Unknown token should be have underline to present error. Token can be detected without the need of LSP.

- Software distributing:
    - Add item to OS context menu.

- Unnessary software distributing (we are hacker, we install software from source code):
    - Compile to Lua to bytecode.
    - Package data. exe-only application. (data embed into exe, faster startup)
    - Make it embedding ready.
    - Installer

## Customization
Additional functionality can be added through plugins which are available from
the [plugins repository](https://github.com/rxi/lite-plugins); additional color
themes can be found in the [colors repository](https://github.com/rxi/lite-colors).
The editor can be customized by making changes to the
[user module](data/user/init.lua).

## Building
Note that the project does not need to be rebuilt if you are only making changes
to the Lua portion of the code.

On Linux:
You can build the project yourself on Linux using the `build.sh` script

On Windows:
- using the `build_clang.bat` script ([clang]([https://nuwen.net/mingw.html](https://clang.llvm.org/)), native-like build).
- using the `build_mingw.bat` script (MinGW/Msys2 for Windows, does not depend on VCredist)

## Building with premake5
- Running `premake5 <toolchain>` (gmake, XCode, Visual Studio, `premake5 --help` for more details)
- Open/build with your selection toolchain
- Open build folder if in debug mode, root location if in release mode

## Contributing
Any additional functionality that can be added through a plugin should be done
so as a plugin, after which a pull request to the
[plugins repository](https://github.com/rxi/lite-plugins) can be made. In hopes
of remaining lightweight, pull requests adding additional functionality to the
core will likely not be merged. Bug reports and bug fixes are welcome.

## License
This project is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.
