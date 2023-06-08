
default :
	make img

ipl.bin : ipl.nas
	nasm ipl.nas -o ipl.bin

haribote.sys : haribote.nas
	nasm haribote.nas -o haribote.sys

floppy.img :
	dd if=/dev/zero of=floppy.img bs=512 count=2880

haribote.img : ipl.bin floppy.img haribote.sys
	# ./makeimg.py ipl.bin floppy.img
	./makeimg2.py ipl.bin haribote.sys floppy.img 0x004200
	mv floppy.img haribote.img

asm :
	make -r ipl.bin

img : 
	make -r haribote.img

run : haribote.img
	# qemu-system-i386 format:raw -fda haribote.img 
	qemu-system-i386 -drive file=haribote.img,index=0,if=floppy,format=raw

clean :
	rm  ipl.bin
	rm  haribote.img
	

