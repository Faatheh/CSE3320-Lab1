# Task: setup & verify 
ACHIEVEMENT: text output from QEMU
	Windows user: WSL2, Mac: VM + Ubuntu 
Estimated time: XXX hours

| Your local machine: | Install...    |
| ------------------------ | ---------------------- |
| Windows                  | WSL2 (cannot be WSL)      |
| Mac                      | VMware Player Workstation  |
| Linux                    | Ubuntu 22.04 native or VMware Player Workstation              |

* We only support Ubuntu 22.04. 

* Other OS/compiler versions are possible but unsupported. 

* In writing below, we will refer to users using VMware Player as "VM users". 

## install toolchain

VM users: skip the installation part. Toolchains are already installed. 

```
sudo apt update
sudo apt install gcc-9-aarch64-linux-gnu
sudo apt install gdb-multiarch
# other useful goodies
sudo apt install tmux build-essential
```

NB: Must be gcc-9. Newer gcc is known to break some kernel code. 

Verify:
```
aarch64-linux-gnu-gcc-9 --version

aarch64-linux-gnu-gcc-9 (Ubuntu 9.5.0-1ubuntu1~22.04) 9.5.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## install gdb
VM users: skip the installation part

```
sudo apt install gdb-multiarch
```

Install the "dashboard" enhancement

```
wget -P ~ https://raw.githubusercontent.com/fxlin/gdb-dashboard-aarch64/master/.gdbinit
```

## install qemu 

VM users: skip the installation part

```
sudo apt install qemu-system-aarch64
```


Verify: 
```
qemu-system-aarch64 --version

QEMU emulator version 6.2.0 (Debian 1:6.2+dfsg-2ubuntu6.18)
Copyright (c) 2003-2021 Fabrice Bellard and the QEMU Project developers
```

## test toolchain & qemu 

Test QEMU with Rpi3 baremetal code (NOTE: this repo is for validating your toolchain & QEMU build; it is NOT our course project)

```
git clone https://github.com/fxlin/raspi3-tutorial.git
cd raspi3-tutorial
git checkout b026449
cd 05_uart0
make 
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
```

Expected output: 
```
My serial number is: 0000000000000000
```

>  Note: the test program runs an infinite loop which will cause high CPU usage on your host machine. Kill the test program timely. 


# SideTask: setup vscode

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

# Task: examine the kernel image
clone this git repo. Assume the path to be under your home path, `~/kernel-lab1/`. 

```
export PLAT=rpi3qemu
./makeall.sh
```

Verify: the output kernel (elf): 
```
file kernel/build-rpi3qemu/kernel8.elf
```

Inspect the kernel elf file. Refer to our short [writeup](dump.md) or ask ChatGPT. 

Make sure you understand the differnce between .elf and .img files

Answer the questions below: 

- What are these sections in the elf file?

- What is the address of symbol kernel_main? What are the first 8 bytes at the symbol? What are the corresponding instructions? 

- How many bytes does each aarch64 instruction contain? 

- How is kernel8.img generated out of kernel8.elf?

- Now examine kernel8.img (use the hexdump command or the VSCode plugin). Search for the first 8 bytes of kernel_main(). Can you find it? At which offset of kernel8.img? 


# Task: bringup of the boot process

OVERVIEW: complete boot.S so that the kernel boots to kernel_main() in kernel.c

## Launch GDB

Briefly:

In a window: 
```
./dbg-rpi3qemu.sh
```

In a different window: 

```
gdb-multiarch
```

which shall read ~/.gdbinit and loads `kernel/build-rpi3qemu/kernel8.elf` because that is what is inside .gdbinit. 
To override that path, do 
```
gdb-multiarch mypath/kernel8.elf
```

More info on GDB (common commands, etc): refer to our short [writeup](gdb.md) or ask ChatGPT. 

## Try single step the kernel 

With GDB, start from the kernel `_start`, single step (per instruction). 
Compare the instructions executed to `boot.S`.  

Until you execute `eret`. Have you observed change in the EL displayed by GDB? 

ACHIEVEMENT: Take a screenshot of GDB (picture). 

## Coding: complete boot.S

**NOTE**. In the writeup below, we will refer to C function names and assembly labels. We will not always give out the file names. 
To quickly locate them, use vscode `ctrl+t` for C functions and `ctrl+shift+f` for assembly labels.

- complete the code at `el1_entry` (as instructed by the code comments)

- complete the code at `setup_sp` (as instructed by the code comments)

- set a breakpoint at the first printf() call in kernel_main(). run the kernel
until the breakpoint is hit. 

ACHIEVEMENT: take a screenshot of GDB. 

# Task: bringup of UART (polling only)

complete `uart_send()` (as instructed by the code comments). 

in `kernel_main()`, before the first call to `printf()`, call `uart_init()` and `init_printf()`, as instructed by the code comments. 

ACHIEVEMENT: take a screenshot of kernel printing: 
```
------ kernel boot ----- core 0
build time (kernel.c) ...
```

# Task: bringup of sys_timer


# SideTask: change luminance of the donuts 

ACHIEVEMENT: take a screenshot of the donuts with different luminance.