/* �}���`�^�X�N�֌W */

#include "bootpack.h"

struct TIMER *mt_timer;
int mt_tr;
/*
MARK: 任务切换时钟中断驱动，且与具体任务解耦了。切换的时候只需要关注mt_tr即可。也就是tr寄存器。
任务切换速度很快，不向缓冲区写入数据，对cpu的负担更小。
*/

void mt_init(void)
{
	mt_timer = timer_alloc();
	/* timer_init�͕K�v�Ȃ��̂ł��Ȃ� */
	timer_settime(mt_timer, 2);
	mt_tr = 3 * 8;
	return;
}

void mt_taskswitch(void)
{
	if (mt_tr == 3 * 8) {
		mt_tr = 4 * 8;
	} else {
		mt_tr = 3 * 8;
	}
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);
	return;
}
