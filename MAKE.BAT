tcc -ms opgen.c
tcc -ms vecgen.c
tcc -ms strip.c
as65 test.a65
bintoasm test.bin >asm.8
opgen >6502_Gen.8
vecgen 6502_op.8 6502_Gen.8 >6502_Vec.8
a86 6502_if.8 demo.8 sysmac.8 6502_mac.8 6502.8 6502_op.8 6502_Gen.8 6502_Vec.8 memory.8 asm.8 to 6502.com

copy 6502_mac.8+6502.8+6502_op.8+6502_Gen.8+6502_Vec.8 _6502.tmp
strip <_6502.tmp >_6502.8
a86 6502_if.8 sysmac.8 demo.8 _6502.8 memory.8 asm.8 TO _6502.com
copy _6502.8 ..
copy _6502.8 c:\vic20
copy _6502.8 c:\apple2
