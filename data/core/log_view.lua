local core = require "core"
local style = require "core.style"
local View = require "core.view"


local LogView = View:extend()


function LogView:new()
    LogView.super.new(self)
    self.last_item = core.log_items[#core.log_items]
    self.selected_idx = 0
    self.scrollable = true
    self.yoffset = 0
end

function LogView:get_name()
    return "Log"
end

function LogView:update()
    local item = core.log_items[#core.log_items]
    if self.last_item ~= item then
        self.last_item = item
        self.scroll.to.y = 0
        self.yoffset = -(style.font:get_height() + style.padding.y)
    end

    self:move_towards("yoffset", 0)

    LogView.super.update(self)
end

local function draw_text_multiline(font, text, x, y, color)
    local th = font:get_height()
    local resx, resy = x, y
    for line in text:gmatch("[^\n]+") do
        resy = y
        resx = renderer.draw_text(style.font, line, x, y, color)
        y = y + th
    end
    return resx, resy
end


local function move_text_multiline(font, text, x, y, color)
    local th = font:get_height()
    local resx, resy = x, y
    for line in text:gmatch("[^\n]+") do
        resy = y
        --resx = renderer.draw_text(style.font, line, x, y, color)
        y = y + th
    end
    return resx, resy
end


function LogView:on_mouse_pressed(button, mx, my, clicks)
    LogView.super.on_mouse_pressed(self, button, mx, my, clicks)
    
    if button == "left" and clicks >= 2 and self.selected_idx > 0 then
        local item = core.log_items[self.selected_idx]
        if not item then
            core.error("Log item at " .. self.selected_idx .. " is nil")
            return
        end

        local file
        local colon_index = 0
        for i = #item.at, 1, -1 do
            if item.at:byte(i) == string.byte(':', 1) then
                file = item.at:sub(1, i - 1)
                colon_index = i
                break
            end
        end

        if file then
            -- core.log("Open file from log view: " .. file)
            local doc = core.open_doc(file)
            local line_number = tonumber(item.at:sub(colon_index + 1))
            doc:set_selection(line_number, 1, line_number, 1)

            local doc_view = core.root_view:open_doc(core.open_doc(file))
            -- doc_view:scroll_to_line(line_number)
        else
            core.error("Failed to open file: " .. item.at)
        end
    end
end


function LogView:on_mouse_moved(mx, my, ...)
    LogView.super.on_mouse_moved(self, mx, my, ...)
    
    self.selected_idx = 0

    local ox, oy = self:get_content_offset()
    local th = style.font:get_height()
    local w, h = renderer.get_size()

    local x = ox + style.padding.x
    local y = oy + style.padding.y + self.yoffset

    for i = #core.log_items, 1, -1 do
        if mx >= x and my >= y - style.padding.y * 0.5 and mx < w and my < y + th + style.padding.y then
            self.selected_idx = i
            break
        end

        local item = core.log_items[i]
        x, y = move_text_multiline(style.font, item.text, x, y, style.text)
        y = y + th
        
        if item.info then
            x, y = move_text_multiline(style.font, item.info, x, y, style.dim)
            y = y + th
        end
        y = y + style.padding.y

        -- Stop drawing when out of scope
        if y > h + th then
            break
        end
    end
end


function LogView:draw()
    self:draw_background(style.background)

    local ox, oy = self:get_content_offset()
    local th = style.font:get_height()
    local y = oy + style.padding.y + self.yoffset
    local w, h = renderer.get_size()

    for i = #core.log_items, 1, -1 do
        local color = style.text

        -- Draw highlight
        if i == self.selected_idx then
            color = style.accent
            renderer.draw_rect(style.padding.x, y - style.padding.y * 0.5, w, th + style.padding.y, style.line_highlight)
        end

        -- Draw date time
        local x = ox + style.padding.x
        local item = core.log_items[i]
        local time = os.date(nil, item.time)
        x = renderer.draw_text(style.font, time, x, y, style.dim)
        x = x + style.padding.x

        -- Draw log content
        x, y = draw_text_multiline(style.font, item.text, x, y, color)

        -- Draw file and position call log
        renderer.draw_text(style.font, " at " .. item.at, x, y, style.dim)
        y = y + th
        if item.info then
            local subx = x
            subx, y = draw_text_multiline(style.font, item.info, subx, y, style.dim)
            y = y + th
        end
        y = y + style.padding.y

        -- Stop drawing when out of scope
        if y > h + th then
            break
        end
    end
end

return LogView
