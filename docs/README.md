# UVA-OS Lab1 "Baremetal" 

This is one part of the UVA-OS class (CS4414/CS6456). 

[OVERVIEW](https://github.com/fxlin/cs4414-main) |
[PROTO1](https://github.com/fxlin/uva-os-world1) |
[PROTO2](https://github.com/fxlin/uva-os-world2) |
[PROTO3](https://github.com/fxlin/uva-os-world3) |
[PROTO4](https://github.com/fxlin/uva-os-world4) |
[PROTO5](https://github.com/fxlin/uva-os-world5) 


### Students: see [quests-lab1.md](quests-lab1.md)

## GALLERY
<img src="donut-text.gif" alt="description" height="200">
<img src="donut-pixel.gif" alt="description" height="200">

## DESIGNS

A single CPU core can boot, print messages from UART, and display pixels. Interrupts work, enabling periodic rendering of a simple "donut" animation. Everything runs in privileged mode (EL1).

<img src="image-1.png" alt="description" width="500">

✅ UART/printf 
✅ Timers (&multiplexing)
✅ Interrupts
✅ Framebuffer & animation

⛔ No multitasking 
⛔ EL1 only


## Build for rpi3 QEMU

To clean up:
```sh
./cleanall.sh
```

To build everything:
```sh
export PLAT=rpi3qemu
./makeall.sh
```

To run on qemu:
```sh
./run-rpi3qemu.sh
```

## Build for rpi3 (hardware)

To clean up:
```sh
./cleanall.sh
```

To build everything:
```sh
export PLAT=rpi3
./makeall.sh
```

(One time): get a blank SD card, burn the provided image with Win32DiskImager, 
balenaEtcher, or Raspberry Pi Imager. 

Copy the kernel image `kernel8.img` to the partition named `bootfs` and boot. 