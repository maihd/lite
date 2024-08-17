# TODOs
ToDo list, order by priority.

- Features (add when needed):
    - Select empty space
    - Line wrapping
    - Multi cursors
    - Next/Previous find with arrow keys
    - Utf8
    - Add polyfill `table.unpack`. (Lua52 compat, may help work with other plugins)
    - Make typing work with UniKey (Vietnamese typing method).
    - Open binary file in preview-mode.
    - Use fast string algorithms (code editing are working on string heavily)
    - Key bindings docs (for Mai usage and MaiStyle). See more https://github.com/maihd/maienv/tree/main/keybinds
    - Recent files in `Open File From Project` command view
    - Mouse next/previous button (good for reading code)
    - VCS status display

- Fix bugs:
    - Scope line wrong position (should be have the position of the first non-space char)
    - MarkDown language highlight
    - Modal tabbing (Vi-like) does not trim. Does it should trim?
    - Crash when long searching progress (commonly with Project Search)
        - Reproduce: search in project with `previous`
        - Reason: `load_glyphset` return dangling pointer

- Native Runtime:
    - Make app more robust.
    - Fast or flexible, friendly experience on IO operations. (Maybe add async IO)
    - Display launching message box with style and helper.
    - Add more render backends: OpenGL, Vulkan, Dear ImGui.
    - Better font rendering: FreeType, SDF, advance usage of `stb_truetype`
    - LiteFx: Framework to make desktop application with C (or other system languages) and Lua/Luau
        - Simple and robust C framework
        - Can be customize Lua runtime: Lua52 (or 53, 54), LuaJIT, Luau
        - Fennel simple showcase for Emacs-like scripting experience
        - Can run LiteXL Lua data (Use Lua54, but there are no compat layer for LuaJIT)
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
