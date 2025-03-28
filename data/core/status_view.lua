local core = require "core"
local common = require "core.common"
local command = require "core.command"
local config = require "core.config"
local style = require "core.style"
local DocView = require "core.doc_view"
local LogView = require "core.log_view"
local View = require "core.view"


local StatusView = View:extend()

StatusView.separator  = "  "
StatusView.separator2 = " | "


function StatusView:new()
    StatusView.super.new(self)
    self.focusable = false
    self.message_timeout = 0
    self.message = {}
end


function StatusView:on_mouse_pressed(button, x, y, clicks)
    if button ~= "left" then
        return
    end

--     core.set_active_view(core.last_active_view)

    if system.get_time() < self.message_timeout
        and not core.active_view:is(LogView)
    then
        command.perform "core:open-log"
    end
end


function StatusView:show_message(icon, icon_color, text)
    self.message = {
        icon_color, style.icon_font, icon,
        style.dim, style.font, StatusView.separator2, style.text, text
    }
    self.message_timeout = system.get_time() + config.message_timeout
end


function StatusView:update()
    self.size.y = style.font:get_height() + style.padding.y * 2

    if system.get_time() < self.message_timeout then
        self.scroll.to.y = self.size.y
    else
        self.scroll.to.y = 0
    end

    StatusView.super.update(self)
end


local function draw_items(self, items, x, y, draw_fn)
    local font = style.font
    local color = style.text

    for _, item in ipairs(items) do
        if type(item) == "userdata" then
            font = item
        elseif type(item) == "table" then
            color = item
        else
            x = draw_fn(font, color, item, nil, x, y, 0, self.size.y)
        end
    end

    return x
end


local function text_width(font, _, text, _, x)
    return x + font:get_width(text)
end


function StatusView:draw_items(items, right_align, yoffset)
    local x, y = self:get_content_offset()
    y = y + (yoffset or 0)
    if right_align then
        local w = draw_items(self, items, 0, 0, text_width)
        x = x + self.size.x - w - style.padding.x
        draw_items(self, items, x, y, common.draw_text)
    else
        x = x + style.padding.x
        draw_items(self, items, x, y, common.draw_text)
    end
end


function StatusView:get_items()
    -- Status of doc
    if getmetatable(core.active_view) == DocView then
        local dv = core.active_view
        local line, col = dv.doc:get_selection()
        local dirty = dv.doc:is_dirty()

        return {
            dirty and style.file_dirty or style.file, style.icon_font, style.icons.file,
            style.dim, style.font, self.separator2, style.text,
            dv.doc.filename and style.text or style.dim, dv.doc:get_name(),
            style.text,
            self.separator,
            "Line: ", line,
            self.separator,
            col > config.line_limit and style.accent or style.text, "Columns: ", col,
            style.text,
            self.separator,
            "Scroll: ", string.format("%d%%", line / #dv.doc.lines * 100),
        }, {
            -- FPS
            style.font, "FPS",
            style.font, style.dim, self.separator2, style.text,
            style.font, tostring(core.fps):sub(1, 3),
            self.separator,

            -- Language
            style.icon_font, style.icons.proglang,
            style.font, style.dim, self.separator2, style.text,
            style.font, dv.doc:get_language_name(),
            self.separator,

            -- Lines
            style.icon_font, style.icons.graph,
            style.font, style.dim, self.separator2, style.text,
            #dv.doc.lines, " Lines",
            self.separator,

            -- Line ending
            dv.doc.crlf and "CRLF" or "LF"
        }
    end

    -- Status of project
    return {
        style.file, style.icon_font, style.icons.folder,
        style.accent, style.font, style.dim, self.separator2,
        style.accent, style.font, core.project_dir_name
    }, {
        -- FPS
        style.font, "FPS",
        style.font, style.dim, self.separator2, style.text,
        style.font, tostring(core.fps):sub(1, 3),
        self.separator,

        -- Files
        style.icon_font, style.icons.graph,
        style.font, style.dim, self.separator2,
        #core.docs, style.text, " / ",
        #core.project_files, " Files"
    }
end


-- When status view have small width
function StatusView:get_short_items()
    -- Status of doc
    if getmetatable(core.active_view) == DocView then
        local dv = core.active_view
        local line, col = dv.doc:get_selection()
        local dirty = dv.doc:is_dirty()

        return {
            dirty and style.file_dirty or style.file, style.icon_font, style.icons.file,
            style.dim, style.font, self.separator2, style.text,
            dv.doc.filename and style.text or style.dim, dv.doc:get_name(),
            style.text,
            self.separator,
            line, ":", col, " - ", string.format("%d%%", line / #dv.doc.lines * 100)
        }, {
            -- FPS (No FPS in short status view)
            -- style.font, "FPS",
            -- style.font, style.dim, self.separator2, style.text,
            -- style.font, tostring(core.fps):sub(1, 3),
            -- self.separator,

            -- Language
            style.icon_font, style.icons.proglang,
            style.font, style.dim, self.separator2, style.text,
            style.font, dv.doc:get_language_name(),
            self.separator,

            -- Lines
            style.icon_font, style.icons.graph,
            style.font, style.dim, self.separator2, style.text,
            #dv.doc.lines, " Lines",
            self.separator,

            -- Line ending
            dv.doc.crlf and "CRLF" or "LF"
        }
    end

    -- Status of project
    return {
        style.file, style.icon_font, style.icons.folder,
        style.accent, style.font, style.dim, self.separator2,
        style.accent, style.font, core.project_dir_name
    }, {
        -- FPS (no FPS when short width)
        -- style.font, "FPS",
        -- style.font, style.dim, self.separator2, style.text,
        -- style.font, tostring(core.fps):sub(1, 3),
        -- self.separator,

        -- Files
        style.icon_font, style.icons.graph,
        style.font, style.dim, self.separator2,
        #core.docs, style.text, " / ",
        #core.project_files, " Files"
    }
end


function StatusView:draw()
    self:draw_background(style.background2)

    if self.message then
        self:draw_items(self.message, false, self.size.y)
    end

    local left, right = self:get_items()

    -- Calculate widths
    local left_width = draw_items(self, left, 0, 0, text_width)
    local right_width = draw_items(self, right, 0, 0, text_width)
    if left_width + style.padding.x > self.size.x - style.padding.x - right_width then
        left, right = self:get_short_items()
    end

    self:draw_items(left)
    self:draw_items(right, true)
end


return StatusView
