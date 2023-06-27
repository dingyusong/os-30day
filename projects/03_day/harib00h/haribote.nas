; haribote-os
; TAB=4

; BOOT_INFO
CYLS	EQU		0x0ff0			; MARK: 设定启动区
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; MARK: 关于颜色的数目信息，颜色的位置
SCRNX	EQU		0x0ff4			; MARK: 分辨率的X
SCRNY	EQU		0x0ff6			; MARK: 分辨率的Y
VRAM	EQU		0x0ff8			; MARK: 图像缓冲区的开始地址

		ORG		0xc200			; MARK: 这个程序要被装载到内存的什么地方

		MOV		AL,0x13			; MARK: VGA显卡，320*200*8位彩色
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; MARK: 记录画面模式
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 用BIOS取得键盘上各种LED指示灯的状态

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

fin:
		HLT
		JMP		fin
