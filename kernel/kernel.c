// #define K2_DEBUG_VERBOSE
#define K2_DEBUG_WARN

#include <stddef.h>
#include <stdint.h>

#include "plat.h"
#include "utils.h"
#include "sched.h"

extern void test_ktimer();     // unittests.c
extern void test_fb_voffset(); // unittests.c
extern void donut();           // donut.c
extern void donut_simple();    // donut.c
extern void donut_text();      // donut.c

void uart_send_string(char* str);

struct cpu cpus[NCPU]; 

void kernel_main() {
	// quest: UART. call uart_init() to initialize
	uart_init();                      // !STUDENT_DONOT_SEE
	// quest: UART. init printf by init_printf(NULL, XXX)
	init_printf(NULL, putc);          // !STUDENT_DONOT_SEE
	printf("------ kernel boot ------  core %d\n\r", cpuid());
	printf("build time (kernel.c) %s %s\n", __DATE__, __TIME__); // simplicity 

	sys_timer_init();                   // kernel timer: delay, timekeeping...
	enable_interrupt_controller(0);     // coreid
	// quest: sys_timer irq
	enable_irq();		// !STUDENT_DONOT_SEE

	generic_timer_init();               // periodic ticks alive

	if (fb_init() != 0) BUG();          // will show the OS logo

	// test_ktimer();
	// test_fb_voffset();               // cycle through color quads
	donut();		// !STUDENT_DONOT_SEE    uses virtual timer for animation

	// quest: pixel donut. call donut_simple()
	/* to enable it,  irq handler must be modified to call sys_timer_irq_simple() */
	// donut_simple();		// !STUDENT_DONOT_SEE		directly uses hw timer irq for animation
	
	// quest: textual donut. call donut_text()
	// donut_text();		// !STUDENT_DONOT_SEE

	while (1)
		asm volatile("wfi");            // what happen here?
}