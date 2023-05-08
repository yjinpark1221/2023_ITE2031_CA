        lw 0 1 first
        lw 0 2 second
        add 0 1 3
        sw 0 2 first
        sw 0 3 second
        halt
first   .fill 1234
second  .fill 5678
