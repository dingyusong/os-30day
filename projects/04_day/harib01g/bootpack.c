void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

#define COL8_000000		0		//黑色
#define COL8_FF0000		1		//亮红色
#define COL8_00FF00		2		//亮绿色
#define COL8_FFFF00		3		//亮黄色
#define COL8_0000FF		4		//亮蓝色
#define COL8_FF00FF		5		//亮紫色
#define COL8_00FFFF		6		//浅亮蓝色
#define COL8_FFFFFF		7		//白色
#define COL8_C6C6C6		8		//亮灰色
#define COL8_840000		9		//暗红色
#define COL8_008400		10		//暗绿色
#define COL8_848400		11		//暗黄色
#define COL8_000084		12		//暗青色
#define COL8_840084		13		//暗紫色
#define COL8_008484		14		//浅暗蓝色
#define COL8_848484		15		//暗灰色

void HariMain(void)
{
	char *p; /* p�Ƃ����ϐ��́ABYTE [...]�p�̔Ԓn */

	init_palette(); /* �p���b�g��ݒ� */

	p = (char *) 0xa0000; /* �Ԓn���� */

	boxfill8(p, 320, COL8_FF0000,  20,  20, 120, 120);
	boxfill8(p, 320, COL8_00FF00,  70,  50, 170, 150);
	boxfill8(p, 320, COL8_0000FF, 120,  80, 220, 180);

	for (;;) {
		io_hlt();
	}
}

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���邢�� */
		0x00, 0xff, 0x00,	/*  2:���邢�� */
		0xff, 0xff, 0x00,	/*  3:���邢���F */
		0x00, 0x00, 0xff,	/*  4:���邢�� */
		0xff, 0x00, 0xff,	/*  5:���邢�� */
		0x00, 0xff, 0xff,	/*  6:���邢���F */
		0xff, 0xff, 0xff,	/*  7:�� */
		0xc6, 0xc6, 0xc6,	/*  8:���邢�D�F */
		0x84, 0x00, 0x00,	/*  9:�Â��� */
		0x00, 0x84, 0x00,	/* 10:�Â��� */
		0x84, 0x84, 0x00,	/* 11:�Â����F */
		0x00, 0x00, 0x84,	/* 12:�Â��� */
		0x84, 0x00, 0x84,	/* 13:�Â��� */
		0x00, 0x84, 0x84,	/* 14:�Â����F */
		0x84, 0x84, 0x84	/* 15:�Â��D�F */
	};
	set_palette(0, 15, table_rgb);
	return;

	/* static char ���߂́A�f�[�^�ɂ����g���Ȃ�����DB���ߑ��� */
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* ���荞�݋��t���O�̒l���L�^���� */
	io_cli(); 					/* ���t���O��0�ɂ��Ċ��荞�݋֎~�ɂ��� */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ���荞�݋��t���O�����ɖ߂� */
	return;
}

/// @brief MARK		: （往buf对应的区域填充字符，如果buf是显存，则显示，否则只是构造buf）绘制一个矩形(有一个很大的作用就是将原来的内容覆盖掉，比如原来底色是红色，你写了个A，然后想将其清楚，就在那块继续绘制一个红色的矩形即可)
/// @param vram 	: 显存映射首地址
/// @param xsize 	: 绘制的矩形的宽度
/// @param c 		: 颜色
/// @param x0 		: 矩形左上角x轴坐标
/// @param y0 		: 矩形左上角y轴坐标
/// @param x1 		: 矩形右下角x轴坐标
/// @param y1 		: 矩形右下角y轴坐标
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}
