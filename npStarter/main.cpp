#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <poll.h>
#include <spawn.h>

#ifdef USEWIRING
#include <wiringPi.h>
#endif

#include <sys/mount.h>
#include <sys/stat.h>

#include <signal.h>

int i;
char cmd[255];

int bootToSystemPin = 18;

volatile int runFullSystem = 0;

static void catch_function(int signo) {
	runFullSystem = 1;
}

static void child_function(int signo) {
	wait();
}

int runInBackground(const char* cmd) {
	pid_t pid;
	pid = fork();
	if (pid == 0)
	{
        // child process
		system(cmd);
		wait();
		exit(1);
	} else if (pid < 0) {
        // fork failed
		printf("fork() failed!\n");
		return 0;
	}
	return 1;
}

void setupBasicSystem() {
 	mount("proc", "/proc", "proc", 0, NULL);
 	mount("sysfs", "/sys", "sysfs", 0, NULL);
 	mount("tmpfs", "/tmp", "tmpfs", 0, NULL);
//  	mount("tmpfs", "/run", "tmpfs", 0, NULL);
	
	mkdir("/dev/shm", 0);
 	mount("tmpfs", "/dev/shm", "tmpfs", 0, "size=128M");
//	mount("tmpfs", "/var/run", "tmpfs", 0, "size=128M");
//	mount("tmpfs", "/var/lock", "tmpfs", 0, "size=128M");
//	mount("tmpfs", "/dev/shm", "tmpfs", 0, "size=128M");
//  	system("/bin/mount -t tmpfs tmpfs /dev/shm -o size=128M");
	mkdir("/dev/pts", 0);
	
 	mount("devpts", "/dev/pts", "devpts", 0, "gid=5,mode=620");
// 	system("/bin/mount devpts /dev/pts -t devpts -o gid=5,mode=620");
// 	mount("/dev/mmcblk0p3", "/opt/naprave", "auto", 0, "defaults");
 	system("/bin/mount /dev/mmcblk0p3 /opt/naprave -o ro");
	
 	system("modprobe fuse");
 	system("modprobe snd_bcm2835");
//*	
 	system("modprobe evdev");
 	system("modprobe uio_pdrv_genirq");
 	system("modprobe ch341");
	system("modprobe snd-usb-audio");

        system("modprobe snd-seq-midi");

	system("modprobe ftdi_sio");
	system("modprobe libphy");
	system("modprobe asix");
	system("modprobe rpi_ft5406"); //screen support

	system("modprobe brcmfmac");
	system("modprobe uvcvideo");

	system("modprobe i2c-bcm2708");
	system("modprobe i2c-dev");
	system("modprobe rtc-ds1307");

	system("modprobe ipv6");

//	system("/sbin/hwclock -s");

/*/	
 	system("/bin/mount / -o remount,rw");
	system("/sbin/udevd --daemon");
 	system("/bin/mount / -o remount,ro");
/**/	

//	mount("/dev/mmcblk0p4", "/data", "auto", 0, "ro");
	system("setterm -foreground black -background black");
	system("setterm -cursor off;");
	system("clear");

//	runInBackground("/usr/bin/php -S 0.0.0.0:80 -t /opt/naprave/app/www/ > /dev/null");

//	sleep(5);
//	mount("/dev/sda1", "/data", "auto", 0, "ro");
	system("/bin/mount /dev/mmcblk0p4 /data -o ro");
	system("/bin/mount /dev/sda1 /data2 -o ro");
	system("/bin/mount /dev/sda2 /data3 -o ro");
	system("/bin/mount /dev/sda /data4 -o ro");

 	system("/usr/sbin/alsactl -L restore");
	system("amixer sset PCM 100%");
}

bool doInit;

int main ( int argc, char *argv[] )
{
	if (signal(SIGINT, catch_function) == SIG_ERR) {
		//return EXIT_FAILURE;
	}

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		//return EXIT_FAILURE;
	}

	int pid = getpid();
//	printf("pid: %d\n",pid);
	if( pid == 1 ) {
	    doInit = 1;
	} else {
		doInit = 0;
		exit(1);
	}

	if (doInit) setupBasicSystem();
	
#ifdef USEWIRING
	
	if (wiringPiSetupGpio () < 0) {
		fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
		return 1;
	}

	int pin = bootToSystemPin;
	
	pinMode (pin, INPUT);
	pullUpDnControl(pin,PUD_UP);
	
	printf("pinval: %d\n",digitalRead(pin));
#endif
	
	if (!doInit) return 0;

#ifdef USEWIRING
	if( digitalRead(pin) != 0 ) 
#endif
	{
		runInBackground("/usr/bin/php /root/start.php");

		system("ifconfig lo up");

		system("ifconfig eth0 up");
		system("ifconfig eth0:1 192.168.1.105/24");

		system("ifconfig eth1 up");
		system("ifconfig eth1:1 192.168.1.106/24");

		system("ifconfig eth2 up > /dev/null");
		system("ifconfig eth2:1 192.168.1.107/24 > /dev/null");

                system("ifconfig wlan0 up > /dev/null");
                system("ifconfig wlan0:1 192.168.42.1/24 > /dev/null");

		//printf("starting getty");
//		runInBackground("/sbin/getty --noclear 38400 tty1 > /dev/null");
		
//                runInBackground("/usr/sbin/hostapd /etc/hostapd/hostapd.conf > /dev/null");

		runInBackground("/usr/sbin/sshd -f /etc/ssh/sshd_config > /dev/null");
	}

	while( true )
	{
// 		printf("pinval: %d\n",digitalRead(pin));
#ifdef USEWIRING
		if( digitalRead(pin) == 0 || runFullSystem == 1) {
			runFullSystem = 1;
		}
#endif
		if( runFullSystem == 1) {
			execve("/lib/systemd/systemd", argv, NULL);
			return 0;
		}
		sleep(1);
	}
/**/	
} 
