Lua string.find/string.gmatch cheatsheet
----------------------------------------
Have you ever hard to find string.find/string.gmatch (used in language plugins) docs? This simple doc should help.

-- CHARACTER CLASSES
  .      -- all characters
  %a    -- letters
  %c    -- control characters
  %d    -- digits
  %l    -- lower case letters
  %p    -- punctuation characters
  %s    -- space characters
  %u    -- upper case letters
  %w    -- alphanumeric characters
  %x    -- hexadecimal digits
  %z    -- the character with representation 0

-- MAGIC CHARACTERS
  . % * + - ? ^ $ [ ] ( )
  .   -- all characters
  %   -- escape character for theses
         %. -- matches a .
         %% -- matches a %
  *   -- match previous character/class zero or more times, as many times as possible
  +   -- match previous character/class one or more times, as many times as possible
  -   -- match previous character/class zero or more times, as few times as possible
  ?   -- make the previous character/class option
  ^   -- start of string
  $   -- end of string
  []  -- make your own class
  ()  -- captures
         date = "04/19/64"
         m, d, y = string.match(date, "(%d+)/(%d+)/(%d+)")
         -- with a small [] class
         local date, time = string.match(line, "(%a+ %d+, %d%d%d%d )(%d+:%d+[ap]m)")
         print("19" .. y)  --> 1964
