@echo off

set name="bsides"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source
ca65 reset.s
ca65 %name%.s
ca65 asm4c.s

ld65 -C nes.cfg -o bsides.nes reset.o %name%.o asm4c.o nes.lib

del *.o
del bsides.s

bsides.nes
