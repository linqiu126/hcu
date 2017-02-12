#!/bin/sh
### BEGIN INIT INFO
# Provides:          hcu
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: Start/stop hcu
# Description:       ...
### END INIT INFO

set -x

#export PATH=/home/forlinx:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/lib
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib/x86_64-linux-gnu:/home/test/ffmpeg_build/lib
#export LD_LIBRARY_PATH=/lib:/usr/local/mysql_arm/lib:/usr/local/ncurses5_arm/lib:/usr/local/arm/linux_arm_a8/lib:/usr/local/arm/linux_arm_a8_curl/lib:/usr/local/pcre_arm/lib:/usr/local/zlib_arm:/usr/local/libpng_arm/lib:/root/ffmpeg_build_arm/lib:$LD_LIBRARY_PATH
ulimit -s unlimited

# Start WIFI card
#wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf &


#
# Function that starts the daemon/service
#
do_start()
{

        # if pgrep "hcu" > /dev/null  ; then

	if [ -f /home/pi/hcu_sw_download/hcu ] ; then
            echo "hcu downloaded to home/pi/hcu_sw_download, replace old hcu ..."

	    /bin/cp -f /home/pi/hcu_sw_active/hcu /home/pi/hcu_sw_backup
            echo "/bin/cp -f /home/pi/hcu_sw_active/hcu /home/pi/hcu_sw_backup ..."

            /bin/cp -f /home/pi/hcu_sw_download/hcu /home/pi/hcu_sw_active
            echo "/bin/cp -f /home/pi/hcu_sw_download/hcu /home/pi/hcu_sw_active ..."

            /bin/rm -fr /home/pi/hcu_sw_download/hcu
            echo "/bin/rm -fr /home/pi/hcu_sw_download/hcu ..."
        fi

	chmod 777 /dev/video0
	chmod 777 /dev/tty*

        # if pgrep "hcu" > /dev/null  ; then
	if pidof "hcu" > /dev/null ; then
		echo "hcu is running"
		exit 1
	else
		echo "hcu not running" 

		/bin/rm -fr /home/pi/hcu.log.old
		/bin/rm -fr /home/pi/hcuerr.log.old	
		/bin/mv -f /home/pi/hcu.log /home/pi/hcu.log.old	
		/bin/mv -f /home/pi/hcuerr.log /home/pi/hcuerr.log.old	
                sleep 10
		/home/pi/hcu_sw_active/hcu > /home/pi/hcu.log &
	fi

        sleep 3
	if pidof "hcu" > /dev/null ; then
		echo "hcu is started ..."		
		exit 1
	else
	    /bin/cp -f /home/pi/hcu_sw_backup/hcu /home/pi/hcu_sw_active
            echo "/bin/cp -f /home/pi/hcu_sw_backup/hcu /home/pi/hcu_sw_active ..."
            #/bin/rm -fr /home/pi/hcu.log.old
	    #/bin/rm -fr /home/pi/hcuerr.log.old	
	    /bin/mv -f /home/pi/hcu.log /home/pi/hcu.log.old
	    /bin/mv -f /home/pi/hcuerr.log /home/pi/hcuerr.log.old
	    /home/pi/hcu_sw_active/hcu > /home/pi/hcu.log &
	    echo "rollback to backup hcu, hcu started ..."	    
        fi
}

#
# Function that starts the daemon/service
#
HCU_PID = ""

do_stop()
{
	# if pgrep "hcu" > /dev/null  ; then
	if pidof "hcu" > /dev/null ; then
		echo "hcu is running ..."
		HCU_PID = $(pidof "hcu")
		kill -9 $(pidof "hcu") 
		echo "hcu killed ..."
		exit 1
	else
		echo "hcu not running, do nothing" 

	fi
}

case "$1" in
  start)
	do_start
	;;
  stop)
	do_stop
	;;
  restart)
	do_stop	
	do_start
	;;
  *)
	echo "Usage: hcu {start|stop|restart|}" >&2
	exit 3
	;;
esac
