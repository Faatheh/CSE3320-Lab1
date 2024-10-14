---
title: "Quests of Kernel Lab1"
author: "Felix Lin"
date: "Oct 2024"
layout: "post"
---

# Quests of Kernel Lab1

Total estimated time: XXX hours

## 🗺️ Quest0: setup & verify 

| Your local machine: | Install...    |
| ------------------------ | ---------------------- |
| Windows                  | WSL2 (cannot be WSL)      |
| Mac                      | VMware Player Workstation  |
| Linux                    | Ubuntu 22.04 native or VMware Player Workstation              |

* We only support Ubuntu 22.04. 

* Other OS/compiler versions are possible but unsupported. 

* In writing below, we will refer to users using VMware Player as "VM users". 

### install toolchain

VM users: skip the installation part. Toolchains are already installed. 

````bash
sudo apt update
sudo apt install gcc-9-aarch64-linux-gnu
sudo apt install gdb-multiarch
# other useful goodies
sudo apt install tmux build-essential
````

NB: Must be gcc-9. Newer gcc is known to break some kernel code. 

Verify:
````
aarch64-linux-gnu-gcc-9 --version

aarch64-linux-gnu-gcc-9 (Ubuntu 9.5.0-1ubuntu1~22.04) 9.5.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
````

### install gdb
VM users: skip the installation part

````
sudo apt install gdb-multiarch
````

Install the "dashboard" enhancement

````
wget -P ~ https://raw.githubusercontent.com/fxlin/gdb-dashboard-aarch64/master/.gdbinit
````

### install qemu 

VM users: skip the installation part

````
sudo apt install qemu-system-aarch64
````


Verify: 
````
qemu-system-aarch64 --version

QEMU emulator version 6.2.0 (Debian 1:6.2+dfsg-2ubuntu6.18)
Copyright (c) 2003-2021 Fabrice Bellard and the QEMU Project developers
````

### test toolchain & qemu 

Test QEMU with Rpi3 baremetal code (NOTE: this repo is for validating your
toolchain & QEMU build; it is NOT our course project)

````
git clone https://github.com/fxlin/raspi3-tutorial.git
cd raspi3-tutorial
git checkout b026449
cd 05_uart0
make 
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
````

Expected output: 
````
My serial number is: 0000000000000000
````

>  Note: the test program runs an infinite loop which will cause high CPU usage
>  on your host machine. Kill the test program timely. 


### ✨SideQuest1.1: setup vscode

install vscode on your local machine. 

| User type | Install vscode ...   |
| ------------------------ | ---------------------- |
| WSL2                  | on Windows; use vscode to connect to the WSL target       |
| Linux native           | nativelly  |
| VM                    |  inside the VM |

recommended vs plugins: 

- Arm Assembly
- Bookmarks (for navigating in large codebases)
- C/C++ Themes
- Project Manager (for managing multiple codebases) 
- Remote Development (Windows WSL target)
- Remote Explorer
- WSL (Windows only)
- C/C++
- GitHub Copilot
- Todo Tree (highlight todo keywords) 

useful hotkeys: 

- ctrl+p go to files
- ctrl+t go to a symbol
- ctrl+shift+f search text in all files 
- ctrl+\ open the current file in a split view
- ctrl+j toggle bottom panel
- ctrl+b toggle left panel

## 🗺️ Quest1: examine the kernel image
clone this git repo. Assume the path to be under your home path,
`~/kernel-lab1/`. 

````
export PLAT=rpi3qemu
./makeall.sh
````

Verify: the output kernel (elf): 
````
file kernel/build-rpi3qemu/kernel8.elf
````

Inspect the kernel elf file. Refer to our short [writeup](dump.md) or ask
ChatGPT. 

Make sure you understand the differnce between .elf and .img files

Answer the questions below: 

- What are these sections in the elf file?

- What is the address of symbol kernel_main? What are the first 8 bytes at the
    symbol? What are the corresponding instructions? 

- How many bytes does each aarch64 instruction contain? 

- How is kernel8.img generated out of kernel8.elf?

- Now examine kernel8.img (use the hexdump command or the VSCode plugin). Search
    for the first 8 bytes of kernel_main(). Can you find it? At which offset of
    kernel8.img? 


## 🗺️ Quest2: bringup of the boot process

OVERVIEW: complete boot.S so that the kernel boots to kernel_main() in kernel.c

### Launch GDB

Briefly:

In a window: 
````
./dbg-rpi3qemu.sh
````

In a different window: 

````
gdb-multiarch
````

which shall read ~/.gdbinit and loads `kernel/build-rpi3qemu/kernel8.elf`
because that is what is inside .gdbinit. To override that path, do 
````
gdb-multiarch mypath/kernel8.elf
````

More info on GDB (common commands, etc): refer to our short [writeup](gdb.md) or
ask ChatGPT. 

### Try single step the kernel 

With GDB, start from the kernel `_start`, single step (per instruction). Compare
the instructions executed to `boot.S`.  

Until you execute `eret`. Have you observed change in the EL displayed by GDB? 

✔️ CHECKPOINT: Take a screenshot of GDB (picture). 

### Coding: complete boot.S

**NOTE**. In the writeup below, we will refer to C function names and assembly
labels. We will not always give out the file names. To quickly locate them, use
vscode `ctrl+t` for C functions and `ctrl+shift+f` for assembly labels.

