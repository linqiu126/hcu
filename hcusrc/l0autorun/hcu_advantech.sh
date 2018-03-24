#!/bin/sh
### BEGIN INIT INFO
# Provides:          hcu
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: Start/stop hcu
# Description:       
#                    ZJL: 2018/3/24 Add Node-JS scripts auto starting method
#                    Target is to merge all scripts into one combined file
#                    Raspy scripts not yet re-use the same one.
# 存放方式:           
#                    将该文件改名为hcu.sh，放置替换到/etc/init.d/hcu.sh
# CRON设置方式：
#  
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
	#判定hcu是否运行
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
			#unzip hcu_new
			unzip /var/hcu/hcu_new -d /var/hcu/temp
			
			#hcu program part
			if [ -f "/var/hcu/temp/hcu" ]; then
				rm /var/hcu/hcu
				cp /var/hcu/temp/hcu /var/hcu/hcu
				chmod 777 /var/hcu/hcu
				echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu upgraded."
			fi
			
			#localui part
			if [ -d "/var/hcu/temp/localui" ]; then
			  rm -r /var/www/html/localui
			  cp -r /var/hcu/temp/localui /var/www/html/localui
			  echo "$(date +%Y-%m-%d_%H:%M:%S)  localui upgraded."
			fi
			
			rm -r /var/hcu/temp
			rm /var/hcu/hcu_new
			echo "$(date +%Y-%m-%d_%H:%M:%S)  directory /var/hcu/temp removed."		
		
			#先停止hcu服务
			echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu_new exits."
			#sudo systemctl stop hcu
			#echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service stopped."
			#sudo rm /var/hcu/hcu
			#sudo mv /var/hcu/hcu_new /var/hcu/hcu
			#sudo chmod 777 /var/hcu/hcu
			#sudo systemctl start hcu
			#echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service started."
		fi  
		#升级完成
		
		#升级upgrade sql file
		if [ -f "/var/hcu/hcu_new.sql" ]; then
			mysql -uroot -p123456 hcudb < /var/hcu/hcu_new.sql
			echo "$(date +%Y-%m-%d_%H:%M:%S)  Data from /var/hcu/hcu_new.sql inserted."
			sudo rm /var/hcu/hcu_new.sql
		fi
		
		#启动MQTT服务
		sudo cd /var/www/html/localui
		sleep 1
		node mqttserver.js &
		sleep 1
		node client_uirouter.js &
	
		#密码
		#echo 123456 | sudo -S /var/hcu/hcu > /var/hcu/hcu.log &
		#echo 123456 | sudo -S /var/hcu/hcu &
		
		#正式启动hcu进程
		sudo -S /var/hcu/hcu &
		#sleep 5
		
		#自动启动界面：这里启动没成功，换做浏览器自动启动方式
    	#chromium-browser --app=http://localhost/bfscui --start-fullscreen --no-sandbox &
	fi
	#循环结束
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
