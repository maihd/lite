local syntax = require "core.syntax"

syntax.add {
    name = "Odin",
    files = { "%.odin$" },
    comment = "//",
    scope_begin = { "{" },
    scope_end = { "}" },
    patterns = {
        { pattern = "//.-\n",                               type = "comment"  },
        { pattern = { "/%*", "%*/" },                       type = "comment"  },
        { pattern = { "#", "[ \n]" },                       type = "keyword2" },
        { pattern = { "@", "[ \n]" },                       type = "keyword2" },
        { pattern = { "@%f[(]", "[ \n]" },                  type = "keyword2" },
        { pattern = { "\\", "[ \n]" },                      type = "keyword2" },
        { pattern = { '"', '"', '\\' },                     type = "string"   },
        { pattern = { "'", "'", '\\' },                     type = "string"   },
        { pattern = { "`", "`", '\\' },                     type = "string"   },
        { pattern = "[+-]?0b[%x_]+",                        type = "number"   },
        { pattern = "[+-]?0o[%x_]+",                        type = "number"   },
        { pattern = "[+-]?0x[%x_]+",                        type = "number"   },
        { pattern = "[+-]?%d+[%d_]+[%d%.eE_]*",             type = "number"   },
        { pattern = "[+-]?%.?%d+",                          type = "number"   },
        { pattern = "[%+%-=/%*%^%%<>!~|&:]",                type = "operator" },
        { pattern = "[%a_][%w_]*%f[(]",                     type = "function" },
        -- { pattern = "([%w_]+)%s*::%s*%f[%w]proc%s*%(",              type = "function" },
        { pattern = "[%a_][%w_]*",                          type = "symbol"   },
    },
    symbols = {
        -- Basic keywords
        ["proc"]        = "keyword",
        ["defer"]       = "keyword",
        ["if"]          = "keyword",
        ["else"]        = "keyword",
        ["do"]          = "keyword",
        ["for"]         = "keyword",
        ["when"]        = "keyword",
        ["where"]       = "keyword",
        ["break"]       = "keyword",
        ["continue"]    = "keyword",
        ["fallthrough"] = "keyword",
        ["or_else"]     = "keyword",
        ["or_break"]    = "keyword",
        ["or_continue"] = "keyword",
        ["or_return"]   = "keyword",
        ["return"]      = "keyword",
        ["struct"]      = "keyword",
        ["union"]       = "keyword",
        ["enum"]        = "keyword",
        ["bit_set"]     = "keyword",
        ["bit_field"]   = "keyword",
        ["foreign"]     = "keyword",
        ["import"]      = "keyword",
        ["switch"]      = "keyword",
        ["case"]        = "keyword",
        ["cast"]        = "keyword",
        ["transmute"]   = "keyword",
        ["auto_cast"]   = "keyword",
        ["using"]       = "keyword",
        ["package"]     = "keyword",
        ["distinct"]    = "keyword",

        ["map"]         = "keyword",
        ["dynamic"]     = "keyword",
        ["matrix"]      = "keyword",

        ["context"]     = "keyword",

        ["in"]          = "keyword",
        ["not_in"]      = "keyword",

        ["typeid"]      = "keyword",

        ["asm"]         = "keyword",

        -- Primitive types
        ["any"]         = "keyword2",
        ["byte"]        = "keyword2",
        ["rawptr"]      = "keyword2",

        ["f16"]         = "keyword2",
        ["f32"]         = "keyword2",
        ["f64"]         = "keyword2",

        ["f16be"]       = "keyword2",
        ["f32be"]       = "keyword2",
        ["f64be"]       = "keyword2",

        ["f16le"]       = "keyword2",
        ["f32le"]       = "keyword2",
        ["f64le"]       = "keyword2",

        ["rune"]        = "keyword2",
        ["string"]      = "keyword2",
        ["string16"]    = "keyword2",
        ["cstring"]     = "keyword2",
        ["cstring16"]   = "keyword2",

        ["bool"]        = "keyword2",
        ["b8"]          = "keyword2",
        ["b16"]         = "keyword2",
        ["b32"]         = "keyword2",

        ["int"]         = "keyword2",
        ["uint"]        = "keyword2",
        ["uintptr"]     = "keyword2",

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

        ["i16be"]       = "keyword2",
        ["i32be"]       = "keyword2",
        ["i64be"]       = "keyword2",
        ["i128be"]      = "keyword2",

        ["u8be"]        = "keyword2",
        ["u16be"]       = "keyword2",
        ["u32be"]       = "keyword2",
        ["u64be"]       = "keyword2",
        ["u128be"]      = "keyword2",

        ["i16le"]       = "keyword2",
        ["i32le"]       = "keyword2",
        ["i64le"]       = "keyword2",
        ["i128le"]      = "keyword2",

        ["u8le"]        = "keyword2",
        ["u16le"]       = "keyword2",
        ["u32le"]       = "keyword2",
        ["u64le"]       = "keyword2",
        ["u128le"]      = "keyword2",

        ["complex32"]   = "keyword2",
        ["complex64"]   = "keyword2",
        ["complex128"]  = "keyword2",

        ["quaternion64"]    = "keyword2",
        ["quaternion128"]   = "keyword2",
        ["quaternion256"]   = "keyword2",

        ["Maybe"]           = "keyword2",
        ["Objc_Block"]      = "keyword2",

        -- Literal, a.k.a untyped values

        ["true"]        = "literal",
        ["false"]       = "literal",
        ["nil"]         = "literal",
    },
}

