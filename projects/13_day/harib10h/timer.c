/* �^�C�}�֌W */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC		1	/* �m�ۂ������ */
#define TIMER_FLAGS_USING		2	/* �^�C�}�쓮�� */

void init_pit(void)
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff; /* �ŏ��͍쓮���̃^�C�}���Ȃ��̂� */
	timerctl.using = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* ���g�p */
	}
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0; /* ������Ȃ����� */
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* ���g�p */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	timerctl.using++;
	if (timerctl.using == 1) {
		/* ���쒆�̃^�C�}�͂����ɂȂ�ꍇ */
		timerctl.t0 = timer;
		timer->next = 0; /* ���͂Ȃ� */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		/* �擪�ɓ����ꍇ */
		timerctl.t0 = timer;
		timer->next = t; /* ����t */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	/* �ǂ��ɓ����΂�������T�� */
	for (;;) {
		s = t;
		t = t->next;
		if (t == 0) {
			break; /* ��Ԃ�����ɂȂ��� */
		}
		if (timer->timeout <= t->timeout) {
			/* s��t�̊Ԃɓ����ꍇ */
			s->next = timer; /* s�̎���timer */
			timer->next = t; /* timer�̎���t */
			io_store_eflags(e);
			return;
		}
	}
	/* ��Ԃ�����ɓ����ꍇ */
	s->next = timer;
	timer->next = 0;
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	int i;
	struct TIMER *timer;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00��t������PIC�ɒʒm */
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return;
	}
	timer = timerctl.t0; /* �Ƃ肠�����擪�̔Ԓn��timer�ɑ�� */
	for (i = 0; i < timerctl.using; i++) {
		/* timers�̃^�C�}�͑S�ē��쒆�̂��̂Ȃ̂ŁAflags���m�F���Ȃ� */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* �^�C���A�E�g */
		timer->flags = TIMER_FLAGS_ALLOC;
		fifo32_put(timer->fifo, timer->data);
		timer = timer->next; /* 更新一下next指针 */
	}
	timerctl.using -= i;

	/* �V�������炵 */
	timerctl.t0 = timer;

	/* timerctl.next�̐ݒ� */
	if (timerctl.using > 0) {
		timerctl.next = timerctl.t0->timeout;
	} else {
		timerctl.next = 0xffffffff;
	}
	return;
}
