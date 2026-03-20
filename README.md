# Boot2TargetDisplay - Boot supported iMacs straight to Target Display Mode

## Status
Works on at least my 2010 iMac. Should work on other DisplayPort-based iMacs but **will not work** on Macs with
Thunderbolt ports as those are far more complicated.

## How it works
This is a tiny EFI executable that pokes at the Apple System Management Controller to enable Target Display Mode. This
can be placed on a USB flash drive (formatted as FAT32 and placed at `\EFI\BOOT\BOOTX64.EFI`) or on the hard drive and
will work regardless of what version of macOS is on the iMac.

## Known issues
### Thunderbolt iMacs
These do not work **at all** with this iteration of Boot2TargetDisplay and appear to be much more complicated in their
design.

### Other issues
* The CPU is pinned at 100% when this is running, which can be solved by hooking this up to EFI timers
* The "waiting for connection" display does not reappear if the cable is disconnected, but Target Display Mode will
  spring back into life when it is reconnected/when there is a signal no problem.

## Compiling
You need GNU make, Clang, and lld-link, once you have those just type `make`.

## To-do
* Don't pin a CPU core while Boot2TargetDisplay is running
* ~~Display auto-detection~~ kinda, we can now hotplug the display connector, but we can't return to EFI
* Possibly put the iMac CPU to sleep to minimize heat output (not sure if this is possible, I'd
have to experiment in Linux)

## Credits
* EFI headers from [EDK2](https://github.com/TianoCore/edk2)
* Apple SMC protocol headers from [OpenCore](https://github.com/acidanthera/OpenCorePkg)
* Target Display reverse engineering based on
[this blog post from Florian Echtler](https://floe.butterbrot.org/matrix/hacking/tdm/)

## License
This is licensed under the MIT license.