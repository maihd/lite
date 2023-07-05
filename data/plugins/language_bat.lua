local syntax = require "core.syntax"

syntax.add {
    name = "Batch",
    files = "%.bat$",
    headers = "^#!.*[ /]batch",
    comment = "::",
    patterns = {
        { pattern = { '"', '"', '\\'            },      type = "string"   },
        { pattern = { "'", "'", '\\'            },      type = "string"   },
        { pattern = { "::", "\n"               },       type = "comment"  },
        { pattern = { "@?[rR][eE][mM]", "\n"   },       type = "comment"  },
        { pattern = " -?0x%x+ ",                        type = "number"   }, -- @note(maihd): not work
        { pattern = " -?%d+[%d%.eE]* ",                 type = "number"   }, -- @note(maihd): not work
        { pattern = " -?%.?%d+ ",                       type = "number"   }, -- @note(maihd): not work
        { pattern = "[%a_][%w_]*",                      type = "symbol"   },
        { pattern = "%^",                               type = "keyword2" },
        { pattern = "%@",                               type = "keyword2" },
    },
    symbols = {
        ["set"]         = "keyword",
        ["del"]         = "keyword",
        ["if"]          = "keyword",
        ["else"]        = "keyword",
        ["start"]       = "keyword",
        ["mkdir"]       = "keyword",
        ["rmdir"]       = "keyword",
        ["rmdir"]       = "keyword",
        ["rem"]         = "keyword",
        ["echo"]        = "keyword",
    },
}

