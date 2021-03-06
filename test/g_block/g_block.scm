
(display "hello from ./test/g_block/g_block.scm") (newline)

(define with-block (not (provided? 'windows)))
(define with-bignums (provided? 'gmp))

(if (and (defined? 'current-time) ; in Snd
	 (defined? 'mus-rand-seed))
    (set! (mus-rand-seed) (current-time)))

;; handler will be called by app
(define handler
  (lambda (a b c)
    (display (string-append "running g_block.scm handler; sum of args == "
                            (number->string (+ a b c))))
    (newline)

    (load "test.scm")

    (when with-block
      (let ((b (make-block 4)))
        (test (eq? b b) #t)
        (test (equal? b b) #t)
        (test (block? b) #t)
        (test (block? #()) #f)
        (test (block? #f) #f)
        (set! (b 0) 32)
        (test (b 0) 32.0)
        (let () (define (hi b i) (b i)) (test (hi b 0) 32.0))
        (let () (define (hi b) (b 0)) (test (hi b) 32.0))
        (let () (define (hi b) (b)) (test (hi b) 'error)) ;was 32!?
        (test b (block 32.0 0.0 0.0 0.0))
        (test (let? (block-let b)) #t)
        (test (((block-let b) 'float-vector?) b) #t)
        (test (object->string b) "(block 32.000 0.000 0.000 0.000)")
        (let ((lt (object->let b)))
          (test ((lt 'c-object-length) b) 4)
          (test ((lt 'c-object-reverse) b) (block 0 0 0 32))
          (test ((lt 'c-object-ref) b 0) 32.0))
        (let ((b1 (make-block 4)))
          (test (eq? b b1) #f))
        (let ((b (block 1 2)))
          (test (vector-ref b 0) 1.0)
          (test (vector-set! b 0 3) 3)
          (test b (block 3 2)))
        (test (map abs b) (list 32.0 0.0 0.0 0.0)))
      (let ((b (block 1 2 3)))
        (test (reverse b) (block 3 2 1))
        (test b (block 1 2 3))
        (test (reverse b 1) 'error)
        (test (reverse! b) (block 3 2 1))
        (test (reverse! b 1) 'error)
        (test b (block 3 2 1))
        (test (b 'a) #<undefined>)            ; hmmm -- it's thinking of methods
        (test ((block) 'a) #<undefined>))

      (test (object->string (block +nan.0)) "(block +nan.0)")
      (test (object->string (block +inf.0)) "(block +inf.0)")
      (test (object->string (block -inf.0)) "(block -inf.0)")
      (test (object->string (block pi)) "(block 3.142)")
      (test (object->string (block +nan.0) :readable) "(block +nan.0)")
      (test (object->string (block +inf.0) :readable) "(block +inf.0)")
      (test (object->string (block -inf.0) :readable) "(block -inf.0)")
      (test (object->string (block pi) :readable) "(block 3.141592653589793)") ; (block pi) would be better
      (let-temporarily (((*s7* 'float-format-precision) 8))
                       (test (object->string (block pi)) "(block 3.14159265)"))

      (let ((v (make-vector 2 (block 1.0) block?)))
        (test (block? (v 0)) #t)
        (vector-set! v 0 (block 2.0))
        (test (block-ref (vector-ref v 0) 0) 2.0)
        (test (#_block-ref (vector-ref v 0) 0) 2.0)
        (test (vector-set! v 0 #f) 'error)
        (test (signature v) (let ((lst (list 'block? 'vector? 'integer?))) (set-cdr! (cddr lst) (cddr lst)) lst)))

      (let ((h (make-hash-table 8 #f (cons symbol? block?))))
        (hash-table-set! h 'a (block 1.0))
        (test (block? (h 'a)) #t)
        (test (block-ref (h 'a) 0) 1.0)
        (test (hash-table-set! h 'b 'asdf) 'error)
        (test (hash-table-set! h "b" (block)) 'error)
        (test (signature h) '(block? hash-table? symbol?)))

      (let ((h (make-hash-table 8 #f (cons #t block?))))
        (hash-table-set! h 'a (block 2.0))
        (test (block? (h 'a)) #t)
        (test (block-ref (h 'a) 0) 2.0)
        (test (hash-table-set! h 'b 'asdf) 'error)
        (test (hash-table-set! h "b" (block)) (block))
        (test (signature h) '(block? hash-table? #t)))

      (let ((h (make-hash-table 8 #f (cons symbol? #t))))
        (hash-table-set! h 'a (block 2.0))
        (test (block? (h 'a)) #t)
        (test (block-ref (h 'a) 0) 2.0)
        (test (hash-table-set! h 'b 'asdf) 'asdf)
        (test (hash-table-set! h "b" (block)) 'error)
        (test (signature h) '(#t hash-table? symbol?))
        (test (block-ref (block 1.0 2.0 3.0) else) 'error))

      (test (make-hash-table 8 #f (cons #t #f)) 'error)
      (test (make-hash-table 8 #f ()) 'error)

      (let ((sig (list #t 'hash-table? #t)))
        (set-cdr! (cddr sig) (cddr sig))
        (test (signature (make-hash-table 8 #f (cons #t #t))) sig)) ; same as (signature (make-hash-table 8 #f [#f]))

      (test (blocks) (list 4 1))
      (test (blocks :frequency 2) (list 2 1))
      (let ((freq :frequency)) (test (blocks freq 2) (list 2 1)))
      (let ((freq :scaler)) (test (blocks freq 2) (list 4 2)))
      (let ((c #f)) (test (blocks (if c 100 :frequency) 10) (list 10 1)))
      (test (blocks :scaler 3 :frequency 2) (list 2 3))
      (test (blocks :scaler 3 :phase 1) 'error)
      (test (map blocks '(1 2 3)) '((1 1) (2 1) (3 1)))
      (test (map blocks '( 1 2 3) '(4 5 6)) '((1 4) (2 5) (3 6)))
      (test (documentation blocks) "test for function*")
      (test (apply blocks '(:frequency 5 :scaler 4)) '(5 4))
      (test (let () (define (b1) (blocks 100)) (b1)) '(100 1))
      (test (let () (define (b1) (blocks 10 2)) (b1)) '(10 2))
      (test (procedure? blocks) #t)
      (unless (or with-bignums (> (*s7* 'debug) 0)) ; debug turns off s7-optimize
        (test (s7-optimize '((block-append (make-block 2) (block)))) (block 0 0))) ; segfault due to plist overuse

      (let ((b1 (block 1)) (b2 (block 2)) (x1 2.0) (x2 3.0) (i 0) (j 0) (x 0.0))
        (define (g1) ; opt_d_dd_fff_rev
          (do ((i 0 (+ i 1))) ((= i 1) x) (set! x (+ (* x1 (block-ref b1 i)) (* x2 (block-ref b2 j))))))
        (test (g1) 8.0)
        (define (g2) ; opt_d_dd_fff
          (do ((i 0 (+ i 1))) ((= i 1) x) (set! x (+ (* (block-ref b1 i) x1) (* (block-ref b2 j) x2)))))
        (test (g2) 8.0))

      (test (unsafe-blocks) (list 4 1))
      (test (unsafe-blocks :frequency 2) (list 2 1))
      (test (unsafe-blocks :scaler 3 :frequency 2) (list 2 3))
      (test (unsafe-blocks :scaler 3 :phase 1) 'error)
      (test (map unsafe-blocks '(1 2 3)) '((1 1) (2 1) (3 1)))
      (test (map unsafe-blocks '( 1 2 3) '(4 5 6)) '((1 4) (2 5) (3 6)))
      (test (documentation unsafe-blocks) "test for function*")
      (test (apply unsafe-blocks '(:frequency 5 :scaler 4)) '(5 4))
      (test (let () (define (b1) (unsafe-blocks 100)) (b1)) '(100 1))
      (test (let () (define (b1) (unsafe-blocks 10 2)) (b1)) '(10 2))
      (test (procedure? unsafe-blocks) #t)

      (test (blocks3 (car (list :a))) 'error)
      (test (let () (define (func) (blocks3 (car (list :a)))) (func)) 'error)

      (test (let () (define (func) (unsafe-blocks :asdf)) (func)) 'error)   ; hop_safe_c_function_star_a bug
      (test (unsafe-blocks 3 :asdf) 'error)
      (test (let () (define (func) (unsafe-blocks 3 :asdf)) (func)) 'error) ; hop_c_aa bug

      (test (let () (define (func) (unsafe-blocks1 :asdf)) (func)) 'error) ; ??
      (test (let () (define (func) (unsafe-blocks1 :fdsa)) (func)) 'error) ; ??
      (test (unsafe-blocks1 3 :asdf) 'error)
      (test (let () (define (func) (unsafe-blocks1 3 :asdf)) (func)) 'error)
      (test (let () (define (func) (unsafe-blocks3 :asdf)) (func)) 'error)  
      (test (unsafe-blocks3 3 :asdf) 'error)
      (test (let () (define (func) (unsafe-blocks3 3 :asdf)) (func)) 'error)

      (test (let () (define (func) (unsafe-blocks3 1 3 :asdf)) (func)) 'error)  ; ?? all 6
      (test (unsafe-blocks3 1 3 :asdf) 'error)
      (test (let () (define (func) (unsafe-blocks3 1 3 :asdf)) (func)) 'error)
      (test (let () (define (func) (unsafe-blocks3 1 3 :fdsa)) (func)) 'error)  
      (test (unsafe-blocks3 1 3 :fdsa) 'error)
      (test (let () (define (func) (unsafe-blocks3 1 3 :fdsa)) (func)) 'error)

      (test (let () (define (func) (unsafe-blocks1 :asdf)) (func)) 'error)  ; h_c_a
      (test (let () (define (func) (unsafe-blocks1 :fdsa)) (func)) 'error) 

      (test (let () (define (func) (unsafe-blocks3 1 3 :asdf)) (func)) 'error)  ; h_c_fx
      (test (unsafe-blocks3 1 3 :asdf) 'error) 

      (test (let () (define (func) (unsafe-blocks3 1 3 :asdf)) (func)) 'error) 
      (test (let () (define (func) (unsafe-blocks3 1 3 :fdsa)) (func)) 'error)   
      (test (unsafe-blocks3 1 3 :fdsa) 'error) 
      (test (let () (define (func) (unsafe-blocks3 1 3 :fdsa)) (func)) 'error) 

      (test (let () (define (func) (blocks :asdf)) (func)) 'error) 
      (test (blocks 3 :asdf) 'error) 
      (test (let () (define (func) (blocks 3 :asdf)) (func)) 'error) 

      (test (blocks1 :asdf) 'error) 
      (test (let () (define (func) (blocks1 :asdf)) (func)) 'error)   ; h_safe_c_d
      (test (let () (define (func) (blocks1 :fdsa)) (func)) 'error)  
      (test (let ((sk :rest)) (define (func) (blocks1  sk)) (func)) 'error)

      (test (blocks1 3 :asdf) 'error) 
      (test (let () (define (func) (blocks1 3 :asdf)) (func)) 'error) 
      (test (let () (define (func) (blocks3 :asdf)) (func)) 'error)   
      (test (blocks3 3 :asdf) 'error) 
      (test (let () (define (func) (blocks3 3 :asdf)) (func)) 'error) 
      (test (let () (define (func) (blocks3 1 3 :asdf)) (func)) 'error)   
      (test (blocks3 1 3 :asdf) 'error) 

      (test (let () (define (func) (blocks3 1 3 :asdf)) (func)) 'error)  ; h_safe_c_d
      (test (let () (define (func) (blocks3 1 3 :fdsa)) (func)) 'error)   
      (test (blocks3 1 3 :fdsa) 'error) 

      (test (let () (define (func) (blocks4 :asdf)) (func)) 'error)   
      (test (blocks4 1 2 3 :etc) 'error) 
      (test (let () (define (func) (blocks4 3 :asdf)) (func)) 'error) 
      (test (let () (define (func) (blocks4 1 2 3 :etc)) (func)) 'error)   
      (test (blocks4 1 3 :fdsa) 'error) 
      (test (let () (define (func) (blocks4 1 2 3 :fdsa)) (func)) 'error)   
      (test (blocks4 1 2 3 :fdsa) 'error) 

      (test (blocks5) '(4))
      (test (blocks5 :frequency 440) '(440))
      (test (blocks5 :frequency 440 :amplitude 1.0) '(440))
      (test (blocks5 1) '(1))
      (test (blocks5 1 2) 'error) ; error: blocks5: too many arguments: (1 2)
      (test (blocks5 :a 1 :b 2) '(4))
      (test (blocks5 :a 1 :b 2 :frequency 440 :c 3) '(440))
      (test (let () (define (f) (blocks5 :a 1 :frequency 440)) (f)) '(440))
      (test (blocks5 :x) 'error) ; value missing
      (test (let () (define (f) (object->string (blocks5 (values :ho)))) (f)) 'error)
      (test (let () (define (f) (object->string (blocks5 (car (list :ho))))) (f)) 'error)
      (test (let () (define (f) (object->string (blocks4 (car (list :ho))))) (f)) 'error)
      (test (let () (define (f) (blocks5 :ho)) (f)) 'error)
      (test (let () (define (f) (blocks5 (symbol->keyword 'oops))) (f)) 'error)
      (test (let () (define (f) (blocks5 (string->keyword 'oops))) (f)) 'error)

      (test (call/cc (setter (block))) 'error)
      (test (call-with-exit (setter (block))) 'error)
      (test (call-with-input-string "123" (setter (block))) 'error)

      (let ((b (make-simple-block 4)))
        (test (eq? b b) #t)
        (test (equal? b b) #t)
        (test (simple-block? b) #t)
        (test (simple-block? #()) #f)
        (test (simple-block? #f) #f)
        (set! (b 0) 32)
        (test (b 0) 32.0)
        (test (length b) 4)
        (test (substring (object->string b) 0 17) "#<<simple-block> ")
        (test (substring (object->string b :readable) 0 17) "#<<simple-block> ")
        (let ((iter (make-iterator b)))
          (test (iterate iter) 32.0)
          (test (iter) 0.0)
          (test (object->string iter) "#<iterator: <simple-block>>"))
        (test (copy b) 'error)
        (test (reverse b) 'error)
        (test (reverse! b) 'error)
        (test (fill! b) 'error)
        (let ((b1 (make-simple-block 4)))
          (copy b b1)
          (test (equal? b b1) #f)
          (test (b1 0) 32.0)
          (test (append b b1) 'error))
        (s7-optimize '((b 0))))

      (test (make-vector 12 "ho" simple-block?) 'error)
      (test (signature (make-vector 12 (make-simple-block 1) simple-block?))
            (let ((L (list 'simple-block? 'vector? 'integer?)))
              (set-cdr! (cddr L) (cddr L))
              L))
      (test (make-vector 12 "ho" block?) 'error)
      (test (signature (make-vector 12 (make-block 1) block?))
            (let ((L (list 'block? 'vector? 'integer?)))
              (set-cdr! (cddr L) (cddr L))
              L))
      (test (make-vector 12 "ho" c-tag?) 'error)
      (test (signature (make-vector 12 (make-c-tag) c-tag?)) 'error)

      (test (let ((h (make-hash-table 8 #f (cons symbol? simple-block?)))) (hash-table-set! h 'a 1234)) 'error)
      (test (let ((h (make-hash-table 8 #f (cons symbol? simple-block?)))) (signature h)) '(simple-block? hash-table? symbol?))
      (test (let ((h (make-hash-table 8 #f (cons simple-block? symbol?)))) (hash-table-set! h (make-simple-block 1) 'a)) 'a)
      (test (let ((h (make-hash-table 8 #f (cons simple-block? symbol?)))) (signature h)) '(symbol? hash-table? simple-block?))

      (let ((g (make-cycle "123")))
        (test (cycle-ref g) "123")
        (test (substring (object->string g) 0 10) "#<<cycle> ")
        (test (object->string g :readable) "(<cycle> \"123\")")
        (test (cycle-set! g "321") "321")
        (test (cycle-ref g) "321")
        (test (equal? g 21) #f)
        (test (equal? g g) #t)
        (test (equal? g (make-cycle #\a)) #f)
        (test (equal? g (make-cycle "321")) #t)
        (test (equal? g (<cycle> "321")) #t)
        (cycle-set! g g)
        (let ((g1 (make-cycle g)))
          (test (equal? g g1) #t))
        (set! (g 0) #f)
        (test (cycle-ref g) #f)
        (set! (g 0) g)
        (test (substring (object->string g) 0 13) "#1=#<<cycle> ")
        (test-wi (object->string g :readable) "(let ((<1> (<cycle> #f))) (set! (<1> 0) <1>) <1>)")
        (let ((L (list 1)))
          (cycle-set! g L)
          (set! (L 0) g)
          (test-wi (object->string g :readable)
                   "(let ((<1> (<cycle> #f)))
                  (set! (<1> 0) (let ((<L> (list #f)))
                                  (set-car! <L> <1>)
                                  <L>))
                  <1>)"))
        (let ((g2 (make-cycle g)))
          (test (equal? g g2) #f))
        (let ((L (list 1)))
          (set-cdr! L L)
          (test-wi (object->string (make-cycle L) :readable) "(let ((<2> (<cycle> #f)) (<1> (list 1))) (set! (<2> 0) <1>) (set-cdr! <1> <1>) <2>)"))
        (let ((L (list (<cycle> 2) 3)))
          (set-cdr! (cdr L) L)
          (test-wi (object->string L :readable) "(let ((<1> (list (<cycle> 2) 3))) (set-cdr! (cdr <1>) <1>) <1>)"))
        (let ((L2 (make-list 3 #f))
              (C (<cycle> #f))
              (V1 (make-vector 3 #f)))
          (set! (L2 0) V1)
          (set! (V1 0) C)
          (set! (C 0) C)
          (set! (V1 1) L2)
          (test-wi (object->string L2 :readable)
                   "(let ((<3> (list #f #f #f))
                      (<2> (vector #f #f #f))
                      (<1> (<cycle> #f)))
                  (set-car! <3> <2>)
                  (set! (<2> 0) <1>)
                  (set! (<2> 1) <3>)
                  (set! (<1> 0) <1>)
                  <3>)"))
        (let ((L (list #f))
              (C (make-cycle #f)))
          (set! (L 0) C)
          (let ((IT (make-iterator L)))
            (set! (C 0) IT)
            (test-wi (object->string IT :readable)
                     "(let ((<1> #f)
                        (<3> (list #f))
                        (<2> (<cycle> #f)))
                    (set! <1> (make-iterator <3>))
                    (set-car! <3> <2>)
                    (set! (<2> 0) <1>)
                    <1>)")))
        (let ((cy (make-cycle #f))
              (it (make-iterator (make-list 3 #f)))
              (cp (c-pointer 1 (make-list 3 #f))))
          (set! (((object->let cp) 'c-type) 1) cy)
          (set! ((iterator-sequence it) 1) it)
          (set! (cy 0) it)
          (test-wi (object->string cp :readable)
                   "(let ((<4> (list #f #f #f))
                      (<3> (<cycle> #f))
                      (<1> #f)
                      (<2> (list #f #f #f)))
                  (set! <1> (make-iterator <2>))
                  (set! (<4> 1) <3>)
                  (set! (<3> 0) <1>)
                  (set! (<2> 1) <1>)
                  (c-pointer 1 <4> #f))")))

      (let ((b (make-c-tag)))
        (test (eq? b b) #t)
        (test (equal? b b) #t)
        (test (b 0) 'error)
        (test (length b) #f)
        (test (substring (object->string b :readable) 0 8) "#<c-tag ")
        (let ((str (object->string b)))
          (test (substring str 0 8) "#<c-tag ")
          (test (str (- (length str) 1)) #\>))
        (let ((iter (make-iterator b)))
          (test (iterate iter) #<eof>)
          (test (object->string iter) "#<iterator: c-tag>"))
        (test (copy b) 'error)
        (test (reverse b) 'error)
        (test (reverse! b) 'error)
        (test (fill! b) 'error)
        (let ((b1 (make-c-tag)))
          (copy b b1)
          (test (equal? b b1) #f)
          (test (append b b1) 'error))
        (test (setter (make-c-tag)) #f))

      (define (fv32)
        (let ((b (block 1 2 3 4))
              (f (make-float-vector 4)))
          (do ((i 0 (+ i 1)))
              ((= i 4) f)
            (set! (f i) (+ (b i) 1.0)))))
      (test (fv32) (float-vector 2.0 3.0 4.0 5.0))

      (define (fv33)
        (let ((b (block 1 2 3 4))
              (f (make-block 4)))
          (do ((i 0 (+ i 1)))
              ((= i 4) f)
            (set! (f i) (+ (b i) 1.0)))))
      (test (fv33) (block 2.0 3.0 4.0 5.0))

      (define (fv34)
        (let ((b (block 1 2 3 4))
              (f (make-vector 4)))
          (do ((k 0 (+ k 1)))
              ((= k 1) f)
            (do ((i 0 (+ i 1)))
                ((= i 4))
              (set! (f i) (b i))))))
      (test (fv34) (vector 1.0 2.0 3.0 4.0)))

    99))

;; last value of file will be the result of s7_load()
(/ 35 5)
