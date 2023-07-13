local syntax = require "core.syntax"

syntax.add {
    name = "License",
    files = { "%LICENSE$", "%LICENSE.txt$", "%LICENSE.md$" },
    patterns = {
        --     { pattern = "\\.",                    type = "normal"   },
        --     { pattern = { "<!%-%-", "%-%->" },    type = "comment"  },
        --     { pattern = { "```", "```" },         type = "string"   },
        --     { pattern = { "``", "``", "\\" },     type = "string"   },
        --     { pattern = { "`", "`", "\\" },       type = "string"   },
        --     { pattern = { "~~", "~~", "\\" },     type = "keyword2" },
        --     { pattern = "%-%-%-+",                type = "comment" },
        --     { pattern = "%*%s+",                  type = "operator" },
        --     { pattern = { "%*", "[%*\n]", "\\" }, type = "operator" },
        --     { pattern = { "%_", "[%_\n]", "\\" }, type = "keyword2" },
        --     { pattern = "#.-\n",                  type = "keyword"  },
        --     { pattern = "!?%[.-%]%(.-%)",         type = "function" },
        { pattern = "^Copyright", type = "function" },
    },
    symbols = {},
}
