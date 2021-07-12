
(display "hello from ./demos/cstruct.scm") (newline)

(write "another message") (newline)

(+ 2 3)

;; handler will be called by app
(define handler
  (lambda (a b c)
    (display (string-append
              "running cstruct.scm handler; sum of args == "
              (number->string (+ a b c))))
    (newline)
    (let ((x (make-cstruct #\a))
          )
      (if (cstruct? x)
          (begin (display "WOOT") (newline))
          (beding (display "SHUCKS") (newline))))
    99))

;; last value of file will be the result of s7_load()
(/ 35 5)
