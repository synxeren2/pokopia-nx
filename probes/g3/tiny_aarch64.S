// G3: independent AArch64 assembly fixture, authored for Pokopia-NX.
// No code/data derived from proprietary Nintendo game binaries.
.text
.balign 4
.global tiny_sum
.type tiny_sum, %function
tiny_sum:
    adds x0, x0, x1
    ret
.size tiny_sum, . - tiny_sum

.global tiny_difference
.type tiny_difference, %function
tiny_difference:
    subs x0, x0, x1
    ret
.size tiny_difference, . - tiny_difference

.global _start
.type _start, %function
_start:
    mov x0, #12
    mov x1, #30
    bl tiny_sum
    b .
.size _start, . - _start
