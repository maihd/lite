local syntax = require "core.syntax"

syntax.add {
    name = "C",
    files = { "%.c$", "%.h$", "%.inl$", "%.cpp$", "%.hpp$" },
    comment = "//",
    scope_begin = { "{" },
    scope_end = { "}" },
    patterns = {
        { pattern = "//.-\n",               type = "comment"  },
        { pattern = { "/%*", "%*/" },       type = "comment"  },
        { pattern = { "#", "[ \n]" },       type = "keyword2" },
        { pattern = { '"', '"', '\\' },     type = "string"   },
        { pattern = { "'", "'", '\\' },     type = "string"   },
--         { pattern = "<[[%a_][%w_]*.\\]*>",  type = "string"   }, -- @note(maihd): trick to highlight include path
        { pattern = "-?0x%x+",              type = "number"   },
        { pattern = "-?%d+[%d%.eE]*f?",     type = "number"   },
        { pattern = "-?%.?%d+f?",           type = "number"   },
        { pattern = "\\\n",                 type = "keyword2" }, -- @note(maihd): end line of macros
        { pattern = "[%+%-=/%*%^%%<>!~|&]", type = "operator" },
        { pattern = "[%a_][%w_]*%f[(]",     type = "function" },
        { pattern = "[%a_][%w_]*",          type = "symbol"   },
        -- @todo(maihd): add label highlight
    },
    symbols = {
        -- Basic keywords
        ["if"]       = "keyword",
        ["else"]     = "keyword",
        ["do"]       = "keyword",
        ["while"]    = "keyword",
        ["for"]      = "keyword",
        ["break"]    = "keyword",
        ["continue"] = "keyword",
        ["return"]   = "keyword",
        ["goto"]     = "keyword",
        ["struct"]   = "keyword",
        ["union"]    = "keyword",
        ["typedef"]  = "keyword",
        ["enum"]     = "keyword",
        ["extern"]   = "keyword",
        ["static"]   = "keyword",
        ["volatile"] = "keyword",
        ["inline"]   = "keyword",
        ["switch"]   = "keyword",
        ["case"]     = "keyword",
        ["default"]  = "keyword",
        ["auto"]     = "keyword",
        ["const"]    = "keyword",
        ["sizeof"]   = "keyword",

        -- C11 keywords
        ["alignas"] = "keyword",
        ["alignof"] = "keyword",
        ["constexpr"] = "keyword",
        ["thread_local"] = "keyword",
        ["static_assert"] = "keyword",

        ["_Bool"] = "keyword",
        ["_Alignas"] = "keyword",
        ["_Alignof"] = "keyword",
        ["_Thread_local"] = "keyword",
        ["_Static_assert"] = "keyword",

        -- Custom keywords
        ["__comptime"] = "keyword",
        ["__forceinline"] = "keyword",

        -- Compiler extensions keywords
        ["__declspec"] = "keyword",
        ["__attribute__"] = "keyword",

        -- Basic types
        ["void"]     = "keyword",
        ["int"]      = "keyword2",
        ["short"]    = "keyword2",
        ["long"]     = "keyword2",
        ["float"]    = "keyword2",
        ["double"]   = "keyword2",
        ["char"]     = "keyword2",
        ["signed"]   = "keyword2",
        ["unsigned"] = "keyword2",
        ["bool"]     = "keyword2",

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

        -- @note: does we need atomic_t highlights?

        -- C11 literal
        ["true"]     = "literal",
        ["false"]    = "literal",
        ["NULL"]     = "literal",
        ["nullptr"]  = "literal",
    },
}

