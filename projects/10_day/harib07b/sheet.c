/* �}�E�X��E�B���h�E�̏d�ˍ��킹���� */

#include "bootpack.h"

#define SHEET_USE		1

/// @brief 图层初始化
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	// 用于记忆图层控制变量的 内存空间，这时必须指定该变量所占空间的大小，不过我们可以使用sizeof(struct SHTCTL)这种写法，让C编译器自动计算。只要写sizeof(变量型)，C编译器就 会计算出该变量型所需的字节数。
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));
	if (ctl == 0) {
		goto err;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* �V�[�g�͈ꖇ���Ȃ� */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = 0; /* ���g�p�}�[�N */
	}
err:
	return ctl;
}

/// @brief 初始化一个图层（就是将其注册到图层控制器里面，让图层控制器将其管理起来）
struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* 标记为正在使用 */
			sht->height = -1; /* 隐藏 */
			return sht;
		}
	}
	return 0;	/* 所有的SHEET都处于正在使用状态 */
}

/// @brief 图层的缓冲区大小和透明色的函数
/// @param sht 图层指针
/// @param buf 图层缓冲区指针
/// @param xsize 图层的宽度
/// @param ysize 图层的高度
/// @param col_inv 图层的透明色色号
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

/// @brief MARK: 设定图层高度（类似level）
/// @param ctl 
/// @param sht 
/// @param height 
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* 存储设置前的高度信息 */

	/* 如果指定的高度过高或过低，则进行修正 */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* 设定高度 */

	/* 下面主要是进行sheets[ ]的重新排列 */
	if (old > height) {	/* 比以前低 */
		if (height >= 0) {
			/* 把中间的往上提 */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;	//图层地址赋值
		} else {	/*old > height 说明以前（old-1）最小是0，原来是显示的，现在要隐藏*/
			if (ctl->top > old) {
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* 由于显示中的图层减少了一个，所以最上面的图层高度下降 */
		}
		sheet_refresh(ctl); /* 按新图层的信息重新绘制画面 */
	} else if (old < height) {	/* 比以前高 */
		if (old >= 0) {
			/* 把中间的拉下去 */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 由隐藏状态转变为显示状态 */
			/* 将已在上面的提上来 */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;	//图层地址赋值
			ctl->top++; /* 由于已经显示的图层增加了1个，所以最上面的图层高度增加 */
		}
		sheet_refresh(ctl); /* 按照新的图层信息重新绘画 */
	}
	return;
}


/// @brief MARK: 刷新图层(从上到下，描绘所有的图层)，注意：图层的重叠，表现在vram上是后面的图层会覆盖前面的图层。
/// @param ctl 
void sheet_refresh(struct SHTCTL *ctl)
{
	int h, bx, by, vx, vy;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	for (h = 0; h <= ctl->top; h++) {				//一层一层的画(除了隐藏的（-1），其他的从下到上都要画)
		sht = ctl->sheets[h];
		buf = sht->buf;
		for (by = 0; by < sht->bysize; by++) {		//一行一行的画
			vy = sht->vy0 + by;						// vy0窗口的Y坐标。by是buf的Y坐标，内部偏移量。整个是VRAM的Y坐标
			for (bx = 0; bx < sht->bxsize; bx++) {	//一列一列的画
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx]; 	//取出那个像素点要显示的颜色信息
				if (c != sht->col_inv) {
					vram[vy * ctl->xsize + vx] = c;
				}
			}
		}
	}
	return;
}

/// @brief MARK: 移动图层(只要控制好左上角那个点就行了)
void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0)
{
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { /* �������\�����Ȃ� */
		sheet_refresh(ctl); /* �V�����������̏��ɉ����ĉ�ʂ�`������ */
	}
	return;
}

/// @brief MARK: 释放图层
/// @param ctl 
/// @param sht 
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) {
		sheet_updown(ctl, sht, -1); /*如果处于显示状态，则先设定为隐藏 */
	}
	sht->flags = 0; /* "未使用"标志 */
	return;
}
