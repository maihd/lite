---
███╗   ███╗ █████╗ ██╗    ██╗     ██╗████████╗███████╗    ███████╗██████╗ ██╗████████╗ ██████╗ ██████╗
████╗ ████║██╔══██╗██║    ██║     ██║╚══██╔══╝██╔════╝    ██╔════╝██╔══██╗██║╚══██╔══╝██╔═══██╗██╔══██╗
██╔████╔██║███████║██║    ██║     ██║   ██║   █████╗      █████╗  ██║  ██║██║   ██║   ██║   ██║██████╔╝
██║╚██╔╝██║██╔══██║██║    ██║     ██║   ██║   ██╔══╝      ██╔══╝  ██║  ██║██║   ██║   ██║   ██║██╔══██╗
██║ ╚═╝ ██║██║  ██║██║    ███████╗██║   ██║   ███████╗    ███████╗██████╔╝██║   ██║   ╚██████╔╝██║  ██║
╚═╝     ╚═╝╚═╝  ╚═╝╚═╝    ╚══════╝╚═╝   ╚═╝   ╚══════╝    ╚══════╝╚═════╝ ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
---


# Mai Lite TODOs

ToDo list, order by priority.
> This file will be open by default when open lite as exe folder.
> Use Ctrl+B to open file explorer.
> Use Ctrl+E to open file from project directory.

- Quality of Life features:
    - Line wrapping
    - Multi cursors (Ctrl + Alt + Up/Down)
    - Multi cursors split (Ctrl + Alt + L)
    - Next/Previous find with arrow keys
    - Recent files in `Open File From Project` command view
    - Mouse next/previous button (good for reading code) -> create better read source code experience
    - Smart word-expansion system (Ctrl + D)
    - Scope-aware bracket selection (Ctrl + Shift + M)
    - Goto anything (Ctrl + P)

- Features (add when needed):
    - Open binary file in preview-mode.
    - Windows OS: Recent projects from Start Menu, Task Bar
    - Open project with command. Also support recents.
    - VCS status display

- Safety:
    - Currently use LuaJIT that support FFI
    - No sandbox environment
    - Run `.lite_project.lua` without sandboxing
    - Directly support load file from anywhere

- Development experience:
    - Live coding
    - Live update `.lite_project.lua`, to add more common
    - REPL for Lua?
    - Quake-like console
    - Time for error.txt logs
    - New logging system: store log in native, help read log when launch failed

- Documentations:
    - Key bindings docs (for Mai usage and MaiStyle). See more https://github.com/maihd/maienv/tree/main/keybinds
    - C system docs

- Localization:
    - Make typing work with UniKey (Vietnamese typing method).
    - Rendering Utf8 text

- Practices:
    - More productivity with Lite
    - Familiar with split views
    - Familiar with keybinds
    - Create some utils tools
    - Practice mode or tutorials
    - Review this project sources, memorize and gain knowledges
    - Learn more about UI/UX design to improvements this editor

- Improvements:
    - New default theme (MaiBlue -> MaiAoi/MaiSora)
    - Background and animations (because I'm a gamer/gamedev, theses are big concerns)
    - Refactory syntax definition, better handle scope, lpeg for complex syntax
    - Tab size detection (good for long names)
    - Ergonomics mouse interactions
    - Use fast string algorithms (code editing are working on string heavily)
    - TreeView -> File Explorer:
        - Resizable
        - File type icon based on extensions/languages
        - Icon color
        - Text color
        - Display file status
        - Focusable with keyboard
    - Console:
        - Clear
    - LogView

- Issues:
    - Ascii art

- Fix bugs:
    - Create docview -> save file -> use existing file name -> existing file is written without asking.
    - When do cmd "Root: Close All". Reproduce steps:
        - Unsaved docs
        - Request saving docs or force close
        - Focus to unsaved docs
        - Cannot return to CommandView, do some weird key stroke to return CommandView
        - Cannot close CommandView, even after choose "Save and Close"
        - After that, cannot close CommandView, even refocus CommandView
    - Change tab name (usually occured when save as, saving unsaved file), does not changing the tab size
    - Unindent wrong or not work in some cases, specially when the file have different indent size with config
    - Mouse over in titlebar can be fallthrough from other window
    - Crash when long searching progress (commonly with Project Search)
        - Reproduce: search in project with `previous`
        - Reason: `load_glyphset` return dangling pointer
    - MarkDown language highlight

- Native Runtime:
    - Unity build. Simpler workflow.
    - Building with TCC for testing purposes. Make Lite becoming more lite, this is madness but fun.
    - Use SDL3, and https://github.com/septag/dmon for better backends
    - `build.bat` is commonly export to global terminal space -> rename to other scripts
    - Make app more robust. Crash report.
    - Fast or flexible, friendly experience on IO operations. (Maybe add async IO)
    - Display launching message box with style and helper. (Sorry, Lite launching process is too fast)
    - Better text rendering: FreeType, SDF, advance usage of `stb_truetype` -> Fonstash
    - LiteFx: Framework to make desktop application with C (or other system languages) and Lua/Luau
        - I don't think that I will need this
        - Because of lazy, I still don't have any motivate to move to Luau
        - Simple and robust C framework for create text tools
        - Add more render backends: SDL3, Raylib, Dear ImGui.
        - Lua runtime selections (LiteLua): Lua52 (or 53, 54), LuaJIT, Luau

## Things not todo

- Syntax highlights:
    - No need something complex like lsp or intellisense. You should remember the API.
    - Simple syntax highlights are enough.
    - When have some syntax highlight is wrong, and cannot ignore, just fix it.
    - When plugins can solve, use it.
    - Token can be detected without the need of LSP.

- Unnessary software distributing (we are hacker, we install software from source code):
    - Compile to Lua to bytecode.
    - Package data. exe-only application. (data embed into exe, faster startup)
    - Make it embedding ready.
    - Installer
