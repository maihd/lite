local syntax = require "core.syntax"

syntax.add {
    name = "Batch",
    version = "v0.0.4",
    files = "%.bat$",
    headers = "^#!.*[ /]batch",
    comment = "::",
    patterns = {
        { pattern = { '"', '"', '\\'           },       type = "string"   },
        { pattern = { "'", "'", '\\'           },       type = "string"   },
        { pattern = ":[%a_][%w_]*",                     type = "function" }, -- @note(maihd): label
        { pattern = { "::", "\n"               },       type = "comment"  },
        { pattern = { ": ", "\n"               },       type = "comment"  },
        { pattern = { "@?[rR][eE][mM] ", "\n"   },      type = "comment"  },
        { pattern = " -?0x%x+ ",                        type = "number"   }, -- @note(maihd): not work
        { pattern = " -?%d+[%d%.eE]* ",                 type = "number"   }, -- @note(maihd): not work
        { pattern = " -?%.?%d+ ",                       type = "number"   }, -- @note(maihd): not work
        { pattern = "[%a_][%w_]*",                      type = "symbol"   },
        { pattern = "%%[%a_][%w_]*%%",                  type = "function" }, -- @note(maihd): variable
        { pattern = "%%%%[%a_][%w_]*",                  type = "function" }, -- @note(maihd): parameter
        { pattern = "%/[[%a_][%w_]*?]",                 type = "operator" }, -- @note(maihd): cmdline argument, start with '/'
        { pattern = "%^",                               type = "keyword2" },
        { pattern = "%@",                               type = "keyword2" },
    },
    symbols = {
        ["set"]         = "keyword",
        ["setx"]        = "keyword",
        ["del"]         = "keyword",
        ["copy"]        = "keyword",
        ["xcopy"]       = "keyword",
        ["if"]          = "keyword",
        ["else"]        = "keyword",
        ["start"]       = "keyword",
        ["mkdir"]       = "keyword",
        ["rmdir"]       = "keyword",
        ["rem"]         = "keyword",
        ["echo"]        = "keyword",
        ["goto"]        = "keyword",
        ["call"]        = "keyword",
        ["not"]         = "keyword2",
        ["exist"]       = "keyword2",
        ["for"]         = "keyword",
        ["in"]          = "keyword2",
        ["do"]          = "keyword2",
        ["pause"]       = "keyword",
        ["exit"]        = "keyword",
    },
}

