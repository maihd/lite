local syntax = require "core.syntax"

syntax.add {
    name = "CMake",
    files = { "CMakeLists.txt$", "%.cmake$" },
    patterns = {
        { pattern = { "#", "\n" },            type = "comment"  },
        { pattern = { "${", "}" },            type = "keyword"  },
        { pattern = "[%a_][%w_]*%s*%f[(]",    type = "keyword"  },
        { pattern = "[%a_][%w_]*",            type = "symbol"   },
    },
    symbols = {
        ["OR"] = "keyword",
        ["AND"] = "keyword",
        ["NOT"] = "keyword",
    },
}
