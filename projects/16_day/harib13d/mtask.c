/* �}���`�^�X�N�֌W */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}
	task = task_alloc();
	task->flags = 2; /* ���쒆�}�[�N */
	task->priority = 2; /* 0.02�b */
	taskctl->running = 1;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority); //MARK: 用优先级来描述调度的频率。
	return task;
}

struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* �g�p���}�[�N */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* �Ƃ肠����0�ɂ��Ă������Ƃɂ��� */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /* �����S���g�p�� */
}

void task_run(struct TASK *task, int priority)
{
	if (priority > 0) {
		task->priority = priority;
	}
	if (task->flags != 2) {
		task->flags = 2; /* ���쒆�}�[�N */
		taskctl->tasks[taskctl->running] = task;
		taskctl->running++;
	}
	return;
}

/// @brief 任务切换,priority越大，运行的时间越长。
/// @param  
void task_switch(void)
{
	struct TASK *task;
	taskctl->now++; //MARK: 任务切换的时候，now指向下一个任务。
	if (taskctl->now == taskctl->running) {
		taskctl->now = 0;
	}
	task = taskctl->tasks[taskctl->now];
	timer_settime(task_timer, task->priority);
	if (taskctl->running >= 2) {
		farjmp(0, task->sel);
	}
	return;
}

/// @brief 任务休眠
/// @param task 
/*
MARK: important

假设task的属性为2，表示正在运行，那么就将其从运行队列中删除，然后将其属性设置为1，表示休眠。

如果 task->flags == 1 我们不管他。
如果 task->flags == 2 ，表示其是运行中。我们就将其从运行队列中删除，然后将其属性设置为1，表示休眠。

MARK: 注意一个概念！我们主动调用sleep的时候，我们就是在主动让出CPU的使用权。所以我们要将其从运行队列中删除。
而cpu调度的话，虽然也是让出CPU的使用权，但是我们不需要将其从运行队列中删除。因为cpu调度的时候，我们还是会继续运行的。


第1种情况：running = 3 ，i = 2 ,now = 2
假设当前running = 3 ，i = 2 ,now = 2. now 可以认为是taskctl->tasks[2]的下标。

1，2, 6, 7
下一个运行的是now = 0。
| 0 | 1 | 2 		| running 	| 4 | 5 | 6 | 7 |...
| 0 | 1 | running 	| 3 		| 4 | 5 | 6 | 7 |...
并没有执行4，往前覆盖的操作。running可以认为是动态数组的长度。他控制着数组的长度。
也就是说我们不用管下标为2的task的内容，我们只需要将running减1就可以了。后面再新增task的时候，会覆盖掉下标为2的task的内容。

第2种情况：假设running是3，now=0. i = 1

| now | i | 2 | running 	| 4 | 5 | 6 | 7 |...

2, 4 
他执行了4这一步，将下标为1的task的内容覆盖了。而且当前不是正在运行，所以不用进行任务切换。

第3种情况：假设running是3，now=2. i = 0

| i | 1 | now 		| running 	| 4 | 5 | 6 | 7 |...
3, 4
now减1
task数组往前覆盖一个
而且当前不是正在运行，所以不用进行任务切换。


*/
void task_sleep(struct TASK *task)
{
	int i;
	char ts = 0;
	if (task->flags == 2) {	
		//1. 判断当前任务是否正在运行（正在获取时间片）
		if (task == taskctl->tasks[taskctl->now]) {
			ts = 1; 
		}
		/* 找到task的下标 */
		for (i = 0; i < taskctl->running; i++) {
			// 2. 找到下标i
			if (taskctl->tasks[i] == task) {
				/* 找到就退出循环 */
				break;
			}
		}
		taskctl->running--;
		if (i < taskctl->now) {
			//3. now减1
			taskctl->now--; 
		}		
		for (; i < taskctl->running; i++) {
			//4. 将后面的任务往前移动，删除task
			taskctl->tasks[i] = taskctl->tasks[i + 1];
		}
		task->flags = 1; 
		if (ts != 0) {
			//5. 如果task正在运行，那么就需要进行任务切换。
			if (taskctl->now >= taskctl->running) {
				// 6. 如果now大于等于running，那么就需要将now设置为0
				taskctl->now = 0;
			}
			//7. 进行任务切换
			farjmp(0, taskctl->tasks[taskctl->now]->sel);
		}
	}
	return;
}
