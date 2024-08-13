local core = require("core")
local command = require("core.command")
local keymap = require("core.keymap")
local console = require("plugins.console")

command.add(nil, {
    ["project:build-project"] = function()
        local prefix = (PLATFORM == "Windows" and "" or "./")
        local ext = (PLATFORM == "Windows" and ".bat" or ".sh")
        core.log "Building..."
        console.run {
            command = prefix .. "build" .. ext,
            file_pattern = "(.*):(%d+):(%d+): (.*)$",
            on_complete = function() core.log "Build complete" end,
        }
    end
})

keymap.add { ["ctrl+shift+b"] = "project:build-project" }

core.add_ignore { "^libs", "^.build", "^.project", "^.fallback", }
