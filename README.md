![PicoBoot](/assets/PicoBoot.png)

# PicoBoot

This is a long awaited IPL replacement modchip for Nintendo GameCube. It's open source, cheap and easy to install.

## Features
* it's open source
* uses $4 Raspberry Pi Pico board
* very easy installation, only 5 wires to solder
* programmable via USB cable, without any drivers and programs
* automatically boots any DOL app of your choice
* uses "IPL injection" approach superior to mods like XenoGC

## Installation guide

## Flashing Raspberry Pi Pico board

Go to [Releases page](https://github.com/webhdx/PicoBoot/releases) and download `picoboot.uf2` file from the latest release available. Now connect Raspberry Pi Pico board to your computer while holding `BOOTSEL` button pressed. If it's been done correctly you'll see `RPI-RP2` mass storage device show up. Drag and drop `picoboot.uf2` file to that device. It'll automatically eject and green LED will light up if it was programmed correctly. Unplug USB cable from Pico and proceed with hardware installation.

## Hardware diagrams

* [Installation diagram for DOL-001](assets/Installation%20Diagram%20(DOL-001).jpg)
* [Installation diagram for DOL-101](assets/Installation%20Diagram%20(DOL-101).jpg)


## FAQ

### I don't understand how it's better than XenoGC

XenoGC is a drive modchip, it can only patch disc data on the fly. This means you have to use a boot disk to run Swiss and play games from an SD card. PicoBoot uses completely different approach - injects custom payload during console boot sequence. This means it can load any application instead of a built in GameCube menu. It will work even if your disc drive is not working.

### I installed your modchip and now my console doesn't work

Sorry. I do not take reponsibility for any damage done by installing this modchip. Do it at your own risk!

### Can I use other RP2040 boards?

Yes, go for it. Just respect the [license agreements](LICENSE) and don't expect me to provide any support for your board. PicoBoot only supports official Raspberry Pi Pico module at the moment.

### Will it work if I have XenoGC installed?

Yes, you can use it with XenoGC intalled.

### I appreciate your work. Can I support you in any way?

This project is free and available for everyone. If you want to support it anyway, consider using Sponsor button in the top. You can also buy some of my other mods from my Tindie store.

## Compiling firmware

Make sure your Raspberry Pi Pico environment is set up on your machine.

Build Makefile and all required build scripts:
```shell
# cmake .
```

Then grab any DOL file you'd like to boot and run processing script:
```shell
# ./process_ipl.py iplboot.dol ipl.h
```

Do not change `ipl.h` output file name.

Once it's ready and `ipl.h` file has been created you can build the firmware:

```shell
# make
```

If everything worked you should see new file `picoboot.uf2` created in the main project directory. Now hold `BOOTSEL` button on Raspberry Pi Pico and connect USB cable. New mass storage device will appear. Copy `picoboot.uf2` file to `RPI-RP2` device. Once it's done it'll automatically eject itself. Disconnect the cable and you're all done.

## Hall of Fame

I'd like to thank people who helped making PicoBoot possible:
* #gc-forever crew: [Extrems](https://github.com/Extrems), [novenary](https://github.com/9ary), [emu_kidid](https://github.com/emukidid) and others 
* [tmbinc](https://github.com/tmbinc) - he started it all 🙏 
* happy_bunny - I started my research with his great writeup on [Shuriken Attack](https://www.retro-system.com/shuriken_attack.htm)
* beta testers: [seewood](https://github.com/seewood), [MethodOrMadness](https://github.com/MethodOrMadness), bela lugosi, 
* people who sponsored this project
* every PicoBoot enjoyer - it's all about you after all 😉

## Acknowledgements

* Some parts of this project use GPL-2.0 licensed code from:
  * https://github.com/redolution/iplboot
