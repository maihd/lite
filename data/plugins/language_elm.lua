local syntax = require("core.syntax")

syntax.add {
    name = "Elm",
    files = "%.elm$",
    comment = "--",
    indent_size = 2,
    patterns = {
        { pattern = { "\"", "\"", "\\" },   type = "string"     },
        { pattern = "0x[%da-fA-F]+",        type = "number"     },
        { pattern = "-?%d+[%d%.eE]*",       type = "number"     },
        { pattern = "-?%.?%d+",             type = "number"     },
        { pattern = "%-%-.-\n",             type = "comment"    },
        { pattern = "[%+%-=/%*%^%%<>!~|&]", type = "operator"   },
        -- { pattern = "[%a_][%w_]*%s+[=%f]",  type = "function"   },
        { pattern = "[%a_][%w_]*",          type = "symbol"     },
    },
    symbols = {
        ["module"]      = "keyword",
        ["exposing"]    = "keyword",
        ["import"]      = "keyword",
        ["case"]        = "keyword",
        ["type"]        = "keyword",
        ["of"]          = "keyword",
        ["as"]          = "keyword",
        ["if"]          = "keyword",
        ["then"]        = "keyword",
        ["else"]        = "keyword",
        ["alias"]       = "keyword",
        ["let"]         = "keyword",
        ["in"]          = "keyword",

        ["True"]        = "literal",
        ["False"]       = "literal",
    }
}
