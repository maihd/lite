(local fennel (require :user.fennel))
(local command (require :core.command))
(local {: log : error &as core} (require :core))

;(tset command.map :user:reinit k nil)
;(command.add nil {:user:reinit #(fennel.dofile "user/data/config.fnl")})

(local repl (coroutine.create (fennel.dofile "data/user/repl.fnl")))

(fn handle [input]
  (local output (coroutine.resume repl (.. input "\n")))
  (log (tostring output)))

(fn rep []
  (core.command_view:enter :Eval handle))

(tset command.map :user:rep nil)
(command.add nil {:user:rep rep})
