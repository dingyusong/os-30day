#!/bin/bash
# copy helloos.img ..\z_tools\qemu\fdimage0.bin
# ..\z_tools\make.exe	-C ../z_tools/qemu


# qemu-system-i386 -hdd ../helloos.img        #硬盘
# qemu-system-i386 -fda ../helloos.img -boot a       #软盘

qemu-system-i386 -drive file=../helloos.img,index=0,if=floppy,format=raw, -boot a 

# qemu-system-i386 -blockdev driver=file,node-name=f0,filename=../helloos.img -device floppy,drive=f0
