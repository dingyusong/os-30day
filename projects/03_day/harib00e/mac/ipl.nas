; haribote-ipl
; TAB=4

CYLS	EQU		10				; CYLS代表 cylinders(圆柱)，表示要读10个柱面。（EQU指令。这相当于C语言的#define命令，用来声明常数。“CYLS EQU 10”意思 是“CYLS = 10”。EQU是“equal”的缩写。）

		ORG		0x7c00			; 指明程序的装载地址（ORG指令。这个指令会告诉nask，在开始执行的时候，把这些机器语言指令 装载到内存中的哪个地址。）汇编语言源程序中若没有ORG伪指令，则程序执行时，指令代码被放到自由内存空间的CS:0处；若有ORG伪指令，编译器则把其后的指令代码放到ORG伪指令指定的偏移地址。两个ORG伪指令之间，除了指令代码，若有自由空间，则用0填充。

; 以下的记述用于标准FAT12格式的软盘

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; 启动区的名称可以是任意的字符串(8字节)
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
		DB		"HARIBOTEOS "	; 磁盘的名称(11字节)
		DB		"FAT12   "		; 磁盘格式名称(8字节)
		RESB	18				; 先空出18字节

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 读取磁盘

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
readloop:
		MOV		SI,0			; 记录失败次数寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : 读入磁盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0			; 数据读到ES:BX
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS,读磁盘
		JNC		next			; 没出错则跳转到next
		ADD		SI,1			; 往SI加1
		CMP		SI,5			; 比较SI和5
		JAE		error			; SI >= 5 跳转到error
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; AH=00 重置驱动器（0x13，是操作磁盘，至于是读还是重置，这个要看AH寄存器）
		JMP		retry
next:
		MOV		AX,ES			; �A�h���X��0x200�i�߂�
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 �Ƃ������߂��Ȃ��̂ł������Ă���
		ADD		CL,1			; CL��1�𑫂�
		CMP		CL,18			; CL��18���r
		JBE		readloop		; CL <= 18 ��������readloop��
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 ��������readloop��
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS ��������readloop��

; �ǂݏI��������ǂƂ肠������邱�ƂȂ��̂ŐQ��

fin:
		HLT						; ��������܂�CPU���~������
		JMP		fin				; �������[�v

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1�𑫂�
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; �ꕶ���\���t�@���N�V����
		MOV		BX,15			; �J���[�R�[�h
		INT		0x10			; �r�f�IBIOS�Ăяo��
		JMP		putloop
msg:
		DB		0x0a, 0x0a		; ���s��2��
		DB		"load error"
		DB		0x0a			; ���s
		DB		0

		RESB	0x1fe-($-$$)		; 0x7dfe�܂ł�0x00�Ŗ��߂閽��

		DB		0x55, 0xaa
