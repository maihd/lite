local syntax = require "core.syntax"

syntax.add {
    name = "4coder",
    files = { "%.4coder$" },
    comment = "//",
    patterns = {
        { pattern = "//.-\n",               type = "comment"  },
        { pattern = { "/%*", "%*/" },       type = "comment"  },
        { pattern = { '"', '"', '\\' },     type = "string"   },
        { pattern = { "'", "'", '\\' },     type = "string"   },
        { pattern = "-?0x%x+",              type = "number"   },
        { pattern = "-?%d+[%d%.eE]*f?",     type = "number"   },
        { pattern = "-?%.?%d+f?",           type = "number"   },
        { pattern = "\\\n",                 type = "keyword2" },
        { pattern = "[%+%-=/%*%^%%<>!~|&]", type = "operator" },
        { pattern = "[%a_][%w_]*[ ]*=",     type = "function" },
        { pattern = "[%a_][%w_]*",          type = "symbol"   },
    },
    symbols = {
        -- @note: does we need atomic_t highlights?

        -- C11 literal
        ["true"]     = "literal",
        ["false"]    = "literal",
    },
}

