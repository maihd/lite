local common = require "core.common"


local syntax = {}
syntax.items = {}


local plain_text_syntax = { patterns = {}, symbols = {} }


local function find_by_name(name)
    for i = #syntax.items, 1, -1 do
        local t = syntax.items[i]
        if t.name == name then
            return t, i
        end
    end
end


function syntax.add(t)
    if not t then
        return
    end

    -- Support reload syntax
    local old_syntax, idx = find_by_name(t.name)
    if old_syntax then
        syntax.items[idx] = t

        -- Do reload syntax for all open docs
        local core = require("core")
        for _, doc in ipairs(core.docs) do
            if doc.syntax == old_syntax then
                doc:reset_syntax()
            end
        end
    end

    -- Simply insert
    table.insert(syntax.items, t)
end


local function find(string, field)
    for i = #syntax.items, 1, -1 do
        local t = syntax.items[i]
        if common.match_pattern(string, t[field] or {}) then
            return t
        end
    end
end


function syntax.get(filename, header)
    return find(filename, "files")
        or find(header, "headers")
        or plain_text_syntax
end


return syntax
