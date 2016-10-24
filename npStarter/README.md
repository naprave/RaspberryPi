# npStarter

== What? ==
npStarter is a very basic init system for the raspberrypi, created to have full control of an otherwise fully functioning Raspbian Linux system.

== Why? ==
Modern linux systems can be a bit too smart and a bit too complicated sometimes. 
For art installations and dependable systems (read-only file systems, ...) more control is needed and fewer parts means less can go wrong.
Through the read-only filesystem and predictable environment it offers a more embedded feel to the raspberry. It's also very fast:)

== Requirements ==
- working raspbian system
- cmake
- compiler (gcc/clang)
- optionally WiringPI (http://wiringpi.com/)

== Installation ==
First become root:
sudo su -

Clone it to /root/npStarter (path is hardcoded currently) and run:
cd /root/npStarter
cmake .
make -j5

Then overwrite /usr/init with /root/npStarter/init, this replaces npStarter as the init system, instead of systemd.

== Usage ==
After rebooting the system starts a barebones, readonly session and can be accessed via SSH on the ethernet port, listening via dhcp (dhclient is called when booting) and/or statically on 192.168.1.105.

When you login you can send a SIGINT signal to init (killall -SIGINT init) and it will run a systemd session, fully booting the pi.

== Device support ==
Modules are hardcoded as needed, ethernet, wifi, usb cameras and the touch display. I might make an ini file with module names so you don't have to recompile to add modules.

== WiringPI ==
You can also enable WiringPI (remove the comments in CMakeLists.txt) so the init will run a full system when two pins are connected (pin 18 and ground)
Or any other configuration change can be done via GPIO, currently just this:)

== Userland ==
I use PHP for the web server which can then run local commands remotely. 
I've also used it for the userland, which configures the network, wifi/ap and misc functions. This I will add as an additional project (npUserland)