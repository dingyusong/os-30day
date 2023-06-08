; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序的装载地址（ORG指令。这个指令会告诉nask，在开始执行的时候，把这些机器语言指令 装载到内存中的哪个地址。）汇编语言源程序中若没有ORG伪指令，则程序执行时，指令代码被放到自由内存空间的CS:0处；若有ORG伪指令，编译器则把其后的指令代码放到ORG伪指令指定的偏移地址。两个ORG伪指令之间，除了指令代码，若有自由空间，则用0填充。

; 以下的记述用于标准FAT12格式的软盘

		JMP		entry			; 0xeb, 0x4e,(78) 跳到地址78处执行！（entry的地址由编译器算出来！）（上面的ORG是伪指令，没有机器码，所以BIOS将程序载入到0x7C00后的第一条指令是这个！）
		DB		0x90
		DB		"HELLOIPL"		; 启动区的名称可以是任意的字符串(8字节)
		DW		512				; 每个扇区(sector)的大小(必须为512字节)
		DB		1				; 簇(cluster)的大小(必须为1个扇区)
		DW		1				; FAT的起始位置(一般从第一个扇区开始)
		DB		2				; FAT的个数(必须为2)
		DW		224				; 根目录的大小(一般设成224项)
		DW		2880			; 该磁盘的大小(必须是2880扇区)
		DB		0xf0			; 磁盘的种类(必须是0xf0)
		DW		9				; FAT的长度(必须是9扇区)
		DW		18				; 1个磁道(track)有几个扇区(必须是18)
		DW		2				; 磁头数(必须是2)
		DD		0				; 不使用分区，必须是0
		DD		2880			; 重写一次磁盘大小2880扇区
		DB		0,0,0x29		; 意义不明，固定
		DD		0xffffffff		; (可能是)卷标号码
		DB		"HELLO-OS   "	; 磁盘的名称(11字节)
		DB		"FAT12   "		; 磁盘格式名称(8字节)
		RESB	18				; 先空出18字节


entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]			; 用方括号把SI括了起来,这个记号代表“内存”。也就是要取【msg】标号内存的内容
		ADD		SI,1			; 给SI加1,前面已经读一个值后，SI将指针往前移动一位。
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色。
		INT		0x10			; 调用显卡BIOS。功能 0EH 入口参数:AH=0EH AL=字符 BH=页码 BL=前景色(图形模式)（参考BIOS中断调用大全）
		JMP		putloop
fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环

msg:
		DB		0x0a, 0x0a		; 换行两次
		DB		"hello, world"
		DB		0x0a			; 换行
		DB		0				; 当上面的si执行到这个0的时候 CMP AL,0 就相等了，JE	fin，然后会跳到无限循环待机中！

		RESB	0x1fe-$		; 填写0x00直到0x001fe

		DB		0x55, 0xaa

; 启动扇区以外部分输出及填充

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