**NOTE**. In all coding exercise below, see the comments in the code for hints
and instructions.

- complete the code at `el1_entry` (as instructed by the code comments)

- complete the code at `setup_sp` (as instructed by the code comments)

- set a breakpoint at the first printf() call in kernel_main(). run the kernel
until the breakpoint is hit. 

✔️ CHECKPOINT: take a screenshot of GDB. 

## 🗺️ Quest3: bringup of UART (polling only)

complete `uart_send()` (as instructed by the code comments). 

in `kernel_main()`, before the first call to `printf()`, call `uart_init()` and
`init_printf()`, as instructed by the code comments. 

✔️ CHECKPOINT: take a screenshot of kernel printing: 
````
------ kernel boot ----- core 0
build time (kernel.c) ...
````

## 🗺️ Quest4: bringup of sys_timer (needed for timed animation)

- complete the core function of kernel timekeeping: `current_counter()`, which
returns the current value of the system timer. 

- complete the delay functions (through busy waiting): `delay_ms()` and
`delay_us()`.

- read `donut_text()` and roughly understand what it does. 

- in `kernel_main()`, call `donut_text()` see the timed animation on uart
    output. 

✔️ CHECKPOINT: take a screenshot of the text donut.

### ✨SideQuest4.1: change luminance of Donut 

✔️ CHECKPOINT: take a screenshot of the donuts with different luminance.

## 🗺️ Quest5: bringup of framebuffer (fb) 

- complete the framebuffer initialization function `do_fb_init()`.

- complete the function that displays the OS logo and name: `fb_showpicture()`.

✔️ CHECKPOINT: take a screenshot of the OS logo and name displayed on the
screen.

### ✨SideQuest5.1: change kernel debug level

- in mbox.c, toggle the KERNEL DEBUG macro to show different sets of debug
    messages.

- in Makefile, toggle CONFIG_GLOBAL_DEBUG_LEVEL to control different sets of
    debug messages.

- Read the code comments, understand how these two work together to control the
    debug messages.

### ✨SideQuest5.2: change the OS logo and name

✔️ CHECKPOINT: take a screenshot of the new OS logo and name displayed on the
screen.

### ✨SideQuest5.3: explore the framebuffer virtual offsets

- try out `test_fb_voffset()` in (unittest.c). Read the code and understand what
    it does.

- understand what the virtual offsets are, and why the code does not work as
    expected.

- find a fix (without modifying QEMU) so that the test works as expected.

✔️ CHECKPOINT: using a smarpthone, shot a short video (5-10sec)

## 🗺️ Quest6: bringup of sys_timer interrupts (irq) 

- understand the table of exception vectors (`vectors:` in `entry.S`).

- in the vector table, insert `el1_irq` at the right place.

- in entry.S, complete the assembly macro `kernel_entry`, with the help of GDB and ChatGPT.

- Understand the key functions of the Donut: 
`donut_simple()`, `sys_timer_irq_simple()` and `draw_frame()` (donut.c).

- Place a call to `donut_simple()` in `kernel_main()`.

- with the help of GDB or debug print, verify that the timer irq is fired:
    i.e. `handle_irq()` (irq.c) is called. Understand why is `handle_irq()` called just one-shot or periodically. 

## 🗺️ Quest7: bringup of Donut (the pixel flavor)

- complete the function `sys_timer_irq_simple()` (donut.c).

- in `handle_irq()` (irq.c), place the call to `sys_timer_irq_simple()` (donut.c).

✔️ CHECKPOINT: using a smarpthone, shot a short video (5-10sec) of the donut animation.

### ✨SideQuest7.1: change the donut's color tone to your like 

- by modifying `draw_frame()` (donut.c), change the color tone of the donut.

✔️ CHECKPOINT: using a smarpthone, shot a short video (5-10sec) of the donut animation.

### ✨SideQuest7.2: bringup of virtual timers

- complete `sys_timer_irq()` and `adjust_sys_timer()` (timer.c).

- test the virtual timers with `test_ktimer()` (unittest.c); place a call to
    `test_ktimer()` in `kernel_main()`.

- in `handle_irq()`, replace the call to `sys_timer_irq_simple()` with
    `sys_timer_irq()`, which is a generic timer irq handler.

- understand `donut()` (donut.c) and how it uses the virtual timers. Place a call to `donut()` in `kernel_main()`.

- see the donut animation with the virtual timers.

✔️ CHECKPOINT: using a smarpthone, shot a short video (5-10sec) of the donut animation.

### ✨SideQuest7.3: bringup UART receive interrupt (rx irq)

- understand what `test_ktimer2()` (unittest.c) does.

- complete the function `uart_init()` (mini_uart.c) to enable the UART receive
    interrupt.

- complete the function `uart_irq()` (mini_uart.c) to handle the UART receive
    interrupt. Place a call to `test_ktimer2()` in `uart_irq()`.

- call `test_ktimer2()` in `kernel_main()`.

- Try: use terminal keystroke to start/kill periodic kernel messages as driven by virtual timers.

✔️ CHECKPOINT: using a smarpthone, shot a short video (5-10sec)

### ✨SideQuest7.4: reproduce all above CHECKPOINTS on a real Rpi3

✔️ CHECKPOINT: using a smarpthone, shot short videos (5-10sec each) of the real Rpi3 output. 

