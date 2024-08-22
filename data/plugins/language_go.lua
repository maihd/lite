local syntax = require("core.syntax")

syntax.add {
    name = "Go",
    files = "%.go$",
    headers = "^#!.*[ /]go",
    version = "0.0.1-dev"
    comment = "//",
    scope_begin = { "{" },
    scope_end = { "}" },
    scope_align = { "}" },
    patterns = {
        { pattern = { "\"", "\"", "\\" },   type = "string"     },
        { pattern = "[%a_][%w]*",           type = "symbol"     },
        { pattern = "[%a_][%w]*%s*%f(",     type = "function"   },
    },
    symbols = {
        ["func"]    = "keyword",
        ["import"]  = "keyword",
        ["package"] = "keyword",
    }
}
