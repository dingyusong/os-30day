/* FIFO���C�u���� */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
/* FIFO�o�b�t�@�̏����� */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* �� */
	fifo->flags = 0;
	fifo->p = 0; /* �������݈ʒu */
	fifo->q = 0; /* �ǂݍ��݈ʒu */
	fifo->task = task; /* MARK：有数据写入时需要唤醒的任务 */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
/* FIFO�փf�[�^�𑗂荞��Œ~���� */
{
	if (fifo->free == 0) {
		/* �󂫂��Ȃ��Ă��ӂꂽ */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {	//MARK: 任务唤醒 有任务了
		if (fifo->task->flags != 2) { /* 如果任务处于休眠状态 */
			task_run(fifo->task); /* 将任务唤醒 */
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
/* FIFO����f�[�^����Ƃ��Ă��� */
{
	int data;
	if (fifo->free == fifo->size) {
		/* �o�b�t�@������ۂ̂Ƃ��́A�Ƃ肠����-1���Ԃ���� */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
/* �ǂ̂��炢�f�[�^�����܂��Ă��邩��񍐂��� */
{
	return fifo->size - fifo->free;
}
