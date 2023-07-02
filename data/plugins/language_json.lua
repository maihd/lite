local syntax = require "core.syntax"

syntax.add {
    files = { "%.json$", "%.cson$" },
    comment = "//",
    patterns = {
        { pattern = "//.-\n",               type = "comment"  },
        { pattern = { "/%*", "%*/" },       type = "comment"  },
        { pattern = { '"', '"', '\\' },     type = "string"   },
        { pattern = { "'", "'", '\\' },     type = "string"   },
        { pattern = { "`", "`", '\\' },     type = "string"   },
        { pattern = "0x[%da-fA-F]+",        type = "number"   },
        { pattern = "-?%d+[%d%.eE]*",       type = "number"   },
        { pattern = "-?%.?%d+",             type = "number"   },
        { pattern = "[:]",                  type = "operator" },
        { pattern = "[%a_][%w_]*",          type = "symbol"   },
    },
    symbols = {
        ["true"]       = "literal",
        ["false"]      = "literal",
        ["null"]       = "literal",
    },
}
