require "core.strict"

local common = require "core.common"
local config = require "core.config"
local style = require "core.style"
local command
local keymap
local RootView
local StatusView
local CommandView
local TitleBar
local Doc

local core = {}

local function project_scan_thread()
    local function diff_files(a, b)
        if #a ~= #b then return true end
        for i, v in ipairs(a) do
            if b[i].filename ~= v.filename
                or b[i].modified ~= v.modified then
                return true
            end
        end
    end

    local function compare_file(a, b)
        return a.filename < b.filename
    end

    local function get_files(path, t)
        coroutine.yield()

        t = t or {}
        local size_limit = config.file_size_limit * 10e5
        local all = system.list_dir(path) or {}
        local dirs, files = {}, {}

        for _, file in ipairs(all) do
            local file = (path ~= "." and path .. PATHSEP or "") .. file

            if not core.is_ignore(file) then
                local info = system.get_file_info(file)
                if info and info.size < size_limit then
                    info.filename = file
                    table.insert(info.type == "dir" and dirs or files, info)
                end
            end
        end

        table.sort(dirs, compare_file)
        for _, f in ipairs(dirs) do
            table.insert(t, f)
            get_files(f.filename, t)
        end

        table.sort(files, compare_file)
        for _, f in ipairs(files) do
            table.insert(t, f)
        end
        return t
    end

    --   local function is_project_changed(current_dir)
    --     current_dir = current_dir or core.project_dir
    --     local t = system.file_time(current_dir)
    --     return
    --   end

    while true do
        -- get project directory write time, replace previous if the directory is written
--         local t = system.file_time(core.project_dir)
--         if t > core.project_dir_time then
--             local project_files = get_files(".")
--             core.project_files = project_files
--             core.project_dir_time = t
--         else
        -- get project files and replace previous table if the new table is
        -- different
        local t = get_files(".")
        if diff_files(core.project_files, t) then
            core.project_files = t
            core.redraw = true
        end
--         end

        -- wait for next scan
        coroutine.yield(config.project_scan_rate)
    end
end


