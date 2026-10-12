local syntax = require "core.syntax"

syntax.add {
    name = "Madness C++",
    files = { "%.mcx$", "%.cpp$", "%.cc$", "%.cxx$", ".hh", ".hpp", }, -- No header file, should only use C API in library
    comment = "//",
    scope_begin = { "{" },
    scope_end = { "}" },
    patterns = {
        { pattern = "//.-\n",               type = "comment"  },
        { pattern = { "/%*", "%*/" },       type = "comment"  },
        { pattern = { "#", "[ \n]" },       type = "keyword2" },
        { pattern = { '"', '"', '\\' },     type = "string"   },
        { pattern = { "'", "'", '\\' },     type = "string"   },
--         { pattern = "#include <[[%a_][%w_]*.\\]*>",  type = "string"   }, -- @note(maihd): trick to highlight include path
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

        -- Basic types
        ["void"]        = "keyword",
        ["int"]         = "keyword2",
        ["short"]       = "keyword2",
        ["long"]        = "keyword2",
        ["float"]       = "keyword2",
        ["double"]      = "keyword2",
        ["char"]        = "keyword2",
        ["signed"]      = "keyword2",
        ["unsigned"]    = "keyword2",
        ["bool"]        = "keyword2",

        -- @region Size integer types

        ["int8_t"]      = "keyword2",
        ["int16_t"]     = "keyword2",
        ["int32_t"]     = "keyword2",
        ["int64_t"]     = "keyword2",
        ["uint8_t"]     = "keyword2",
        ["uint16_t"]    = "keyword2",
        ["uint32_t"]    = "keyword2",
        ["uint64_t"]    = "keyword2",
        ["size_t"]      = "keyword2",
        ["intptr_t"]    = "keyword2",
        ["uintptr_t"]   = "keyword2",
        ["__int128"]    = "keyword2",
        ["__uint128"]   = "keyword2",

        ["true"]        = "literal",
        ["false"]       = "literal",
        ["nullptr"]     = "literal",

        -- @region primitive type based on Odin (isize, usize is specical case)

        ["i8"]          = "keyword2",
        ["i16"]         = "keyword2",
        ["i32"]         = "keyword2",
        ["i64"]         = "keyword2",
        ["i128"]        = "keyword2",
        ["u8"]          = "keyword2",
        ["u16"]         = "keyword2",
        ["u32"]         = "keyword2",
        ["u64"]         = "keyword2",
        ["u128"]        = "keyword2",
        ["isize"]       = "keyword2",
        ["usize"]       = "keyword2",
        ["uintptr"]     = "keyword2",

        ["f32"]         = "keyword2",
        ["f64"]         = "keyword2",

        ["fx32"]        = "keyword2",
        ["fx64"]        = "keyword2",

        -- @region keywords can use

        ["constexpr"]   = "keyword",
        ["consteval"]   = "keyword",
        ["namespace"]   = "keyword",
        ["using"]       = "keyword",
        ["template"]    = "keyword",
        ["typename"]    = "keyword",
        ["operator"]    = "keyword",
        ["alignas"]     = "keyword",
        ["alignof"]     = "keyword",

        -- @region banned keywords

        ["class"]       = "keyword3",
        ["virtual"]     = "keyword3",
        ["override"]    = "keyword3",
        ["final"]       = "keyword3",
        ["friend"]      = "keyword3",
        ["public"]      = "keyword3",
        ["private"]     = "keyword3",
        ["protected"]   = "keyword3",
        ["throw"]       = "keyword3",
        ["try"]         = "keyword3",
        ["catch"]       = "keyword3",
        ["noexcept"]    = "keyword3",
        ["concept"]     = "keyword3",
        ["requires"]    = "keyword3",
        ["typeid"]      = "keyword3",
        ["dynamic_cast"]= "keyword3",
        ["import"]      = "keyword3",
        ["export"]      = "keyword3",
        ["module"]      = "keyword3",

        -- Custom keywords
        ["__comptime"]      = "keyword",
        ["__forceinline"]   = "keyword",

        -- Compiler extensions keywords
        ["__declspec"]      = "keyword",
        ["__attribute__"]   = "keyword",
    },
}

