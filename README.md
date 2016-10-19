# Repetier-Firmware for Nightly Board
I hope You know what I mean with nightly board...

# Current state
- Prints really well with repetier host - nothing else tested for now
- Display, keys, and printing from SD is working now. Currently no idea what to do with "move right" key. 
- No music ;-)
- Settings in eeprom are currently not enabled.
- Optimized for old plywood printers. If You have a device with stiff (metal or acrylic) frame You can set xy-acceleration to 3000 mm/s^2 and beyond
- Heated bed thermistor values are for CTC clones having a 50k ntc - and guessed. Be careful with bed temp settings!
- Bed zero position is at left and lowest (near to front). When I set it to bed center there are issues with soft endstops. I'll discuss that with repetier team when major issues are resolved
- Not yet ready for merging to origin. config.h is hardcoded and DUE port is not updated yet.
- Reverting back to sailfish is not tested yet. At minimum You will have to erase the eeprom.


Installation:
Compile with arduino tools 1.6.12 or later ( see boards manager ), set Arduino/Genuino Mega 2560, processor type ATmega 1280
I'm using [http://www.fischl.de/usbasp/](usbasp) directly connected to the main processors isp port - nothing else tested yet. Use "upload with programmer" in arduino ide.
The reset pin on by board is not connected to the usb adapter. It is nearly impossible to use the boot loader.

(some notice: the display code did not kill my hardware as denoted in an earlier version of this document.)

Some legal notice:
This is experimental software. Use at your own risk. We cannot be held liable under any circumstances for damage to hardware or software, lost data, or other direct or indirect damage resulting from the use of this software. If you do not agree to these terms and conditions, you are not permitted to use or further distribute this software.


Here the forked original descption:

# Repetier-Firmware - the fast and user friendly firmware

## Notes for developers/pull requests

This software is open source licensed under the GPL V3. As any free project we
like contributions from other sources, especially since the firmware is very
hardware related and many things can only be implemented/tested with the right
hardware. To allow easy integration of new features you should follow some simple
principals.
1. Only senad pull requests against development version. This is where we add
and test new features and bug fixes. From time to time we push these to master
as a new version.
2. Do not include your personal configuration files. If you need new configuration
options, add them to the official configuration.h file.
3. We have to folders for different processor architectures. So for most
changes modifications need to be in both folders. The general files are identical.
In fact we develop on avr and just copy them to due. Only pins.h/fastio.h/hal.*
and configuration.h are architecture dependend, so chnages there need to be made
twice and not copied.
4. Document what your pull request will change/fix/introduce. Please also mention
new configurations since we need to add them also to our online tool, so users
can set them correctly.

## Installation

Please use your configuration tool at 
[http://www.repetier.com/firmware/v092](http://www.repetier.com/firmware/v092)
or for latest development version at
[http://www.repetier.com/firmware/dev](http://www.repetier.com/firmware/dev)
for easy and fast configuration. You get the complete sources you need to compile from the online configurator.
This system also allows it to upload configurations created with this tool and modify the configuration. This is handy for updates as you get all newly introduced parameter just by uploading the old version and downloading the
latest version. New parameter are initalized with default values.

## Version 0.92.8 
* Cleaner code base.
* Pulse dense modulation for heater and fans.
* Bed bump correction for delta printer.
* Correction of parallelogram distortions.
* Decoupling test for heater and sensor for more safety.
* Mixing extruder support.
* Test for watchdog.
* Allow cold extrusion.
* Fixed pause sd print issues.
* Commands on sd stop.
* Disable heaters/extruders on sd stop.
* Safety question for sd stop.
* Many minor corrections and improvements.
* Extra motor drivers.
* Event system for lights etc.
* New homing sequence with preheat for nozzle based z sensors.
* Language selectable on runtime.
* Fix structure for Arduino 1.6.7
* New bed leveling.
* Fatal error handling added.

## Version 0.91 released 2013-12-30

WARNING: This version only compiles with older Arduino IDE 1.0.x, for
compilation with newest version use 0.92

Improvements over old code:
* Better readable code.
* Long filename support (from Glenn Kreisel).
* Animated menu changes.
* Separation of logic and hardware access to allow different processor architectures
  by changing the hardware related files.
* z-leveling support.
* Mirroring of x,y and z motor.
* Ditto printing.
* Faster and better delta printing.
* New heat manager (dead time control).
* Removed OPS handling.
* Full graphic display support.
* Many bug fixes.
* many other changes.

## Documentation

For documentation please visit [http://www.repetier.com/documentation/repetier-firmware/](http://www.repetier.com/documentation/repetier-firmware/)

## Developer

The sources are managed by the Hot-World GmbH & Co. KG
It was initially based on the Sprinter firmware from Kliment, but the code has run
through many changes since them.
Other developers:
- Glenn Kreisel (long filename support)
- Martin Croome (first delta implementation)
- John Silvia (Arduino Due port)
- sdavi (first u8glib code implementation)
- plus several small contributions from other users.

## Introduction

Repetier-Firmware is a firmware for RepRap like 3d-printer powered with
an arduino compatible controller.
This firmware is a nearly complete rewrite of the sprinter firmware by kliment
which based on Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
Some ideas were also taken from Teacup, Grbl and Marlin.

## Features

- Supports cartesian, delta and core xy/yz printers.
- RAMP acceleration support.
- Path planning for higher print speeds.
- Trajectory smoothing for smoother lines.
- Nozzle pressure control for improved print quality with RAMPS.
- Fast - 40000 Hz and more stepper frequency is possible with a 16 MHz AVR.
- Support for Arduino Due based boards allowing much faster speeds. 
- Multiple extruder supported (max. 6 extruder).
- Standard ASCII and improved binary (Repetier protocol) communication.
- Autodetect the command protocol, so it will work with any host software.
- Important parameters are stored in EEPROM and can easily be modified without
  recompilation of the firmware.
- Automatic bed leveling.
- Mixed extruder.
- Detection of heater/thermistor decoupling.
- 2 fans plus thermistor controlled fan.
- Multi-Language support, switchable at runtime.
- Stepper control is handled in an interrupt routine, leaving time for
  filling caches for next move.
- PID control for extruder/heated bed temperature.
- Interrupt based sending buffer (Arduino library normally waits for the
  recipient to receive written data)
- Small RAM memory print, resulting in large caches.
- Supports SD-cards.
- mm and inches can be used for G0/G1
- Arc support
- Dry run : Execute yout GCode without using the extruder. This way you can
  test for non-extruder related failures without actually printing.

## Controlling firmware

Also you can control the firmware with any reprap compatible host, you will only get
the full benefits with the following products, which have special code for this
firmware:

* [Repetier-Host for Windows/Linux](http://www.repetier.com/download/)
* [Repetier-Host for Mac](http://www.repetier.com/download/)
* [Repetier-Server](http://www.repetier.com/repetier-server-download/)

## Installation

For documentation and installation please visit 
[http://www.repetier.com/documentation/repetier-firmware/](http://www.repetier.com/documentation/repetier-firmware/).

## Changelog

See changelog.txt
