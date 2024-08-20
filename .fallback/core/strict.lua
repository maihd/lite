local strict = {}
strict.defined = {}

-- Polyfill
if type(pack) == "function" then
    table.pack = pack
end

if type(unpack) == "function" then
    table.unpack = unpack
end

-- used to define a global variable
function global(t)
    for k, v in pairs(t) do
        strict.defined[k] = true
        rawset(_G, k, v)
    end
end


function strict.__newindex(t, k, v)
    error("cannot set undefined variable: " .. k, 2)
end


function strict.__index(t, k)
    if not strict.defined[k] then
        error("cannot get undefined variable: " .. k, 2)
    end
end


setmetatable(_G, strict)
