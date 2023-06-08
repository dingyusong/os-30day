
# ��?����

default :
	make img

# ��?����?��

ipl.bin : ipl.nas
	nasm ipl.nas -o ipl.bin

floppy.img :
	dd if=/dev/zero of=floppy.img bs=512 count=2880

helloos.img : ipl.bin floppy.img	
	./makeimg.py ipl.bin floppy.img
	mv floppy.img helloos.img

# ??�C����?���C?�s

asm :
	make -r ipl.bin

img : 
	make -r helloos.img

run : helloos.img
	qemu-system-i386 -fda helloos.img


clean :
	rm  ipl.bin
	rm  helloos.img
	

