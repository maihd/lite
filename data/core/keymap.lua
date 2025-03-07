local command = require "core.command"
local keymap = {}

keymap.modkeys = {}
keymap.map = {}
keymap.reverse_map = {}

local modkey_map = {
    ["left ctrl"]   = "ctrl",
    ["right ctrl"]  = "ctrl",
    ["left shift"]  = "shift",
    ["right shift"] = "shift",
    ["left alt"]    = "alt",
    ["right alt"]   = "altgr",
    ["left windows"]    = "windows",
    ["right windows"]   = "windows",
}

local modkeys = { "ctrl", "alt", "altgr", "shift" }

local function key_to_stroke(k)
    local stroke = ""
    for _, mk in ipairs(modkeys) do
        if keymap.modkeys[mk] then
            stroke = stroke .. mk .. "+"
        end
    end
    return stroke .. k
end


function keymap.add(map, overwrite)
    for stroke, commands in pairs(map) do
        if type(commands) == "string" then
            commands = { commands }
        end
        if overwrite then
            keymap.map[stroke] = commands
        else
            keymap.map[stroke] = keymap.map[stroke] or {}
            for i = #commands, 1, -1 do
                table.insert(keymap.map[stroke], 1, commands[i])
            end
        end
        for _, cmd in ipairs(commands) do
            keymap.reverse_map[cmd] = stroke
        end
    end
end


function keymap.get_binding(cmd)
    return keymap.reverse_map[cmd]
end


function keymap.on_key_pressed(k)
    local mk = modkey_map[k]
    if mk then
        keymap.modkeys[mk] = true
        -- work-around for windows where `altgr` is treated as `ctrl+alt`
        if mk == "altgr" then
            keymap.modkeys["ctrl"] = false
        end
    else
        local stroke = key_to_stroke(k)
        local commands = keymap.map[stroke]
        if commands then
            for _, cmd in ipairs(commands) do
                local performed = command.perform(cmd)
                if performed then break end
            end
            return true
        end
    end
    return false
end


function keymap.on_key_released(k)
    local mk = modkey_map[k]
    if mk then
        keymap.modkeys[mk] = false
    end
end


