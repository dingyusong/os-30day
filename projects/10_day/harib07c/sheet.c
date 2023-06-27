/* �}�E�X��E�B���h�E�̏d�ˍ��킹���� */

#include "bootpack.h"

#define SHEET_USE		1

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
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

struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* �g�p���}�[�N */
			sht->height = -1; /* ��\���� */
			return sht;
		}
	}
	return 0;	/* �S�ẴV�[�g���g�p�������� */
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

/// @brief MARK: 指定刷新区域
/// @param ctl : 图层控制器
/// @param vx0 : 刷新区域左上角x坐标
/// @param vy0 : 刷新区域左上角y坐标
/// @param vx1 : 刷新区域右下角x坐标
/// @param vy1 : 刷新区域右下角y坐标
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1)
{
	int h, bx, by, vx, vy;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		for (by = 0; by < sht->bysize; by++) {
			vy = sht->vy0 + by;
			for (bx = 0; bx < sht->bxsize; bx++) {
				vx = sht->vx0 + bx;
				//如果(vx,vy)落在了(vx0,vy0):(vx1,vy1) 这个区域里面就刷新，否则不刷新！
				if (vx0 <= vx && vx < vx1 && vy0 <= vy && vy < vy1) {
					c = buf[by * sht->bxsize + bx];
					if (c != sht->col_inv) {
						vram[vy * ctl->xsize + vx] = c;
					}
				}
			}
		}
	}
	return;
}

void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* �ݒ�O�̍������L������ */

	/* �w�肪�Ⴗ���⍂������������A�C������ */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* ������ݒ� */

	/* �ȉ��͎��sheets[]�̕��בւ� */
	if (old > height) {	/* �ȑO�����Ⴍ�Ȃ� */
		if (height >= 0) {
			/* �Ԃ̂��̂������グ�� */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* ��\���� */
			if (ctl->top > old) {
				/* ��ɂȂ��Ă�����̂����낷 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* �\�����̉������������̂ŁA��ԏ�̍��������� */
		}
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {	/* �ȑO���������Ȃ� */
		if (old >= 0) {
			/* �Ԃ̂��̂����������� */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* ��\����Ԃ���\����Ԃ� */
			/* ��ɂȂ���̂������グ�� */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* �\�����̉��������������̂ŁA��ԏ�̍����������� */
		}
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	return;
}

/// @brief MARK: 刷新指定图层的指定区域
/// @param ctl 图层控制器
/// @param sht 指定图层
/// @param bx0 指定区域左上角x坐标
/// @param by0 指定区域左上角y坐标
/// @param bx1 指定区域右下角x坐标
/// @param by1 指定区域右下角y坐标
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) { /* �������\�����Ȃ�A�V�����������̏��ɉ����ĉ�ʂ�`������ */
		sheet_refreshsub(ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}

void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { /* �������\�����Ȃ�A�V�����������̏��ɉ����ĉ�ʂ�`������ */
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}

void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) {
		sheet_updown(ctl, sht, -1); /* �\�����Ȃ�܂���\���ɂ��� */
	}
	sht->flags = 0; /* ���g�p�}�[�N */
	return;
}
