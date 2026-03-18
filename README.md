# Boot2Target - Boot supported iMacs straight to Target Display Mode

## Status
Works on at least my 2010 iMac. Display autodetection does not work, so you have to reboot the
device for it to properly detect connected displays.

## How it works
This is a tiny (<5KB) EFI executable that pokes at the Apple System Management Controller to enable
Target Display Mode. This can be placed on a USB flash drive or on the hard drive and will work
regardless of what version of macOS is on the iMac (although since Target Display Mode is an opaque
hardware feature, the usual disclaimers apply to computers connected to the iMac)

## Compiling
You need Clang and lld-link, once you have those just type `make`.

## To-do
* Don't pin a CPU core while Boot2Target is running
* ~~Display auto-detection~~ kinda, we can now hotplug the display connector but we can't return to EFI
* Possibly put the iMac CPU to sleep to minimize heat output (not sure if this is possible, I'd
have to experiment in Linux)

## Credits
* EFI headers from [EDK2](https://github.com/TianoCore/edk2)
* Apple SMC protocol headers from [OpenCore](https://github.com/acidanthera/OpenCorePkg)
* Target Display reverse engineering based on
[this blog post from Florian Echtler](https://floe.butterbrot.org/matrix/hacking/tdm/)