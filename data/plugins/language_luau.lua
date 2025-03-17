local syntax = require "core.syntax"

syntax.add {
    name = "Luau",
    files = "%.luau$",
    headers = "^#!.*[ /]luau",
    comment = "--",
    scope_begin = {
        "do", "if", "repeat", "function", "{",
    },
    scope_end = {
        "end", "until", "}"
    },
    scope_align = {
        ["else"] = -1,
        ["elseif"] = -1
    },
    patterns = {
        { pattern = { '"', '"', '\\' },       type = "string"   },
        { pattern = { "'", "'", '\\' },       type = "string"   },
        { pattern = { "%[%[", "%]%]" },       type = "string"   },
        { pattern = { "%-%-%[%[", "%]%]"},    type = "comment"  },
        { pattern = "%-%-.-\n",               type = "comment"  },
        { pattern = "-?0x%x+",                type = "number"   },
        { pattern = "-?%d+[%d%.eE]*",         type = "number"   },
        { pattern = "-?%.?%d+",               type = "number"   },
        { pattern = "<%a+>",                  type = "keyword2" },
        { pattern = "%.%.%.?",                type = "operator" },
        { pattern = "[<>~=]=",                type = "operator" },
        { pattern = "[%+%-=/%*%^%%#<>:]",     type = "operator" }, -- @note(maihd): Add ':' to highlight type declaration
        { pattern = "[%a_][%w_]*%s*%f[(\"{]", type = "function" },
        { pattern = "[%a_][%w_]*",            type = "symbol"   },
        { pattern = "::[%a_][%w_]*::",        type = "function" }, -- @todo(maihd): label highlight
    },
    symbols = {
        ["if"]       = "keyword",
        ["then"]     = "keyword",
        ["else"]     = "keyword",
        ["elseif"]   = "keyword",
        ["end"]      = "keyword",
        ["do"]       = "keyword",
        ["function"] = "keyword",
        ["repeat"]   = "keyword",
        ["until"]    = "keyword",
        ["while"]    = "keyword",
        ["for"]      = "keyword",
        ["break"]    = "keyword",
        ["return"]   = "keyword",
        ["local"]    = "keyword",
        ["in"]       = "keyword",
        ["not"]      = "keyword",
        ["and"]      = "keyword",
        ["or"]       = "keyword",
        ["goto"]     = "keyword",
        ["type"]     = "keyword",
        ["continue"] = "keyword",
        ["self"]     = "keyword2",
        ["true"]     = "literal",
        ["false"]    = "literal",
        ["nil"]      = "literal",
    },
}

