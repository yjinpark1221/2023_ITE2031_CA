target  lw 0 1 one reg1 = 1
        beq 1 2 behalt
        lw 1 2 one reg2 = 10
        add 1 2 3 reg3 = reg1 + reg2
        add 1 1 1 reg1 *= 2
        add 1 1 1 reg1 *= 2
        add 1 1 1 reg1 *= 2
        add 1 3 3 reg3 = reg1(8) + reg3(11)
        sw 0 2 one mem[11] = reg2(10)
        beq 0 0 target
behalt  beq 0 0 halt
one     .fill 1
ten     .fill 10
halt    .fill 25165824
