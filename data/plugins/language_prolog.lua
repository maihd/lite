-- language_prolog.lua

local syntax = require "core.syntax"

syntax.add {
    files = { "%.prolog$" }, -- Matches .prolog files
    comment = "%", -- Line comment character

    patterns = {
        -- Comments
        { pattern = "%%.*", type = "comment" },

        -- Strings (double-quoted)
        { pattern = { '"', '"', '\\' }, type = "string" },

        -- Quoted atoms (single-quoted)
        { pattern = { "'", "'", '\\' }, type = "string" },

        -- Numbers (integers and floats, with optional sign)
        { pattern = "%-?%d+%.?%d*", type = "number" },

        -- Variables (start with uppercase or underscore)
        { pattern = "[_%u][%w_]*", type = "keyword2" },

        -- Function/predicate calls (atom followed by an open parenthesis)
        { pattern = "[%l][%w_]*%f[(]", type = "function" },

        -- Atoms (start with lowercase)
        { pattern = "[%l][%w_]*", type = "symbol" },

        -- Operators (basic set)
        { pattern = "[:%-+*/=<>&|!;,.%[%]%(%){}]", type = "operator" },
    },

    -- Symbols are used for keywords and literals
    symbols = {
        -- Core keywords
        ["is"]          = "keyword",
        ["mod"]         = "keyword",
        ["div"]         = "keyword",
        ["true"]        = "literal",
        ["false"]       = "literal",
        ["fail"]        = "literal",
        ["!"]           = "keyword", -- Cut operator

        -- Common built-in predicates (optional)
        ["var"]         = "keyword2",
        ["nonvar"]      = "keyword2",
        ["integer"]     = "keyword2",
        ["float"]       = "keyword2",
        ["atom"]        = "keyword2",
        ["atomic"]      = "keyword2",
        ["compound"]    = "keyword2",
        ["callable"]    = "keyword2",
        ["is_list"]     = "keyword2",
        ["append"]      = "function",
        ["member"]      = "function",
        ["length"]      = "function",
        ["reverse"]     = "function",
        ["findall"]     = "function",
        ["bagof"]       = "function",
        ["setof"]       = "function",
        ["assert"]      = "function",
        ["retract"]     = "function",
        ["call"]        = "function",
        ["once"]        = "function",
        ["catch"]       = "function",
        ["throw"]       = "function",
        ["read"]        = "function",
        ["write"]       = "function",
        ["nl"]          = "function",
        ["format"]      = "function",
    },
}
