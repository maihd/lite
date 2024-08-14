local core = require "core"
local common = require "core.common"
local command = require "core.command"
local config = require "core.config"
local style = require "core.style"
local DocView = require "core.doc_view"
local LogView = require "core.log_view"
local View = require "core.view"


local TitleBar = View:extend()

TitleBar.separator  = "  "
TitleBar.separator2 = " | "


function TitleBar:new()
    TitleBar.super.new(self)
end


function TitleBar:show_message(icon, icon_color, text)
end


function TitleBar:update()
    self.size.y = style.font:get_height() + style.padding.y * 2
    self.scroll.to.y = 0

    local turn_off = true -- Temporary turn titlebar off
    if turn_off then
        self.size.y = 0
        system.show_window_titlebar()
    else
        system.hide_window_titlebar()
    end

    TitleBar.super.update(self)
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


local function draw_right_items(self, items, x, y, item_size, draw_fn)
    local font = style.font
    local color = style.text

    local i = 0
    for _, item in ipairs(items) do
        if type(item) == "userdata" then
            font = item
        elseif type(item) == "table" then
            color = item
        else
            local w = draw_fn(font, { 0, 0, 0, 0 }, item, nil, 0, y, 0, self.size.y)
            draw_fn(font, color, item, nil, x + i * item_size + (item_size - w) * 0.5, y, 0, self.size.y)
            i = i + 1
        end
    end

    return x + i * item_size
end


local function text_width(font, _, text, _, x)
    return x + font:get_width(text)
end


function TitleBar:on_mouse_moved(x, y, dx, dy)
    -- Handle moving window

    if self.moving then
        local mx, my = system.get_global_mouse_position()
        system.set_window_position(mx - self.mouse_dx, my - self.mouse_dy)
        return
    end

    -- Handle hover buttons

    local items = self:get_items()
    local pos_x, pos_y = self:get_content_offset()

    local item_size = 46
    local w = draw_right_items(self, items, 0, 0, item_size, text_width)
    pos_x = pos_x + self.size.x - w

    if x < pos_x or x > self.size.x then
        self.hover_index = nil
        return
    end

    if y < pos_y or y > self.size.y then
        self.hover_index = nil
        return
    end

    local item_size = 48
    local dx = x - pos_x
    local i = math.floor(dx / item_size)
    self.hover_index = i
end


function TitleBar:on_mouse_pressed(button, x, y, clicks)
    if button ~= "left" then
        return
    end

    -- Minimize
    if self.hover_index == 0 then
        system.minimize_window()
        return
    end

    -- Maximize
    if self.hover_index == 1 then
        system.toggle_maximize_window()
        return
    end

    -- Close
    if self.hover_index == 2 then
        system.close_window()
        return
    end

    self.moving = true

    local mx, my = system.get_global_mouse_position()
    local wx, wy = system.get_window_position()

    self.mouse_dx = mx - wx
    self.mouse_dy = my - wy
end


function TitleBar:on_mouse_released(button, x, y, clicks)
    if self.moving then
        self.moving = false
    end
end


function TitleBar:draw_items(items, right_align, yoffset)
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


function TitleBar:draw_right_items(items, yoffset)
    local x, y = self:get_content_offset()

    local item_size = 46
    local w = draw_right_items(self, items, 0, 0, item_size, text_width)
    x = x + self.size.x - w

    if self.hover_index then
        renderer.draw_rect(x + self.hover_index * item_size, y, item_size, self.size.y, style.syntax["keyword2"])
    end

    draw_right_items(self, items, x, y, item_size, common.draw_text)
end


function TitleBar:get_items()
    return {
        style.file, style.font, "{L}",
        style.accent, style.font, style.dim, self.separator2,
        style.accent, style.font, core.window_title,
    }, {
        -- Minimize Button
        style.text, "-",

        -- Maximize Button
        style.text, "=",

        -- Close button
        style.text, "X",
    }
end


function TitleBar:draw()
    self:draw_background(style.background2)

--     if self.message then
--         self:draw_items(self.message, false, self.size.y)
--     end

    local left, right = self:get_items()
    self:draw_items(left)
    self:draw_right_items(right)
end


return TitleBar
