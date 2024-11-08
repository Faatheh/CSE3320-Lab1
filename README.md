# UVA-OS WORLD1

<img src="docs/donut-pixel.gif" alt="description" width="200">

<img src="docs/image-1.png" alt="description" width="500">


For quests, see [docs/quests-lab1.md](docs/quests-lab1.md)

## Build for rpi3 QEMU

To clean up:
```sh
./cleanall.sh
```

(Optional) To configure the kernel, change `kernel/param.h`.

To build everything:
```sh
export PLAT=rpi3qemu
./makeall.sh
```

To run:
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
export PLAT=rpi
./makeall.sh
```

To copy the kernel image to `d:/trampframe`:
```sh
cp kernel8-rpi3.img /mnt/d/tmp/kernel8-rpi3.img
```

Insert SD card, copy to SD card:
```sh
copy "d:\tmp\kernel8-rpi3.img" f:\kernel8-rpi3.img
```

Faster way:
Copy `9999-download.bat` and `delay-exit.bat` to Windows desktop, then double-click `9999-download.bat`.
