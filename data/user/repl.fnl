(local fennel (require :user.fennel))
(local {: log : error &as core} (require :core))

(each [key value (pairs fennel.repl)]
  (log key (tostring value)))

;; Initialize repl
(partial fennel.repl {:moduleName "user.fennel"
                      :readChunk coroutine.yield
                      :onValues #(log (table.concat $...))
                      :onError #(error (table.concat $...))
                      :pp fennel.view
                      :allowedGlobals true})

(local repl (coroutine.create
             (fn []
               (while true
                 (local input (coroutine.yield))
                 (coroutine.yield (fennel.dostring input))))))
(coroutine.resume repl)