keymap.add {
    ["alt+x"] = "core:find-command",
    ["ctrl+p"] = "core:find-file",
    ["ctrl+o"] = "core:open-file",
    ["ctrl+n"] = "core:new-doc",
    ["f11"] = "core:toggle-fullscreen",

    ["ctrl+alt+shift+left"] = "root:split-left",
    ["ctrl+alt+shift+right"] = "root:split-right",
    ["ctrl+alt+shift+up"] = "root:split-up",
    ["ctrl+alt+shift+down"] = "root:split-down",

    -- @todo(maihd): add switch-toggle command
    ["alt+shift+left"] = "root:switch-to-left",
    ["alt+shift+right"] = "root:switch-to-right",
    ["alt+shift+up"] = "root:switch-to-up",
    ["alt+shift+down"] = "root:switch-to-down",

    ["ctrl+w"] = "root:close",
    ["ctrl+tab"] = "root:switch-to-next-tab",
    ["ctrl+shift+tab"] = "root:switch-to-previous-tab",
    ["ctrl+pageup"] = "root:move-tab-left",
    ["ctrl+pagedown"] = "root:move-tab-right",
    ["ctrl+1"] = "root:switch-to-tab-1",
    ["ctrl+2"] = "root:switch-to-tab-2",
    ["ctrl+3"] = "root:switch-to-tab-3",
    ["ctrl+4"] = "root:switch-to-tab-4",
    ["ctrl+5"] = "root:switch-to-tab-5",
    ["ctrl+6"] = "root:switch-to-tab-6",
    ["ctrl+7"] = "root:switch-to-tab-7",
    ["ctrl+8"] = "root:switch-to-tab-8",
    ["ctrl+9"] = "root:switch-to-tab-9",

    ["ctrl+f"] = "find-replace:find",
    ["ctrl+r"] = "find-replace:replace",
    ["f3"] = "find-replace:repeat-find",
    ["shift+f3"] = "find-replace:previous-find",
    ["ctrl+g"] = "doc:go-to-line",
    ["ctrl+s"] = "doc:save",
    ["ctrl+shift+s"] = "doc:save-as",

    ["ctrl+z"] = "doc:undo",
    ["ctrl+shift+z"] = "doc:redo",
    ["ctrl+x"] = "doc:cut",
    ["ctrl+c"] = "doc:copy",
    ["ctrl+v"] = "doc:paste",
    ["escape"] = { "command:escape", "doc:select-none" },
    ["tab"] = { "command:complete", "doc:indent" },
    ["shift+tab"] = "doc:unindent",
    ["backspace"] = "doc:backspace",
    ["shift+backspace"] = "doc:backspace",
    ["ctrl+backspace"] = "doc:delete-to-previous-word-start",
    ["ctrl+shift+backspace"] = "doc:delete-to-previous-word-start",
    ["delete"] = "doc:delete",
    ["shift+delete"] = "doc:delete",
    ["ctrl+delete"] = "doc:delete-to-next-word-end",
    ["ctrl+shift+delete"] = "doc:delete-to-next-word-end",
    ["return"] = { "command:submit", "doc:newline" },
    ["keypad enter"] = { "command:submit", "doc:newline" },

    ["ctrl+return"] = "doc:newline-below",
    ["ctrl+shift+return"] = "doc:newline-above",
    ["ctrl+j"] = "doc:join-lines",
    ["ctrl+a"] = "doc:select-all",
    ["ctrl+d"] = { "find-replace:select-next", "doc:select-word" },
    ["ctrl+l"] = "doc:select-lines",
    ["ctrl+/"] = "doc:toggle-line-comments",

    ["alt+up"] = "doc:move-lines-up",
    ["alt+down"] = "doc:move-lines-down",

--     ["alt+shift+up"] = { "doc:duplicate-lines", "command:select-previous", "doc:move-to-previous-line" },
--     ["alt+shift+down"] = "doc:duplicate-lines",

    ["ctrl+shift+d"] = "doc:duplicate-lines",
    ["ctrl+shift+k"] = "doc:delete-lines",

    ["left"] = "doc:move-to-previous-char",
    ["right"] = "doc:move-to-next-char",
    ["up"] = { "command:select-previous", "doc:move-to-previous-line" },
    ["down"] = { "command:select-next", "doc:move-to-next-line" },
    ["ctrl+left"] = "doc:move-to-previous-word-start",
    ["ctrl+right"] = "doc:move-to-next-word-end",
    ["ctrl+up"] = "doc:move-to-previous-block-start",
    ["ctrl+down"] = "doc:move-to-next-block-end",
    ["home"] = "doc:move-to-start-of-line",
    ["end"] = "doc:move-to-end-of-line",
    ["ctrl+home"] = "doc:move-to-start-of-doc",
    ["ctrl+end"] = "doc:move-to-end-of-doc",
    ["pageup"] = "doc:move-to-previous-page",
    ["pagedown"] = "doc:move-to-next-page",

    ["shift+left"] = "doc:select-to-previous-char",
    ["shift+right"] = "doc:select-to-next-char",
    ["shift+up"] = "doc:select-to-previous-line",
    ["shift+down"] = "doc:select-to-next-line",

    ["ctrl+shift+left"] = "doc:select-to-previous-word-start",
    ["ctrl+shift+right"] = "doc:select-to-next-word-end",
    ["ctrl+shift+up"] = "doc:select-to-previous-block-start",
    ["ctrl+shift+down"] = "doc:select-to-next-block-end",

    ["shift+home"] = "doc:select-to-start-of-line",
    ["shift+end"] = "doc:select-to-end-of-line",
    ["ctrl+shift+home"] = "doc:select-to-start-of-doc",
    ["ctrl+shift+end"] = "doc:select-to-end-of-doc",
    ["shift+pageup"] = "doc:select-to-previous-page",
    ["shift+pagedown"] = "doc:select-to-next-page",
}

return keymap
