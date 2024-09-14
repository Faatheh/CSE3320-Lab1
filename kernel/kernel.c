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
extern void test_fb(); 
extern void test_sound(); 
extern void test_sd(); 
extern void test_kernel_tasks();
extern void test_spinlock();
extern void test_sem();
extern void test_sf();
extern void donut(); 	//donut.c
extern void donut_uart(); 	//donut.c

void uart_send_string(char* str);

// test_ktimer(); while (1);
// test_sys_sleep(); while (1); 
// test_malloc(); while (1); 
// test_usb_kb(); while (1); 
// test_usb_storage(); while (1); 
// test_fb(); while (1); 
// test_sound(); while (1); 
// test_sd(); while (1); 	// works for both rpi3 hw & qemu
// test_spinlock(); while (1);
// test_kernel_tasks(); while (1);
// while (1) {test_sem();} while (1);	
// dump_mem_info(); test_sf(); while (1);
// show_stack(myproc(), "");

struct cpu cpus[NCPU]; 

void kernel_main() {
	uart_init();
	init_printf(NULL, putc);	
	printf("------ kernel boot ------  core %d\n\r", cpuid());
	printf("build time (kernel.c) %s %s\n", __DATE__, __TIME__); // simplicity 
			
	sys_timer_init(); 		// kernel timer: delay, timekeeping...
	enable_interrupt_controller(0/*coreid*/);
	enable_irq();
	
	generic_timer_init();  // periodic ticks alive

	if (fb_init()!=0) BUG();  // will show the OS logo

	// test_ktimer();		// useful. passed
	// test_fb(); 		// cycle through color quads
	// donut();
	donut_uart();

    while (1)
        asm volatile("wfi"); // what happen here?
	// project idea: measure cpu util
}