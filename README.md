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

## Changes
- Use LuaJIT-2.1.0-beta3 instead of Lua 5.2
- Check directory changed based on last write time instead of file diffs
- Add premake5 to generate Visual Studio project, for debugging purpose
- Add clang build script on Windows
- Display MessageBox when app failed to launch.
- Add clang-format.
- Skip opening binary file.
- Fix BOM on Unicode files.
- Soft tab VSCode-like.
- Languages: Batch, `C#`, C.
- Double click to log line to open file.
- Refactor C sources.
- Align line numbers near code.
- Display icon to show log type.
- Draw intent guide line. (may be need more mechanic for syntax highlight)
- Display project name in title bar and status bar

## TODOs
- Make app more robust.
- Convert to use native API instead of large library like SDL.
- Fast or flexible, friendly experience on IO operations. (Maybe add async IO)
- Line wrapping.
- Fix bug: some time single mouse click cause select token, line
- Add polyfill `table.unpack`. (Lua52 compat, may help work with other plugins)
- Make typing work with UniKey (Vietnamese typing method).
- Open binary file in preview-mode.
- Use fast string algorithms (code editting are working on string heavily)
- Bootstrap version for debugging and development.
- Display launching messagebox with style and helper.
- Add more render backends: OpenGL, Vulkan, Dear ImGui.
- Compile to Lua to bytecode.
- Package data. exe-only application. (data embed into exe, faster startup)
- Make it embedding ready.
- Better mouse interacting.
- Software distributing
    - Installer
    - Add item to OS context menu.

## Customization
Additional functionality can be added through plugins which are available from
the [plugins repository](https://github.com/rxi/lite-plugins); additional color
themes can be found in the [colors repository](https://github.com/rxi/lite-colors).
The editor can be customized by making changes to the
[user module](data/user/init.lua).

## Building
You can build the project yourself on Linux using the `build.sh` script
or on Windows using the `build.bat` script *([MinGW](https://nuwen.net/mingw.html) is required)*.
Note that the project does not need to be rebuilt if you are only making changes
to the Lua portion of the code.

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
