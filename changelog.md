## Change from rxi/lite
- Important: Keybinds is difference from rxi/lite
- Use LuaJIT-2.1.0-beta3 instead of Lua 5.2.
- Check directory changed based on last write time instead of file diffs.
- Add premake5 to generate Visual Studio project, for debugging purpose.
- Add clang build script on Windows.
- Display MessageBox when app failed to launch.
- Add clang-format.
- Skip opening binary file.
- Fix BOM on Unicode files.
- Languages: `Batch`, `C#`, `C`.
- Double click log line to open file.
- Refactory C sources.
- Align line numbers near code.
- Display icon to show log type.
- Draw intent guide line. (may be need more mechanic for syntax highlight)
- Display project name in title bar and status bar.
- Convert to use native API instead of large library like SDL. (Still have fallback to SDL when native API version is buggy)

- Separated mouse button events:
    - Left Click for selection
    - Right Click for open menu context (WIP, currently no-ops)
    - Middle Click for close tab
    - X1, X2 for backward and forward (WIP, currently no-ops)

- Fix bugs:
    - Some time single mouse click cause select token, line.
    - Extra space for end of file is too large
    - TreeView have no scroll rect
    - Project directory end with '\' or '/' resulting empty project directory name
    - Toggle comment not work when:
        - Prefix have no space between content
        - Prefix does not start at begin of the line
    - Cannot open file with command when focus TreeView (or locked view) -> fix with TreeView cannot focus
    - Cannot return from TreeView to DocView -> fix with TreeView cannot focus

- Better mouse interacting.
- Fallback version for fixing scripts (safe mode).
- Bootstrap version for debugging and development.
    - Realize that thing exactly what above things did, but with different words, the problem turn out was very simple.

- Software distributing:
    - Add item to OS context menu
    - Add update script
    - Add install script
    - Add sync fallback script

- Ignore folders, files (blacklist)
- Better log and log style
- Custom titlebar, borderless window:
    - Fix maximize wrong size
    - Add window border
    - Resizing window
    - Corecting moving window when holding titlebar

- Workflow:
    - Have an fallback version when `build_clang.bat` failed to edit code

- VSCode features (what I feel its' good):
    - Select empty space with double clicks
    - Soft tab VSCode-like
    - Toggle comment with right indent
    - Add indent should jump to next level indent

- Add polyfill `table.unpack`. (Lua52 compat, may help work with other plugins)
    - Not reliable anymore, LiteXL use Lua54
    - But can use old Lite plugins
    - Maybe Pragtical plugins (it use LuaJIT by default)
