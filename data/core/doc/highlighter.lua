local core = require "core"
local config = require "core.config"
local tokenizer = require "core.tokenizer"
local Object = require "core.object"


local Highlighter = Object:extend()


function Highlighter:new(doc)
    self.doc = doc
    self:reset()

    -- init incremental syntax highlighting
    core.add_thread(function()
        while true do
            if self.first_invalid_line > self.max_wanted_line then
--                 self.max_wanted_line = 0
                coroutine.yield(1 / config.fps)
            else
                self:update()
                core.redraw = true
                coroutine.yield()
            end
        end
    end, self)
end


function Highlighter:reset()
    self.lines = {}
    self.first_invalid_line = 1
    self.max_wanted_line = #self.doc.lines

    self.scope_nest = 0
    self.scopes = {}
end


function Highlighter:invalidate(idx)
    self.first_invalid_line = 1
--     math.min(self.first_invalid_line, idx)
    self.max_wanted_line = #self.doc.lines
--     math.min(self.max_wanted_line, #self.doc.lines)
end


function Highlighter:update()
    local max = math.min(self.first_invalid_line + 40, self.max_wanted_line)
--     local max = self.max_wanted_line

    for i = self.first_invalid_line, max do
        local state = (i > 1) and self.lines[i - 1].state
        local line = self.lines[i]
        if not (line and line.init_state == state) then
            self.lines[i] = self:tokenize_line(i, state)
        end
    end

    self.first_invalid_line = max + 1
end


function Highlighter:tokenize_line(idx, state)
    local res = {}
    res.init_state = state
    res.text = self.doc.lines[idx]
    res.tokens, res.state, res.begin_scopes, res.end_scopes = tokenizer.tokenize(self.doc.syntax, res.text, state)

    if res.begin_scopes > 0 and res.begin_scopes > res.end_scopes then
        table.insert(self.scopes, { begin_line = idx, end_line = idx, nest = self.scope_nest })
        self.scope_nest = self.scope_nest + 1
    elseif res.end_scopes > 0 and res.end_scopes > res.begin_scopes then
        self.scope_nest = self.scope_nest - 1

        for i = #self.scopes, 1, -1 do
            local scope = self.scopes[i]
            if scope.nest == self.scope_nest then
                scope.end_line = idx
                break
            end
        end
    end

    return res
end


function Highlighter:get_line(idx)
    local line = self.lines[idx]
    if not line or line.text ~= self.doc.lines[idx] then
        local prev = self.lines[idx - 1]
        line = self:tokenize_line(idx, prev and prev.state)
        self.lines[idx] = line
    end
    self.max_wanted_line = math.max(self.max_wanted_line, idx)
    return line
end


function Highlighter:each_token(idx)
    local max_wanted_line = self.max_wanted_line
    self.max_wanted_line = idx
    self:update()
    self.max_wanted_line = self.max_wanted_line

    return tokenizer.each_token(self:get_line(idx).tokens)
end


return Highlighter
