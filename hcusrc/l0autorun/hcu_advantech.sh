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

#export PATH=$PATH:/var/hcu
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
	if pidof "hcu" > /dev/null ; then
		echo "hcu is running"
		exit 1
	else
		echo "hcu not running" 
		#chmod 777 /dev/video0
		#chmod 777 /dev/tty*
		/bin/rm -fr /var/hcu/hcu.log.old
		/bin/rm -fr /var/hcu/hcuerr.log.old	
		/bin/mv -f /var/hcu/hcu.log /var/hcu/hcu.log.old	
		/bin/mv -f /var/hcu/hcuerr.log /var/hcu/hcuerr.log.old	
		/sbin/insmod /lib/usbcan.ko                

		#执行hcu/DB/UI的升级
		sleep 10
		sudo cd /var/hcu
		if [ -f "/var/hcu/hcu_new" ]; then 
			#执行顺序
			#先tar -uzip
			#然后将界面目录替换性拷贝到目标 /var/www/html/bfui/
			#将hcu_new中的执行文件拷贝覆盖hcu执行程序
			#最终清理hcu_new以及tar -unzip的现场垃圾文件

			#先停止hcu服务
			echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu_new exits."
			#sudo systemctl stop hcu
			#echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service stopped."
			sudo rm /var/hcu/hcu
			echo "$(date +%Y-%m-%d_%H:%M:%S)  old hcu removed."
			sudo mv /var/hcu/hcu_new /var/hcu/hcu
			sudo chmod 777 /var/hcu/hcu
			echo "$(date +%Y-%m-%d_%H:%M:%S)  new hcu replaced."
			#sudo systemctl start hcu
			#echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service started."
		fi

		#upgrade sql file
		if [ -f "/var/hcu/hcu_new.sql" ]; then
			mysql -uroot -p123456 hcudb < /var/hcu/hcu_new.sql
			echo "$(date +%Y-%m-%d_%H:%M:%S)  Data from /var/hcu/hcu_new.sql inserted."
			sudo rm /var/hcu/hcu_new.sql
		fi

		#echo 123456 | sudo -S /var/hcu/hcu > /var/hcu/hcu.log &
		#echo 123456 | sudo -S /var/hcu/hcu &
		sudo -S /var/hcu/hcu &
		#sleep 5
    		#chromium-browser --app=http://localhost/bfscui --start-fullscreen --no-sandbox &
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
