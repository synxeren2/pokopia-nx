// SPDX-License-Identifier: GPL-3.0-or-later
// G6: self-authored AArch64 fixture for REAL lifted-memory instructions.
.text
.balign 4
.global g6_add64
.type g6_add64, %function
g6_add64:
    ldr x2, [x0]
    add x2, x2, x1
    str x2, [x0]
    ret
.size g6_add64, . - g6_add64

.global g6_add32
.type g6_add32, %function
g6_add32:
    ldr w2, [x0]
    add w2, w2, w1
    str w2, [x0]
    ret
.size g6_add32, . - g6_add32

.global _start
.type _start, %function
_start:
    b .
.size _start, . - _start
