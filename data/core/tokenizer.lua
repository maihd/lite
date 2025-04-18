local tokenizer = {}


local function trim(s)
    return string.gsub(s, "^%s*(.-)%s*$", "%1")
end


local function push_token(t, type, text)
    local prev_type = t[#t-1]
    local prev_text = t[#t]
    if prev_type and (prev_type == type or prev_text:find("^%s*$")) then
        t[#t-1] = type
        t[#t] = prev_text .. text
    else
        table.insert(t, type)
        table.insert(t, text)
    end
end


local function is_escaped(text, idx, esc)
    local byte = esc:byte()
    local count = 0
    for i = idx - 1, 1, -1 do
        if text:byte(i) ~= byte then break end
        count = count + 1
    end
    return count % 2 == 1
end


local function find_non_escaped(text, pattern, offset, esc)
    while true do
        local s, e = text:find(pattern, offset)
        if not s then
            break
        end

        if esc and is_escaped(text, s, esc) then
            offset = e + 1
        else
            return s, e
        end
    end
end


function tokenizer.tokenize(syntax, text, state)
    local res = {}
    local begin_scopes, end_scopes, scope_align = 0, 0, 0
    local begin_scope_token, end_scope_token = nil, nil
    local i = 1

    if #syntax.patterns == 0 then
        return { "normal", text }, nil, begin_scopes, end_scopes, scope_align
    end

    while i <= #text do
        -- continue trying to match the end pattern of a pair if we have a state set
        if state then
            local p = syntax.patterns[state]
            local s, e = find_non_escaped(text, p.pattern[2], i, p.pattern[3])

            if s then
                push_token(res, p.type, text:sub(i, e))
                state = nil
                i = e + 1
            else
                push_token(res, p.type, text:sub(i))
                break
            end
        end

        -- find matching pattern
        local matched = false
        for n, p in ipairs(syntax.patterns) do
            local pattern = (type(p.pattern) == "table") and p.pattern[1] or p.pattern
            local s, e = text:find("^" .. pattern, i)

            if s then
                -- matched pattern; make and add token
                local t = text:sub(s, e)
                push_token(res, syntax.symbols[trim(t)] or p.type, t)

                -- update state if this was a start|end pattern pair
                if type(p.pattern) == "table" then
                    state = n
                end

                -- move cursor past this token
                i = e + 1
                matched = true

                -- match scope
                t = trim(t)
                if type(syntax.scope_begin) == "table" and type(syntax.scope_end) == "table" then
                    for _, x in pairs(syntax.scope_begin) do
                        if x == t then
                            begin_scopes = begin_scopes + 1
                            begin_scope_token = x
                            break
                        end
                    end

                    for _, x in pairs(syntax.scope_end) do
                        if x == t then
                            end_scopes = end_scopes + 1
                            end_scope_token = x
                            break
                        end
                    end
                end

                -- match scope align
                if type(syntax.scope_align) == "table" then
                    local align = syntax.scope_align[t]
                    if align then
                        scope_align = scope_align + align
                    end
                end

                break
            end
        end

        -- consume character if we didn't match
        if not matched then
            local t = text:sub(i, i)
            push_token(res, "normal", t)
            i = i + 1

            -- match scope
            if type(syntax.scope_begin) == "table" and type(syntax.scope_end) == "table" then
                for _, x in pairs(syntax.scope_begin) do
                    if x == t then
                        begin_scopes = begin_scopes + 1
                        begin_scope_token = x
                        break
                    end
                end

                for _, x in pairs(syntax.scope_end) do
                    if x == t then
                        end_scopes = end_scopes + 1
                        end_scope_token = x
                        break
                    end
                end
            end
        end
    end

    return res, state, begin_scopes, end_scopes, scope_align, begin_scope_token, end_scope_token
end


local function iter(t, i)
    i = i + 2
    local type, text = t[i], t[i+1]
    if type then
        return i, type, text
    end
end

function tokenizer.each_token(t)
    return iter, t, -1
end


return tokenizer
