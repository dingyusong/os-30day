/* FIFO���C�u���� */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
/* FIFO初始化 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 缓冲区大小 */
	fifo->flags = 0;
	fifo->p = 0; /* 写指针，下一个写的位置 */
	fifo->q = 0; /* 读指针，下一个读的位置 */
	return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data)
/* 往FIFO中放一个数据 */
{
	if (fifo->free == 0) {
		/* 缓冲区满了，返回异常 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo8_get(struct FIFO8 *fifo)
/* 从FIFO中获取一个数据 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 如果缓冲区为空，则返回-1 */
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

int fifo8_status(struct FIFO8 *fifo)
/* 报告下积攒了多少数据 */
{
	return fifo->size - fifo->free;
}
