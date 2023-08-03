local syntax = require "core.syntax"

syntax.add {
    name = "ActionScript3",
    files = { "%.as$" },
    comment = "//",
    scope_begin = { "{" },
    scope_end = { "}" },
    patterns = {
        { pattern = "//.-\n",               type = "comment"  },
        { pattern = { "/%*", "%*/" },       type = "comment"  },
--         { pattern = { "#", "[ \n]" },       type = "keyword2" },
        { pattern = { '"', '"', '\\' },     type = "string"   },
        { pattern = { "'", "'", '\\' },     type = "string"   },
--         { pattern = "<[[%a_][%w_]*.\\]*>",  type = "string"   }, -- @note(maihd): trick to highlight include path
        { pattern = "-?0x%x+",              type = "number"   },
        { pattern = "-?%d+[%d%.eE]*f?",     type = "number"   },
        { pattern = "-?%.?%d+f?",           type = "number"   },
--         { pattern = "\\\n",                 type = "keyword2" }, -- @note(maihd): end line of macros
        { pattern = "[%+%-=/%*%^%%<>!~|&]", type = "operator" },
        { pattern = "[%a_][%w_]*%f[(]",     type = "function" },
        { pattern = "[%a_][%w_]*",          type = "symbol"   },
        -- @todo(maihd): add label highlight
    },
    symbols = {
        -- Basic keywords
        ["if"]          = "keyword",
        ["else"]        = "keyword",
        ["do"]          = "keyword",
        ["while"]       = "keyword",
        ["for"]         = "keyword",
        ["break"]       = "keyword",
        ["continue"]    = "keyword",
        ["return"]      = "keyword",
        ["goto"]        = "keyword",
        ["class"]       = "keyword",
        ["extends"]     = "keyword",
        ["static"]      = "keyword",
        ["switch"]      = "keyword",
        ["case"]        = "keyword",
        ["default"]     = "keyword",
        ["const"]       = "keyword",
        ["public"]      = "keyword",
        ["private"]     = "keyword",
        ["import"]      = "keyword",
        ["package"]     = "keyword",
        ["function"]    = "keyword",
        ["as"]          = "keyword",
        ["new"]         = "keyword",
        ["var"]         = "keyword",
        ["get"]         = "keyword",
        ["set"]         = "keyword",

        ["try"]         = "keyword",
        ["catch"]       = "keyword",
        ["finally"]     = "keyword",

        ["null"]        = "literal",
        ["true"]        = "literal",
        ["false"]       = "literal",

        -- Basic types
        ["void"]     = "keyword2",
        ["int"]      = "keyword2",
        ["uint"]     = "keyword2",
        ["Array"]    = "keyword2",
        ["Object"]   = "keyword2",
        ["String"]   = "keyword2",
        ["Number"]   = "keyword2",
        ["Boolean"]  = "keyword2",

        ["Event"]    = "keyword2",
        ["Error"]    = "keyword2",

        -- Size integer types
        ["int8_t"]   = "keyword2",
        ["int16_t"]  = "keyword2",
        ["int32_t"]  = "keyword2",
        ["int64_t"]  = "keyword2",
        ["uint8_t"]  = "keyword2",
        ["uint16_t"] = "keyword2",
        ["uint32_t"] = "keyword2",
        ["uint64_t"] = "keyword2",
        ["size_t"]   = "keyword2",
        ["intptr_t"] = "keyword2",
        ["uintptr_t"] = "keyword2",
    },
}
