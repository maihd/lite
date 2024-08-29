local config = {}

config.project_scan_rate = 1.0 -- seconds
config.fps = 60
config.max_log_items = 80
config.message_timeout = 3
config.mouse_wheel_scroll = 100 * SCALE
config.file_size_limit = 10
config.ignore_files = "^%.git$" -- "^%."
config.symbol_pattern = "[%a_][%w_]*"
config.non_word_chars = " \t\n/\\()\"':,.;<>~!@#$%^&*|+=[]{}`?-"
config.undo_merge_timeout = 0.3
config.max_undos = 10000
config.highlight_current_line = true
config.line_height = 1.2
config.indent_size = 4
config.tab_type = "soft"
config.line_limit = 80
config.draw_space_dot = false -- for testing
config.scope_highlight = true

-- MaiHD customs

config.auto_indent              = true
config.indent_newline           = false
config.prevent_open_binary_file = true

config.window_opacity           = 1.0

return config
