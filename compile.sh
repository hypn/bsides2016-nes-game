#!/bin/bash
cc65 -Oi bsides.c --add-source
ca65 reset.s
ca65 bsides.s
ca65 asm4c.s

ld65 -C nes.cfg -o bsides.nes reset.o bsides.o asm4c.o nes.lib -Ln labels.txt

rm *.o
rm bsides.s
