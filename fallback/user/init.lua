-- put user settings here
-- this module will be loaded after everything else when the application starts

local keymap = require("core.keymap")
local config = require("core.config")
local style = require("core.style")

-- light theme:
-- require "user.colors.summer"

-- key binding:
keymap.add { ["ctrl+e"] = "core:find-file" }
-- keymap.add { ["ctrl+b"] = "treeview:toggle" }

-- fennel
--local fennel = require("user.fennel")
--table.insert(package.loaders or package.searchers, fennel.searcher)
--fennel.dofile("data/user/config.fnl")