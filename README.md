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

## Video guides and overview of the features

- [PicoBoot Modchip Will Unleash The POWER of Your Nintendo GAMECUBE! | Installation Guide and Overview](https://www.youtube.com/watch?v=qwL4ZSa0xMo) by [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions)
- [This new Gamecube Modchip is a GAMECHANGER - PicoBoot](https://www.youtube.com/watch?v=lfMTLEM0yeQ) by [RockerGaming](https://www.youtube.com/c/RockerGaming)
- [$5 Gamecube IPL Modchip?! Picoboot Dol-001 + Dol-101 Installation / Setup / Showcase](https://www.youtube.com/watch?v=W_9-mSBMBJ4) by [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)

## 🛠 Installation guide

## 1. Prerequisites

* Raspberry Pi Pico ([Amazon](https://amzn.to/3JbIsMw), [AliExpress](https://s.click.aliexpress.com/e/_DkGpnTv))
* SD Gecko/WiiSD ([Amazon](https://amzn.to/3cPCEMs), [AliExpress](https://s.click.aliexpress.com/e/_Dm43n6J)) or SD2SP2 ([Amazon](https://amzn.to/3I1cCSl), [AliExpress](https://amzn.to/3PNamBe))
* FAT32 or exFAT formatted SD card

Links above are affiliate and I get small comission 💵 if you buy these products. Thanks to everyone who used links above 🙏

## 2. Flashing Raspberry Pi Pico board

Go to [the latest release page](https://github.com/webhdx/PicoBoot/releases/latest) and download `picoboot.uf2` file. Now connect Raspberry Pi Pico board to your computer while holding `BOOTSEL` button pressed. If it's been done correctly you'll see `RPI-RP2` mass storage device show up. Drag and drop `picoboot.uf2` file to that device. It'll automatically eject and green LED will light up if it was programmed correctly. Unplug USB cable from Pico and proceed with hardware installation.

## 3. Preparing SD card

Format your SD card to FAT32 or exFAT. Download the latest Swiss release from [here](https://github.com/emukidid/swiss-gc/releases/latest) and grab `swiss_rXXXX.dol` file, rename it to `ipl.dol` and copy to the root of your SD card.

## 4. Hardware installation

![Wiring diagram](assets/Wiring%20diagram.jpg)


## FAQ

### Can I use new Pico W board?

It depends. According to the reports it works but the green LED won't light up which may be confusing at first. I haven't tested it personally therfore I can't provide you with any support at this time. It's planned to fully support Pico W and WiFi functionalities in the future.

### My console doesn't start - I see a black screen.

This indicates problem with wiring. Make sure there are no unintentional shorts near RTC-DOL (U10) IC. It's recommended to use wires as short as possible. The wires can't be too thin, you need at least 26 AWG for 3.3V and GND. Optimal location for PicoBoot is between controller port board and fan assembly. Especially I do not recommend installing PicoBoot in Serial Port 1 slot - it makes wire runs too long and may stop working with future updates! Use only official diagrams, any alternative soldering points and chip locations are not guaranteed to work.

### I don't understand how it's better than XenoGC

XenoGC is a drive modchip, it can only patch disc data on the fly. This means you have to use a boot disk to run Swiss and play games from an SD card. PicoBoot uses completely different approach - injects custom payload during console boot sequence. This means it can load any application instead of a built in GameCube menu. It will work even if your disc drive is not working.

### I installed your modchip and now my console doesn't work

Sorry. I do not take reponsibility for any damage done by installing this modchip. Do it at your own risk!

### Can I use other RP2040 boards?

Yes, go for it. Just respect the [license agreements](LICENSE) and don't expect me to provide any support for your board. PicoBoot only supports official Raspberry Pi Pico module at the moment.

### Will it work if I have XenoGC installed?

Yes, you can use it with XenoGC intalled.

### I appreciate your work. Can I support you in any way?

This project is free and available for everyone. If you want to support it anyway, consider using [:heart: Sponsor](https://github.com/sponsors/webhdx) button.

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
* beta testers: [seewood](https://github.com/seewood), [MethodOrMadness](https://github.com/MethodOrMadness), [renanbianchi](https://github.com/renanbianchi)
* content creators: [MachoNachoProductions](https://www.youtube.com/c/MachoNachoProductions), [RockerGaming](https://www.youtube.com/c/RockerGaming), [ModzvilleUSA!](https://www.youtube.com/c/ModzvilleUSA)
* people who sponsored this project
* every PicoBoot enjoyer - it's all about you after all 😉

## Acknowledgements

Some parts of this project use GPL-2.0 licensed code from:
 * https://github.com/redolution/iplboot
 * https://github.com/seewood/iplboot
