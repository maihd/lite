# TODOs
ToDo list, order by priority.

- Features (add when needed):
    - Make typing work with UniKey (Vietnamese typing method).
    - Line wrapping
    - Multi cursors
    - Next/Previous find with arrow keys
    - Utf8
    - Open binary file in preview-mode.
    - Key bindings docs (for Mai usage and MaiStyle). See more https://github.com/maihd/maienv/tree/main/keybinds
    - Recent files in `Open File From Project` command view
    - Mouse next/previous button (good for reading code)
    - Windows OS: Recent projects from Start Menu, Task Bar
    - Open project with command. Also support recents.
    - VCS status display
    - Remake all Lite Editor with LiteFx+Fennel -> MaiMacs

- Practices:
    - More productivity with Lite
    - Familiar with keybinds
    - Create some utils tools
    - Practice mode or tutorials
    - Review this project sources, memorize and gain knowledges

- Improvements:
    - New default theme (MaiBlue -> MaiAoi/MaiSora)
    - Background and animations (because I'm a gamer/gamedev, theses are big concerns)
    - Refactory syntax definition
    - Tab size detection
    - Ergonomics mouse interactions
    - Use fast string algorithms (code editing are working on string heavily)
    - TreeView:
        - Resizable
        - File type icon based on extensions/languages
        - Icon color
        - Text color
        - Display file status

- Fix bugs:
    - Unindent wrong or not work in some cases,
        specially when the file have different indent size with config
    - Mouse over in titlebar can be fallthrough from other window
    - Crash when long searching progress (commonly with Project Search)
        - Reproduce: search in project with `previous`
        - Reason: `load_glyphset` return dangling pointer
    - MarkDown language highlight

- Native Runtime (will move to LiteFx soon):
    - Make app more robust.
    - Fast or flexible, friendly experience on IO operations. (Maybe add async IO)
    - Display launching message box with style and helper.
    - Add more render backends: OpenGL, Vulkan, Dear ImGui.
    - Better font rendering: FreeType, SDF, advance usage of `stb_truetype`
    - LiteFx: Framework to make desktop application with C (or other system languages) and Lua/Luau
        - Simple and robust C framework
        - Lua runtime selections: Lua52 (or 53, 54), LuaJIT, Luau
        - Fennel simple showcase for Emacs-like scripting experience
        - Can run LiteXL Lua data (LiteXL use Lua54, but there are no compat layer for LuaJIT)
        - Can run Pragtical Lua data

- Syntax highlights:
    - Simple syntax highlights are enough.
    - When have some syntax highlight is wrong, and cannot ignore, just fix it.
    - When plugins can solve, use it.
    - Unknown token should be have underline to present error. Token can be detected without the need of LSP.

## Things not todo

- Syntax highlights:
    - No need something complex like lsp or intellisense. You should remember the API.

- Unnessary software distributing (we are hacker, we install software from source code):
    - Compile to Lua to bytecode.
    - Package data. exe-only application. (data embed into exe, faster startup)
    - Make it embedding ready.
    - Installer
