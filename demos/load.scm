
(display "hello from ./demos/load.scm") (newline)

(write "another message") (newline)

(+ 2 3)

;; handler will be called by app
(define handler
  (lambda (a b c)
    (display (string-append "running load.scm handler; sum of args == "
                            (number->string (+ a b c))))
    (newline)
    99))

;; last value of file will be the result of s7_load()
(/ 35 5)
