local core = require "core"
local common = require "core.common"
local command = require "core.command"
local config = require "core.config"
local keymap = require "core.keymap"
local style = require "core.style"
local View = require "core.view"

config.treeview_size = 200 * SCALE

local function get_depth(filename)
    local n = 0
    for sep in filename:gmatch("[\\/]") do
        n = n + 1
    end
    return n
end


local TreeView = View:extend()


function TreeView:new()
    TreeView.super.new(self)

    self.focusable = false
    self.scrollable = true
    self.visible = true
    self.init_size = true
    self.cache = {}
    self.scrollable_size = 0
end


function TreeView:get_cached(item)
    local t = self.cache[item.filename]
    if not t then
        t = {}
        t.filename = item.filename
        t.abs_filename = system.absolute_path(item.filename)
        t.name = t.filename:match("[^\\/]+$")
        t.depth = get_depth(t.filename)
        t.type = item.type
        self.cache[t.filename] = t
    end
    return t
end


function TreeView:get_name()
    return "---"
end


function TreeView:get_item_height()
    return style.font:get_height() + style.padding.y
end


function TreeView:get_scrollable_size()
    if self.scrollable_size == 0 then
        return math.huge
    end

    return math.max(self.scrollable_size, self.size.y)
end


function TreeView:check_cache()
    -- invalidate cache's skip values if project_files has changed
    if core.project_files ~= self.last_project_files then
        for _, v in pairs(self.cache) do
            v.skip = nil
        end
        self.last_project_files = core.project_files
    end
end


function TreeView:each_item()
    return coroutine.wrap(function ()
        self:check_cache()

        local scrollable_size = 0

        local ox, oy = self:get_content_offset()
        local y = oy + style.padding.y
        local w = self.size.x
        local h = self:get_item_height()

        local i = 1
        while i <= #core.project_files do
            local item = core.project_files[i]
            local cached = self:get_cached(item)

            coroutine.yield(cached, ox, y, w, h)
            y = y + h
            i = i + 1

            scrollable_size = scrollable_size + h

            if not cached.expanded then
                if cached.skip then
                    i = cached.skip
                else
                    local depth = cached.depth
                    while i <= #core.project_files do
                        local filename = core.project_files[i].filename
                        if get_depth(filename) <= depth then break end
                        i = i + 1
                    end
                    cached.skip = i
                end
            else
                scrollable_size = scrollable_size + h * cached.depth
            end
        end

        scrollable_size = scrollable_size + h * 4
        if scrollable_size > self.scrollable_size then
            self.scrollable_size = scrollable_size
        end
    end)
end


function TreeView:on_mouse_moved(px, py)
    self.hovered_item = nil
    for item, x, y, w, h in self:each_item() do
        if px > x and py > y and px <= x + w and py <= y + h then
            self.hovered_item = item
            break
        end
    end

    if self.hovered_item then
        self.cursor = "hand"
    else
        self.cursor = "arrow"
    end
end


function TreeView:on_mouse_pressed(button, x, y)
    if button ~= "left" then
        return
    end

    if not self.hovered_item then
        return
    elseif self.hovered_item.type == "dir" then
        self.hovered_item.expanded = not self.hovered_item.expanded
        self.scrollable_size = 0 -- Set dirty to recalculate scrollable_size
    else
        core.try(function ()
            local doc = core.open_doc(self.hovered_item.filename)
            if doc then
                core.root_view:open_doc(doc)
            end
        end)
    end
end


function TreeView:update()
    TreeView.super.update(self)

    -- update width
    local dest = self.visible and config.treeview_size or 0
    if self.init_size then
        self.size.x = dest
        self.init_size = false
    else
        self:move_towards(self.size, "x", dest)
    end
end


function TreeView:draw()
    self:draw_background(style.background2)

    local icon_width = style.icon_font:get_width("D")
--     local spacing = style.font:get_width(" ") * 2
    local spacing = 2

    local doc = core.active_view.doc
    local active_filename = doc and system.absolute_path(doc.filename or "")

    for item, x, y, w, h in self:each_item() do
        local color = style.text

        -- highlight active_view doc
        if item.abs_filename == active_filename then
            color = style.accent
        end

        -- hovered item background
        if item == self.hovered_item then
            renderer.draw_rect(x, y, w, h, style.line_highlight)
            color = style.accent
        end

        -- icons
        x = x + item.depth * style.padding.x + style.padding.x
        if item.type == "dir" then
            local icon1 = item.expanded and "-" or "+"
            local icon2 = item.expanded and "D" or "d"
            common.draw_text(style.icon_font, color, icon1, nil, x, y, 0, h)
            x = x + style.padding.x
            common.draw_text(style.icon_font, color, icon2, nil, x, y, 0, h)
            x = x + icon_width
        else
            x = x + style.padding.x
            common.draw_text(style.icon_font, color, "f", nil, x, y, 0, h)
            x = x + icon_width
        end

        -- text
        x = x + spacing
        x = common.draw_text(style.font, color, item.name, nil, x, y, 0, h)
    end

    self:draw_scrollbar()
end


-- init
local view = TreeView()
local node = core.root_view:get_active_node()
node:split("left", view, true)

-- register commands and keymap
command.add(nil, {
    ["treeview:toggle"] = function()
        view.visible = not view.visible
    end,

    ["treeview:toggle-focus"] = function()
        view.visible = not view.visible
        -- TreeView cannot focus now
        -- if view.visible then
        --     core.set_active_view(view)
        -- else
        --     core.set_active_view(core.last_active_view)
        -- end
    end
})

keymap.add { ["ctrl+\\"] = "treeview:toggle" }
keymap.add { ["ctrl+b"] = "treeview:toggle-focus" }
