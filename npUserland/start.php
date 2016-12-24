#!/usr/bin/php
<?php
/*
Package requirements (all optional):
- hostapd
- manually compiled jack (remove dbus support)
- vnc4server
- memcache
*/
	function log_msg($msg) {
		file_put_contents('/tmp/userland.log', "$msg\n", FILE_APPEND);
		echo "$msg\n";
	}
	log_msg("starting");
    chdir('/root/npUserland/');

    $options['wifi'] = 1;
    $options['wifiap'] = 1;
    $options['ssid'] = 'node';
    $options['password'] = 'yourpassword';
    $options['clearterm'] = 1;
    $options['vncserver'] = 1;
//    $options['jack'] = 1;
    
    @include('/opt/naprave/app/options.php');
    print_r($options);

    if( !file_exists('/tmp/userland.done') ) {
		log_msg("running userland");
		file_put_contents('/tmp/userland.done', 0);

		if( $options['clearterm'] ) {
			//doesnt really work?
			system('setterm -foreground black -background black');
			system('setterm -cursor off;');
		}

		$mac = exec('ifconfig | grep \'eth0 \'');
		$mac = substr($mac, -17);
		file_put_contents('/tmp/mac', $mac);
		$ssid_suffix = '_'.substr(str_replace(':','', $mac),-4);
		//print_r($mac); exit;
		//exit;
		
		system('date -s "`cat /etc/fake-hwclock.data`"');
		system('dhclient eth0');

		if( $options['wifi'] ) {
			log_msg("enabling wifi");
			if( $options['wifiap'] ) {
				log_msg("enabling wifi AP");
				$hostapd = file_get_contents('hostapd.conf');

				$hostapd = str_replace('SSID',$options['ssid'].$ssid_suffix, $hostapd);
				$hostapd = str_replace('PASSWORD',$options['password'], $hostapd);
				
				file_put_contents('/tmp/hostapd.conf', $hostapd);
			
				system('/sbin/ifconfig wlan0 up');
				system('/sbin/ifconfig wlan0 192.168.42.2/24');
				system('touch /tmp/dhcpd.leases');
				system('/usr/sbin/hostapd /tmp/hostapd.conf > /dev/null &');
				system('/usr/sbin/dhcpd -lf /tmp/dhcpd.leases wlan0 --no-pid > /dev/null &');
			} else {
				system('/sbin/ifconfig wlan0 up');
				system('/sbin/ifconfig wlan0 192.168.42.2/24');
			}
		} else {
			system('/sbin/ifconfig wlan0 down');
			system('/usr/sbin/rfkill block wifi');
		}
		
		if( $options['memcache'] ) {
			system('/usr/bin/memcached -u pi -d');
		}
		if( $options['jack'] ) {
			$jack_device = 'hw:Device';
			if( !empty($options['jack_device']) ) $jack_device = $options['jack_device'];
			log_msg("starting JACK... $jack_device");
		
			system('/root/jack2/build/linux/jackd -Rv -p 128 -d alsa -d'.$jack_device.' -r 48000 -H -S -n 2 -p 512 > /dev/null &');
			sleep(4);
			log_msg("done starting JACK.");
		}
		
		if( $options['vncserver'] ) {
			system('/usr/bin/Xvnc4 :1 -rfbauth /root/.vnc/passwd > /dev/null &');
			system('export DISPLAY=:1 ; xterm > /dev/null &');
		}

		file_put_contents('/tmp/userland.done', 1);

	}
    if( file_exists('/opt/naprave/app') ) {
		log_msg("Running app");
        system('/opt/naprave/app/start.php >/dev/null &');
    } else {
		log_msg("Running basic app");
        system('/opt/naprave/start.php >/dev/null &');
    }

/*
system('echo 25 > /sys/class/gpio/export');
sleep(1);
system('echo out > /sys/class/gpio/gpio25/direction');
sleep(1);
/**/
?>
