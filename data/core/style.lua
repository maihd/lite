-- Default style of Lite

local common = require("core.common")
local style = {}

-- Size

style.padding           = { x = common.round(14 * SCALE), y = common.round(7 * SCALE) }
style.titlebar_padding  = { x = common.round(14 * SCALE), y = common.round(12 * SCALE) }

style.divider_size      = common.round(1 * SCALE)
style.scrollbar_size    = common.round(4 * SCALE)
style.caret_width       = common.round(2 * SCALE)
style.tab_width         = common.round(170 * SCALE)


-- Fonts

style.font      = renderer.font.load(EXEDIR .. "/data/fonts/ZeitungMonoProNerdFont-Regular.ttf", 14 * SCALE) -- renderer.font.load(EXEDIR .. "/data/fonts/font.ttf", 14 * SCALE)
style.big_font  = renderer.font.load(EXEDIR .. "/data/fonts/ZeitungMonoProNerdFont-Regular.ttf", 24 * SCALE) -- renderer.font.load(EXEDIR .. "/data/fonts/font.ttf", 34 * SCALE)
style.icon_font = renderer.font.load(EXEDIR .. "/data/fonts/icons.ttf", 15 * SCALE)
-- style.code_font = renderer.font.load(EXEDIR .. "/data/fonts/RobotoMono-Medium.ttf", 15 * SCALE) -- For multi language text rendering support
-- style.code_font = renderer.font.load(EXEDIR .. "/data/fonts/CascadiaCode.ttf", 14 * SCALE) -- For multi language text rendering support
style.code_font = renderer.font.load(EXEDIR .. "/data/fonts/ZeitungMonoProNerdFont-Regular.ttf", 14 * SCALE)
style.titlebar_font = renderer.font.load(EXEDIR .. "/data/fonts/ZeitungMonoProNerdFont-Regular.ttf", 20 * SCALE)


-- Colors

style.background        = { common.color "#2e2e32" }
style.background2       = { common.color "#252529" }
style.background3       = { common.color "#252529" }
style.text              = { common.color "#97979c" }
style.caret             = { common.color "#93DDFA" }
style.caret2            = { common.color "#ceeffd" }
style.accent            = { common.color "#e1e1e6" }
style.dim               = { common.color "#525257" }
style.divider           = { common.color "#202024" }
style.selection         = { common.color "#48484f" }
style.line_number       = { common.color "#525259" }
style.line_number2      = { common.color "#83838f" }
style.line_highlight    = { common.color "#343438" }
style.scrollbar         = { common.color "#414146" }
style.scrollbar2        = { common.color "#4b4b52" }
style.scope_line        = { common.color "#494950" }


-- MaiHD custom colors

style.info              = { common.color "#97979c" }
style.warn              = { common.color "#f7c95c" }
style.error             = { common.color "#F77483" }

style.file              = { common.color "#93DDFA" }
style.file_dirty        = { common.color "#F77483" }

style.titlebar              = { common.color "#111111" }
style.titlebar_icon         = { common.color "#e1e1e6" }
style.titlebar_icon_hover   = { common.color "#ff2b44" }
style.titlebar_background   = { common.color "#111113" }

-- Common syntax colors

style.syntax = {}
style.syntax["normal"]      = { common.color "#e1e1e6" }
style.syntax["symbol"]      = { common.color "#e1e1e6" }
style.syntax["comment"]     = { common.color "#676b6f" }
style.syntax["keyword"]     = { common.color "#E58AC9" }
style.syntax["keyword2"]    = { common.color "#F77483" }
style.syntax["number"]      = { common.color "#FFA94D" }
style.syntax["literal"]     = { common.color "#FFA94D" }
style.syntax["string"]      = { common.color "#f7c95c" }
style.syntax["operator"]    = { common.color "#93DDFA" }
style.syntax["function"]    = { common.color "#93DDFA" }


-- Status view icon
style.icons = {}
style.icons.file            = "f"
style.icons.folder          = "d"
style.icons.graph           = "g"
style.icons.language        = "l" -- Speaking/natural language
style.icons.proglang        = "p" -- Programming language

return style
