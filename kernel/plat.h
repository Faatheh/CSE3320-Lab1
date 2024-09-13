#ifdef PLAT_VIRT
#include "plat-virt.h"
#elif defined PLAT_RPI3QEMU || defined PLAT_RPI3
#include "plat-rpi3qemu.h"
#else
#error "unimpl"
#endif

#define PAGE_SHIFT	 	    12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)	
#define SECTION_SIZE		(1 << SECTION_SHIFT)	
