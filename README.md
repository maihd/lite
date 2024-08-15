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
If you sometimes found this fork is complex or hard to understand, remember this physiology phrase: The *simple* will be keep in the interfaces, *implementation* will be seperated into two categories: *simple* and *optimized*.
This editor don't and won't support C++ editting. For C++, I will use full IDE for the jobs.

Production ready Lite forks (recommended):
- Lite XL Editor: https://lite-xl.com/
- Pragtical Editor: https://pragtical.dev/

## Install and Update
- Run install.bat to install. Which contains theses steps:
    - Building with clang
    - Add edit with lite to OS context menu
    - Add path to environment

- Run update.bat to update. Which contains theses steps:
    - Pulling from github repo
    - Building with clang

## Changes
See [changelog.md](/changelog.md)

## TODOs
See [todos.md](/todos.md)

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
