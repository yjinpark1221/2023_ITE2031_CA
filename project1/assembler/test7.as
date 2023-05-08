      lw 0 1 one
      add 0 1 2 result
      add 0 0 3 index
      lw 0 4 exp
loop  beq 3 4 break
      add 2 2 2
      add 1 3 3
      beq 0 0 loop
break halt
one   .fill 1
exp   .fill 6
