local syntax = require "core.syntax"

syntax.add {
    name = "XML",
    version = "v0.1.1",
    files = { "%.xml$", "%.html?$", "%.csproj", "%.vcxproj" },
    headers = "<%?xml",
    patterns = {
        { pattern = { "<!%-%-", "%-%->" },     type = "comment" },
        { pattern = { '%f[^>][^<]', '%f[<]' }, type = "normal" },
        { pattern = { '"', '"', '\\' },        type = "string" },
        { pattern = { "'", "'", '\\' },        type = "string" },
        { pattern = "0x[%da-fA-F]+",           type = "number" },
        { pattern = "-?%d+[%d%.]*f?",          type = "number" },
        { pattern = "-?%.?%d+f?",              type = "number" },
        { pattern = "%f[^<]![%a_][%w_]*",      type = "keyword2" },
        { pattern = "%f[^<][%a_][%w_]*",       type = "function" },
        { pattern = "%f[^<]/[%a_][%w_]*",      type = "function" },
        { pattern = "[%a_][%w_]*",             type = "keyword" },
        { pattern = "[/<>=]",                  type = "operator" },
    },
    symbols = {},
}
