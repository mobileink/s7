(display "hello from ./demos/cstruct.scm")
(newline)

(define (applicator-test)
  (display "APPLICATOR-TEST") (newline)

  (define cs1 (make-cstruct))

  ;; specialized ref
  (display (cstruct-ref cs1 :str)) (newline)

  ;; applicator ref - redirects to cstruct-ref
  (display (cs1 :str)) (newline)

  ;; applicator method - looks up symbol in method table
  (display "VECTOR-REF") (newline)
  (display (keyword? 'a)) (newline)
  (display (cs1 'vector-ref)) (newline)
  (display ((cs1 'vector-ref) cs1 :str)) (newline)

  ;; methods whose names match Scheme proc names extend the latter;
  ;; here built-in vector ref delegates the call to our method, which is (cs1 'vector-ref)
  (display (vector-ref cs1 :str)) (newline) ;; -> ((cs1 'vector-ref) cs1 :str)

  ;; we can do this for any predefined proc, no matter how perverse:
  ;; (display (memq cs1 :str)) (newline) ;; => ;memq argument 2, :str, is a symbol but should be a list
  (display "PERVERSE MEMQ") (newline)
  (display (cs1 'memq)) (newline)
  (display ((cs1 'memq) cs1 :str)) (newline) ;; => ;memq argument 2, :str, is a symbol but should be a list

  ;; we can define whatever methods we please
  (display "CUSTOM") (newline)
  (display ((cs1 'foo) cs1)) (newline) ;; => "hello from foo method!"

  ;; but if there is no procedure with same name we cannot do this:
  ;; (display (foo cs1 :str)) (newline) => unbound variable foo

  ;; our methods will only extend a predefined set of procs (vector-ref, arity, etc.)
  (define (foo o arg) "hi from foo proc")
  (display (foo cs1 :str)) (newline) ;; => "hi from foo proc", not "hello from foo method!"

  (display ((cs1 'foo) cs1)) (newline) ;; => "hello from foo method!"

  ;; (display (object->string cs1 :readable)) (newline)
  ;; (display (object->string cs2)) (newline)
  )

(define (method-test)
  (display "METHOD-TEST") (newline)

  (display (format #f "(keyword? :a) ~A" (keyword? (string->keyword "a"))))
  (newline)
  (display (format #f "(symbol? :a) ~A" (symbol? (string->keyword "a"))))
  (newline)


  (define cs1 (make-cstruct))
  ;; (define sub1 (make-cstruct :str "substruct1"))
  ;; (set! (cs1 :substruct) sub1)

  (display (cs1 'vector-ref)) (newline)

  (display (cstruct-ref cs1 :str)) (newline)
  (display ((cs1 'vector-ref) cs1 :str)) (newline)
  (display (cs1 :str)) (newline)

  ;; (display (object->string cs1 :readable)) (newline)
  ;; (display (object->string cs2)) (newline)
  )

(define (copy-test)

  (define cs1 (make-cstruct))
  ;; (define sub1 (make-cstruct :str "substruct1"))
  ;; (set! (cs1 :substruct) sub1)

  (define cs2 (copy cs1))

  ;; (display (object->string cs1 :readable)) (newline)
  (display (object->string cs2)) (newline)

  (display (#(a b c) 1)) (newline)

  )

(define (equality)

  (define cs1 (make-cstruct))
  (define sub1 (make-cstruct :str "substruct1"))
  (set! (cs1 :substruct) sub1)

  (define cs2 (make-cstruct))
  (define sub2 (make-cstruct :str "substruct2"))
  (set! (sub2 :c) #\y)
  (set! (cs2 :substruct) sub2)

  (display (format #f "eq? ~A" (eq? cs1 cs2))) (newline)
  (display (format #f "eqv? ~A" (eqv? cs1 cs2))) (newline)
  (display (format #f "equal? ~A" (equal? cs1 cs2))) (newline)
  (display (format #f "equivalent? ~A" (equivalent? cs1 cs2))) (newline)

  (display (object->string cs1 :readable)) (newline)
  (display (object->string cs2)) (newline)
  )

(define (setters)
    (define-constant cs1
      ;; (make-cstruct :i 1)
      (make-cstruct)
      )
    (define cs2
      ;; (make-cstruct :i 2)
      (make-cstruct)
      )

    (display "==== (cstruct-set! cs1 :pd 3.14) > set_specialized") (newline)
    (cstruct-set! cs1 :pd 3.14) ;; calls set_specialized

    ;; function setters must be added in Scheme
    (set! (setter cstruct-ref) cstruct-set!)
    ;; now we can use cstruct-ref in a set! context:
    (display "==== (set! (cstruct-ref cs1 :str) \"bye\") > set_generic") (newline)
    (set! (cstruct-ref cs1 :str) "bye") ;; calls setter of cstruct-ref

    (display "==== (set! (cs1 :str) \"bye\") > set_generic") (newline)
    (set! (cs1 :str) "bye") ;; calls set_generic, passing :str "bye"

    (display "==== (cs1 :str) > ref_dispatcher (not ref_generic!)") (newline)
    (cs1 :str) ;; calls set_generic, passing :str "bye"

    (display "==== (ref cs1 :str) > generic ref, NOT SUPPORTED") (newline)
    ;; (ref cs1 :pd) ;; calls ref_generic NOT SUPPORTED

    (display "==== (cstruct-ref cs1 :str) > ref_specialized") (newline)
    (cstruct-ref cs1 :pd) ;; calls ref_specialized

    (display "================") (newline)
    ;; in set! context, (cs1 :str) does NOT call any ref method, only set
    (display (format #f "(set! (cs1 :str) \"bye\") -> ~A" cs1))
    (newline)

    (let ((cref (cs1 :str))) ;; calls ref_dispatcher which calls ref_specialized
      (display (format #f "(cs1 :str) -> ~A" cref))
      (newline))

    (let ((cref (cstruct-ref cs1 :pd))) ;; calls ref_specialized
      (display (format #f "(cstruct-ref :pd) -> ~A" cref))
      (newline))

    (set! (cs1 :str) "bye") ;; calls set_generic, passing :str "bye"
    ;; in set! context, (cs1 :str) does NOT call any ref method, only set
    (cs1 :str) ;; calls dispatcher which calls ref_specialized

    ;; (display (x :c)) (newline)
    (let* (;;(y cs1)

          ;; (v '#(a b c))
          ;; (vsref (vector-ref v 1)) ;; specialized ref
          ;; ;; (vgref (ref v)) ;; 1)) ;; generic ref - not predefined!
          ;; (vmref (v 1)) ;; method ref

           ;; (sref (cstruct-ref cs1 :i))
           ;; (gref (ref cs1 :i))
           ;; (sumints (cs1 :sumints))
          )
      (display (object->string cs1 :readable)) (newline)
      ;; (display (format #f "sref: ~A" sref)) (newline)
      ;; (display (format #f "sref: ~A" sref)) (newline)
      ;; (display (format #f "gref: ~A" gref)) (newline)
      ;; (display (format #f "sumints: ~A" sumints)) (newline)
      ;; (display cs2) (newline)
      ;; (display (format #f "(eq? cs1 cs2) ~A" (eq? cs1 cs2))) (newline)
      ;; (display (format #f "(eqv? cs1 cs2) ~A" (eqv? cs1 cs2))) (newline)
      ;; (display (format #f "(equal? cs1 cs2) ~A" (equal? cs1 cs2))) (newline)
      ;; (display (format #f "(equivalent? cs1 cs2) ~A" (equivalent? cs1 cs2))) (newline)
      ;; (provide 'foo)
      ;; (require 'foobar)
      ;; (display (provided? 'foo)) (newline)
      ;; (display (format #f "set! result: ~A" x)) (newline)
      ;; (display (format #f "(x :d): ~A" (x :d))) (newline)
      ;; (display (format #f "(x :pd): ~A" (y :pd))) (newline)
      )
      ;; (display (c-object-let x))
      ;; (newline))
    ;; (let ((x (make-cstruct #\a))
    ;;       )
    ;; (if (cstruct? x)
    ;;     (begin (display "WOOT") (newline))
    ;;     (begin (display "SHUCKS") (newline)))
    )

;; handler will be called by app
(define handler
  (lambda (a)
    (display "running cstruct.scm handler") (newline)

    ;; (display (string-append
    ;;           "running cstruct.scm handler; sum of args == "
    ;;           (number->string (+ a b c))))
    ;; (newline)

    ;; (setters)
    ;; (equality)
    ;; (copy-test)
    (applicator-test)
    ;; (method-test)

    ;; (set! (a :str) "bye")

    ;; ((setter a) a :d 97)

    ;; (display (format #f "setter: ~A"
    ;;                  (object->string (setter a)))) (newline)

    ;; (display (object->string a :readable)) (newline)

    99))

;; last value of file will be the result of s7_load()
(/ 35 5)
