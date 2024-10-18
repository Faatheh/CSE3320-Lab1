// #define K2_DEBUG_VERBOSE
#define K2_DEBUG_WARN

#include <stddef.h>
#include <stdint.h>

#include "plat.h"
#include "utils.h"
#include "sched.h"

// unittests.c
extern void test_ktimer(); 
extern void test_sys_sleep();
extern void test_malloc(); 
extern void test_mbox(); 
extern void test_usb_kb(); 
extern void test_usb_storage(); 
extern void test_fb_voffset(); 
extern void test_sound(); 
extern void test_sd(); 
extern void test_kernel_tasks();
extern void test_spinlock();
extern void test_sem();
extern void test_sf();
extern void donut(); 	//donut.c
extern void donut_simple(); 	//donut.c
extern void donut_text(); 	//donut.c

void uart_send_string(char* str);

struct cpu cpus[NCPU]; 

void kernel_main() {
	uart_init();                        // quest: make this call 
	init_printf(NULL, putc);            // quest: make this call (only ask for arg2)
	printf("------ kernel boot ------  core %d\n\r", cpuid());
	printf("build time (kernel.c) %s %s\n", __DATE__, __TIME__); // simplicity 

	sys_timer_init();                   // kernel timer: delay, timekeeping...
	enable_interrupt_controller(0);     // coreid
	enable_irq();

	generic_timer_init();               // periodic ticks alive

	if (fb_init() != 0) BUG();          // will show the OS logo

	// test_ktimer();                   // useful. passed
	// test_fb_voffset();               // cycle through color quads
	// donut();
	// donut_simple();
	donut_text();

	while (1)
		asm volatile("wfi");            // what happen here?
	// project idea: measure cpu util
}