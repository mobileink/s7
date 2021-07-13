
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
    (define-constant x (make-cstruct :i 11 :pi 22 :c #\x :s "bye"))
    (begin
      ;; (display x) (newline)
      ;; (write x) (newline)
      (display (object->string x)) ;; :readable)
      (newline)
       ;; (output-port? (current-output-port)))
      ;; (write "xxxxxxxxxxxxxxxx" (current-output-port))
       ;; (current-output_port)
      ;; (newline)
      )
      ;; (display (c-object-let x))
      ;; (newline))
    ;; (let ((x (make-cstruct #\a))
    ;;       )
    ;; (if (cstruct? x)
    ;;     (begin (display "WOOT") (newline))
    ;;     (begin (display "SHUCKS") (newline)))
    99))

;; last value of file will be the result of s7_load()
(/ 35 5)
