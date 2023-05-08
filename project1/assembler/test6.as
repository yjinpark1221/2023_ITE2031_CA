      lw 0 1 one constant     reg1 = 1
      lw 0 2 array            reg2 = 
      add 0 0 3               reg3 = 0
      lw 0 4 five             reg4 = 8
loop  beq 3 4 break           if (reg3 == 8) break
      add 1 3 3               reg3++
      add 0 1 5               reg5 = 1
      lw 3 6 array            reg6 = array[reg3]
loop2 beq 3 5 loop            if (reg5 == reg3) break
      add 6 6 6
      add 1 5 5
      sw 3 6 array
      beq 0 0 loop2
break halt
five  .fill 5
array .fill 0
one   .fill 1
      .fill 10
      .fill 100
      .fill 1000
      .fill 10000