function core.init()
    command = require "core.command"
    keymap = require "core.keymap"
    RootView = require "core.root_view"
    StatusView = require "core.status_view"
    CommandView = require "core.command_view"
    TitleBar = require "core.titlebar"
    Doc = require "core.doc"

    local project_dir = EXEDIR
    local files = {}
    for i = 2, #ARGS do
        local info = system.get_file_info(ARGS[i]) or {}
        if info.type == "file" then
            local file_path = system.absolute_path(ARGS[i])
            table.insert(files, file_path)
            project_dir = file_path:sub(1, #file_path:match("(.*[/\\])") - 1)
        elseif info.type == "dir" then
            project_dir = system.absolute_path(ARGS[i])
        end
    end

    if string.sub(project_dir, #project_dir):match("[/\\]") then
        project_dir = string.sub(project_dir, 1, #project_dir - 1)
    end

    system.chdir(project_dir)
    core.project_dir = project_dir
    core.project_dir_name = project_dir:sub(#project_dir:match("(.*[/\\])") + 1)
    -- core.project_dir_time = system.file_time(core.project_dir) -- @note(maihd): this cause project files does not loaded in ProjectView
    core.project_dir_time = 0

    core.frame_start = 0
    core.clip_rect_stack = { { 0, 0, 0, 0 } }
    core.log_items = {}
    core.docs = {}
    core.threads = setmetatable({}, { __mode = "k" })
    core.project_files = {}
    -- core.project_files = get_files(core.project_dir)
    core.redraw = true

    core.root_view = RootView()
    core.command_view = CommandView()
    core.status_view = StatusView()
    core.titlebar = TitleBar()

--     core.log(core.project_dir)

    core.root_view.root_node:split("down", core.command_view, true)

    core.root_view.root_node.a:split("up", core.titlebar, true)
    core.root_view.root_node.b:split("down", core.status_view, true)

    command.add_defaults()

    local got_plugin_error = not core.load_plugins()
    local got_user_error = not core.try(require, "user")
    local got_project_error = not core.load_project_module()

    for _, filename in ipairs(files) do
        core.root_view:open_doc(core.open_doc(filename))
    end

    if got_plugin_error or got_user_error or got_project_error then
        command.perform("core:open-log")
    end

    core.add_thread(project_scan_thread)
    system.set_window_opacity(config.window_opacity or 1.0)
end


local temp_uid = (system.get_time() * 1000) % 0xffffffff
local temp_file_prefix = string.format(".lite_temp_%08x", temp_uid)
local temp_file_counter = 0

local function delete_temp_files()
    for _, filename in ipairs(system.list_dir(EXEDIR)) do
        if filename:find(temp_file_prefix, 1, true) == 1 then
            os.remove(EXEDIR .. PATHSEP .. filename)
        end
    end
end


function core.temp_filename(ext)
    temp_file_counter = temp_file_counter + 1
    return EXEDIR .. PATHSEP .. temp_file_prefix
        .. string.format("%06x", temp_file_counter) .. (ext or "")
end


function core.quit(force)
    if force then
        delete_temp_files()
        os.exit()
    end
    local dirty_count = 0
    local dirty_name
    for _, doc in ipairs(core.docs) do
        if doc:is_dirty() then
            dirty_count = dirty_count + 1
            dirty_name = doc:get_name()
        end
    end
    if dirty_count > 0 then
        local text
        if dirty_count == 1 then
            text = string.format("\"%s\" has unsaved changes. Quit anyway?", dirty_name)
        else
            text = string.format("%d docs have unsaved changes. Quit anyway?", dirty_count)
        end
        local confirm = system.show_confirm_dialog("Unsaved Changes", text)
        if not confirm then return end
    end
    core.quit(true)
end


function core.load_plugins()
    local no_errors = true
    local files = system.list_dir(EXEDIR .. "/data/plugins")
    for _, filename in ipairs(files) do
        local modname = "plugins." .. filename:gsub(".lua$", "")
        print("Load plugin: " .. modname)
        print("Load plugin filename: " .. filename)
        local ok = core.try(require, modname)
        if ok then
            core.log_quiet("Loaded plugin %q", modname)
        else
            no_errors = false
        end
    end
    return no_errors
end


function core.load_project_module()
    local filename = ".lite_project.lua"
    if system.get_file_info(filename) then
        return core.try(function()
            local fn, err = loadfile(filename)
            if not fn then error("Error when loading project module:\n\t" .. err) end
            fn()
            core.log_quiet("Loaded project module")
        end)
    end
    return true
end


function core.reload_module(name)
    local old = package.loaded[name]
    package.loaded[name] = nil
    local new = require(name)
    if type(old) == "table" then
        for k, v in pairs(new) do old[k] = v end
        package.loaded[name] = old
    end
end


function core.set_active_view(view)
    assert(view, "Tried to set active view to nil")
    if view ~= core.active_view then
        core.last_active_view = core.active_view
        core.active_view = view
    end
end


function core.add_thread(f, weak_ref)
    local key = weak_ref or #core.threads + 1
    local fn = function() return core.try(f) end
    core.threads[key] = { cr = coroutine.create(fn), wake = 0 }
end


function core.push_clip_rect(x, y, w, h)
    local x2, y2, w2, h2 = unpack(core.clip_rect_stack[#core.clip_rect_stack])
    local r, b, r2, b2 = x + w, y + h, x2 + w2, y2 + h2
    x, y = math.max(x, x2), math.max(y, y2)
    b, r = math.min(b, b2), math.min(r, r2)
    w, h = r - x, b - y
    table.insert(core.clip_rect_stack, { x, y, w, h })
    renderer.set_clip_rect(x, y, w, h)
end


function core.pop_clip_rect()
    table.remove(core.clip_rect_stack)
    local x, y, w, h = unpack(core.clip_rect_stack[#core.clip_rect_stack])
    renderer.set_clip_rect(x, y, w, h)
end


function core.open_doc(filename)
    if filename then
        -- try to find existing doc for filename
        local abs_filename = system.absolute_path(filename)
        for _, doc in ipairs(core.docs) do
            if doc.filename
                and abs_filename == system.absolute_path(doc.filename) then
                return doc
            end
        end

        -- check is binary file; skip opening
        -- @note(maihd): if open binary file, memory may leak
        if config.prevent_open_binary_file then
            local is_binary = system.is_binary_file(abs_filename)
            if is_binary then
                core.log("Open doc failed: " .. filename .. " is a binary file, skip opening!")
                return nil
            end
        end
    end

    -- no existing doc for filename; create new
    local doc = Doc(filename)
    table.insert(core.docs, doc)
    core.log_quiet(filename and "Opened doc \"%s\"" or "Opened new doc", filename)
    return doc
end


function core.get_views_referencing_doc(doc)
    local res = {}
    local views = core.root_view.root_node:get_children()
    for _, view in ipairs(views) do
        if view.doc == doc then table.insert(res, view) end
    end
    return res
end


local function log(icon, icon_color, fmt, ...)
    local text = string.format(fmt, ...)
    if icon and core.status_view then
        core.status_view:show_message(icon, icon_color, text)
    end

    local info = debug.getinfo(2, "Sl")
    local at = string.format("%s:%d", info.short_src, info.currentline)
    local item = { icon = icon, icon_color = icon_color, text = text, time = os.time(), at = at }
    table.insert(core.log_items, item)
    if #core.log_items > config.max_log_items then
        table.remove(core.log_items, 1)
    end
    return item
end


function core.log(...)
    return log("i", style.info, ...)
end


function core.log_quiet(...)
--     return log("i", style.text, ...)
    return log(nil, nil, ...)
end


function core.warn(...)
    return log("!", style.warn, ...)
end


function core.error(...)
    return log("!", style.error, ...)
end


function core.try(fn, ...)
    local err
    local ok, res = xpcall(fn, function(msg)
        local item = core.error("%s", msg)
        item.info = debug.traceback(nil, 2):gsub("\t", "")
        err = msg
    end, ...)
    if ok then
        return true, res
    end
    return false, err
end


function core.on_event(type, ...)
    local did_keymap = false
    if type == "textinput" then
        core.root_view:on_text_input(...)
    elseif type == "keypressed" then
        did_keymap = keymap.on_key_pressed(...)
    elseif type == "keyreleased" then
        keymap.on_key_released(...)
    elseif type == "mousemoved" then
        core.root_view:on_mouse_moved(...)
    elseif type == "mousepressed" then
        core.root_view:on_mouse_pressed(...)
    elseif type == "mousereleased" then
        core.root_view:on_mouse_released(...)
    elseif type == "mousewheel" then
        core.root_view:on_mouse_wheel(...)
    elseif type == "filedropped" then
        local filename, mx, my = ...
        local info = system.get_file_info(filename)
        if info and info.type == "dir" then
            system.exec(string.format("%q %q", EXEFILE, filename))
        else
            local ok, doc = core.try(core.open_doc, filename)
            if ok then
                local node = core.root_view.root_node:get_child_overlapping_point(mx, my)
                node:set_active_view(node.active_view)
                core.root_view:open_doc(doc)
            end
        end
        -- @note(maihd): not worked, because when sizing main thread is pause
--     elseif type == "resized" or type == "exposed" then
--         core.redraw = true
--         core.draw()

--         core.log("resized")
    elseif type == "quit" then
        core.quit()
    end
    return did_keymap
end


function core.step()
    -- handle events
    local did_keymap = false

    local mouse_move = nil
    local mouse_press = nil
    local mouse_move_before_pressed = false

    for type, a, b, c, d, e in system.poll_event do
        -- if type == "mousemoved" then
        --     mouse_move = { a, b, c, d, e}
        --     mouse_move_before_pressed = not mouse_press
        -- elseif type == "mousepressed" then
        --     mouse_press = core.mouse_press or {}
        --     if mouse_press[a] then
        --         mouse_press[a][4] = mouse_press[a][4] + 1
        --     else
        --         mouse_press[a] = { a, b, c, 1, e }
        --     end

        --     mouse_move_before_pressed = not mouse_move
        -- else
        if type == "textinput" and did_keymap then
            did_keymap = false
        else
            local _, res = core.try(core.on_event, type, a, b, c, d, e)
            did_keymap = res or did_keymap
        end
        core.redraw = true
    end
    -- if mouse_move_before_pressed then
    --     if mouse_move then
    --         core.try(core.on_event, "mousemoved", unpack(mouse_move))
    --     end

    --     for _, event in pairs(mouse_press or {}) do
    --         core.try(core.on_event, "mousepressed", unpack(event))
    --     end
    -- else
    --     for _, event in pairs(mouse_press or {}) do
    --         core.try(core.on_event, "mousepressed", unpack(event))
    --     end

    --     if mouse_move then
    --         core.try(core.on_event, "mousemoved", unpack(mouse_move))
    --     end
    -- end

    local width, height = renderer.get_size()

    -- update
    core.root_view.position.x, core.root_view.position.y = 0, 0
    core.root_view.size.x, core.root_view.size.y = width, height
    core.root_view:update()

    -- if not core.redraw then return false end
    -- core.redraw = false

    -- close unreferenced docs
    for i = #core.docs, 1, -1 do
        local doc = core.docs[i]
        if #core.get_views_referencing_doc(doc) == 0 then
            table.remove(core.docs, i)
            core.log_quiet("Closed doc \"%s\"", doc:get_name())
        end
    end

    -- update window title
    local name = core.active_view:get_name()
    local title_root = core.project_dir_name -- .. " - Lite" -- We have the icon, no need to present "Lite"
    local title = (name ~= "---") and (name .. " - " .. title_root) or title_root
    if title ~= core.window_title then
        system.set_window_title(title)
        core.window_title = title
    end

    -- Drawing
    core.draw()

    return true
end


function core.draw()
    local width, height = renderer.get_size()

    renderer.begin_frame()

    -- Draw root
    core.clip_rect_stack[1] = { 0, 0, width, height }
    renderer.set_clip_rect(unpack(core.clip_rect_stack[1]))
    core.root_view:draw()

    -- Draw border
    renderer.draw_rect(0, 0, 1, height, style.titlebar_background)
    renderer.draw_rect(width - 1, 0, 1, height, style.titlebar_background)
    renderer.draw_rect(0, height - 1, width, 1, style.titlebar_background)

    renderer.end_frame()
end


local run_threads = coroutine.wrap(function()
    while true do
        local max_time = 1 / config.fps - 0.004
        local ran_any_threads = false

        for k, thread in pairs(core.threads) do
            -- run thread
            if thread.wake < system.get_time() then
                local _, wait = assert(coroutine.resume(thread.cr))
                if coroutine.status(thread.cr) == "dead" then
                    if type(k) == "number" then
                        table.remove(core.threads, k)
                    else
                        core.threads[k] = nil
                    end
                elseif wait then
                    thread.wake = system.get_time() + wait
                end
                ran_any_threads = true
            end

            -- stop running threads if we're about to hit the end of frame
            if system.get_time() - core.frame_start > max_time then
                coroutine.yield()
            end
        end

        if not ran_any_threads then coroutine.yield() end
    end
end)


function core.run()
    local system_begin_frame = system.begin_frame
    local system_end_frame = system.end_frame

    local time = 0
    while true do
        system_begin_frame()

        core.frame_start = system.get_time()
        local did_redraw = core.step()
        run_threads()
        --     if not did_redraw and not system.window_has_focus() then
        --       system.wait_event(0.25)
        --     end
        local elapsed = system.get_time() - core.frame_start
        local frame = 1 / config.fps

        -- We have time to do GC
        if frame > elapsed then
            -- Do GC every seconds
            if time - math.floor(time) <= frame then
                -- core.log("Cleaning up memory...")
                collectgarbage("collect")
            end

            -- Recalculate the elapsed
            elapsed = system.get_time() - core.frame_start
        end

        local delta = 0
        if frame > elapsed then
            system.sleep(frame - elapsed)
            delta = frame
        else
            delta = elapsed

            -- Do GC every seconds (2nd attempt)
            -- @note(maihd): call here to avoid missing GC call
            if time - math.floor(time) <= frame then
                -- core.log("Cleaning up memory...")
                collectgarbage("collect")
            end
        end

        time = time + delta
        core.fps = math.min(60, 1 / delta)

        system_end_frame()
    end
end


function core.on_error(err)
    -- write error to file
    local fp = io.open(EXEDIR .. "/error.txt", "wb")
    fp:write("Error: " .. tostring(err) .. "\n")
    fp:write(debug.traceback(nil, 4))
    fp:close()
    -- save copy of all unsaved documents
    for _, doc in ipairs(core.docs) do
        if doc:is_dirty() and doc.filename then
            doc:save(doc.filename .. "~")
        end
    end
end


function core.add_ignore(pattern)
    if type(pattern) == "table" then
        for _, v in ipairs(pattern) do
            core.add_ignore(v)
        end

        return
    end

    if not core.ignore_files then
        core.ignore_files = {}
    end

    table.insert(core.ignore_files, pattern)
end


function core.is_ignore(file)
    if config.ignore_files and common.match_pattern(file, config.ignore_files) then
        return true
    end

    if not core.ignore_files then
        return false
    end

    return common.match_pattern(file, core.ignore_files)
end

return core
