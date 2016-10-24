# npStarter
== What?? ==
npStarter is a very basic init system for the raspberrypi, created to have full control of an otherwise fully functioning Raspbian system.

== Installation ==

First become root:
sudo su -

Clone it to /root/npStarter and run:
cd /root/npStarter
cmake .
make -j5

Then overwrite /usr/init with /root/npStarter/init, this replaces npStarter as the init system, instead of systemd.

== Usage ==

After rebooting the system starts a barebones, readonly session and can be accessed via SSH on the ethernet port, listening via dhcp and/or statically on 192.168.1.105.

When you login you can send a SIGINT signal to init (npStarter) and it will run a systemd session, fully booting the pi.

== Device support ==

Modules are hardcoded as needed, ethernet, wifi, usb cameras and the touch display. I might make a ini file with module names so you don't have to recompile to add modules.

