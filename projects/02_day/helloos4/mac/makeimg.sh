#!/bin/bash
# copy helloos.img ..\z_tools\qemu\fdimage0.bin
# ..\z_tools\make.exe	-C ../z_tools/qemu



dd bs=512 count=2880 if=/dev/zero of=floppy.img

dd if=hello.img of=floppy.img bs=512 count=1

