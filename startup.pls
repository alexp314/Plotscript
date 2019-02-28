(begin
(define make-point (lambda (a b)(set-property "object-name" "point" (set-property "size" (0)(list a b)))))
(define make-line (lambda (a b)(set-property "object-name" "line" (set-property "thickness" (1)(list a b)))))
(define make-text (lambda (x) (set-property "object-name" "text" (set-property "position"(make-point 0 0) (set-property "text-scale" (1) (set-property "text-rotation" (0) (x)))))))
)