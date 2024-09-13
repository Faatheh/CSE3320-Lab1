include $(abspath ../config.mk)

ifndef PLAT
$(error PLAT must be set to virt|rpi3qemu|rpi3. e.g. export PLAT=rpi3qemu)
endif

BUILD_DIR = build-$(PLAT)
SRC_DIR = .

ARMGNU ?= aarch64-linux-gnu
COPS = -Wall -Werror -nostdlib -nostartfiles -ffreestanding \
-I$(SRC_DIR) \
-Iaddon/usb/include\
-mgeneral-regs-only \
-g 

# the default debug level, to be overridden by less verbose levels in individual files 
COPS += -DCONFIG_KAGE_GLOBAL_DEBUG_LEVEL=10   # verbose
# COPS += -DCONFIG_KAGE_GLOBAL_DEBUG_LEVEL=30   # info
# COPS += -DCONFIG_KAGE_GLOBAL_DEBUG_LEVEL=40   # warning

# COPS += -DUSE_LFB		# uncomment this to enable GUI console. 

ASMOPS = -I$(SRC_DIR)  -g 

##### platform specific flags, targets ########
ifeq (${PLAT}, virt)
COPS += -DPLAT_VIRT
ASMOPS += -DPLAT_VIRT
LINKSCR = $(SRC_DIR)/linker-virt.ld
KERNEL = kernel8-virt.img
endif

ifeq (${PLAT}, rpi3qemu)
COPS += -DPLAT_RPI3QEMU
ASMOPS += -DPLAT_RPI3QEMU
LINKSCR = $(SRC_DIR)/linker-rpi3qemu.ld
KERNEL = kernel8-rpi3qemu.img
endif

# if SCTLR_EL1 specifies alginment check, we must specify -mstrict-align here,
# otherwise compiler will issue unaligned access -- memory exception. 
ifeq (${PLAT}, rpi3)
COPS += -DPLAT_RPI3 -mcpu=cortex-a53+fp+simd -mstrict-align
ASMOPS += -DPLAT_RPI3
LINKSCR = $(SRC_DIR)/linker-rpi3.ld
KERNEL = kernel8-rpi3.img
endif

# generic
COPS += -O2
# COPS += -O0

# for stacktrace purpose
COPS += -fno-omit-frame-pointer -Wno-frame-address 

all : $(KERNEL)

clean :
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	@echo  KERNEL CC $< "->" $@
	$(VB) mkdir -p $(@D)
	$(VB) $(ARMGNU)-gcc-9 $(COPS) -MMD -c $< -o $@

# font build rules. 
# NB: the symbols in the binary will have prefix "_binary" (added by ld)
$(BUILD_DIR)/%_psf.o: %.psf
	$(VB) mkdir -p $(@D)
	$(VB) $(ARMGNU)-ld -r -b binary -o $@ $<

# assembly rule 
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	@echo  KERNEL AS $< "->" $@
	$(VB) $(ARMGNU)-gcc-9 $(ASMOPS) -MMD -c $< -o $@

# generic kernel
C_OBJS += $(BUILD_DIR)/gic_c.o
C_OBJS += $(BUILD_DIR)/irq_c.o
C_OBJS += $(BUILD_DIR)/kernel_c.o
C_OBJS += $(BUILD_DIR)/printf_c.o
C_OBJS += $(BUILD_DIR)/string_c.o
C_OBJS += $(BUILD_DIR)/spinlock_c.o
C_OBJS += $(BUILD_DIR)/timer_c.o
C_OBJS += $(BUILD_DIR)/mbox_c.o
C_OBJS += $(BUILD_DIR)/unittests_c.o

ASM_OBJS = $(BUILD_DIR)/boot_s.o
ASM_OBJS += $(BUILD_DIR)/entry_s.o
ASM_OBJS += $(BUILD_DIR)/utils_s.o

# binary objs: font
BIN_OBJS += $(BUILD_DIR)/font_psf.o

# ------- Rpi3 QEMU specific source -------
ifeq (${PLAT}, rpi3qemu)
C_OBJS += $(BUILD_DIR)/mini_uart_c.o
endif

# ------- Rpi3 specific source -------
ifeq (${PLAT}, rpi3)
C_OBJS += $(BUILD_DIR)/mini_uart_c.o
endif

# ------- Virt QEMU specific source -------
ifeq (${PLAT}, virt)
C_OBJS += $(BUILD_DIR)/pl011_c.o
C_OBJS += $(BUILD_DIR)/virtio_disk_c.o
endif

DEP_FILES = $(C_OBJS:%.o=%.d)
-include $(DEP_FILES)

$(KERNEL): $(LINKSCR) $(C_OBJS) $(ASM_OBJS) $(BIN_OBJS)
	$(VB) echo "KERNEL LD"
	$(VB) $(ARMGNU)-ld -T $(LINKSCR) -o $(BUILD_DIR)/kernel8.elf $(C_OBJS) $(ASM_OBJS) $(BIN_OBJS)
	$(VB) $(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(KERNEL)
	$(VB) $(ARMGNU)-objdump -dS $(BUILD_DIR)/kernel8.elf > kernel8.asm
	$(VB) nm --numeric-sort $(BUILD_DIR)/kernel8.elf > kernel8.sym
	$(VB) echo KERNEL INSTALL

# easier to copy the kernel img to the sd card 
ifeq ($(PLAT),rpi3) 	
	cp $(KERNEL) /mnt/d/tmp/$(KERNEL)
endif	