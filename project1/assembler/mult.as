        lw      0       1       mcand   reg1 = 32766 (symbolic address)
        lw      0       2       mplier  reg2 = 12328 (symbolic address)  
        lw      0       3       one     reg3 = 0x0001 (symbolic address)
loop    nor     0       3       4       reg4 = 0xFFFE
        nor     2       4       4       reg4 = 0xFFFF(if reg2 bit 1) or 0xFFFE(if reg2 bit 0)
        beq     0       4       addm
        beq     0       0       notadd
addm    add     1       5       5       reg5 += reg1
notadd  add     1       1       1       reg1 *= 2
        add     3       3       3       reg3 *= 2
        beq     0       3       done     if (reg3 == 0) break;
        beq     0       0       loop
done    add     0       5       1       reg1 = reg5
        halt                            end of program
mcand   .fill   32766
mplier  .fill   12328
numloop .fill   16
one     .fill   1
