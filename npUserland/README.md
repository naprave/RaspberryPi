# npUserland

== What? ==

npUserland is a script that sets up the basic environment on the RaspberryPi when using the npStarter init system. 
After the kernel boots and npStarter init system starts, this script sets up:
- wifi (client/ap)
- dhcp server (needs dhcpcd)
- vncserver (needs vnc4server)
- jack
- memcache

And starts the default application by calling /opt/naprave/app/start.php.

== Why? ==

Although you want a fairly clean system, some basic configuration is necessary. 
This scripts is meant for generic configuration while still getting options from the application.

== Requirements ==

- working raspbian system, booted with npStarter
- php
Optionally:
- dhcpcd
- vnc4
- manually compiled jack, without dbus
- memcache

== Installation ==

First become root:
sudo su -

Clone it to /root/npUserland (path is hardcoded currently), add a symlink of /root/npUserland/start.php to /root/start.php and run npStarter to use it.

== Usage ==

Set the default options in start.php, or in /opt/naprave/app/options.php (if you have it).
