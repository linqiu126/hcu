#!/bin/bash -x
PATH="/bin:/usr/bin:/usr/sbin:/sbin"
status=`sudo systemctl status hcu | grep Active: | awk '{ print $3}'`
sudo cd /var/hcu
if [ $status == "(running)" ]
	then
#	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is running." >> /var/hcu/hcu-abnormal-status.log
	exit
elif [ $status == "(exited)" ]
	then
	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is exited." >> /var/hcu/hcu-abnormal-status.log
	sudo systemctl stop hcu
	sudo systemctl start hcu
	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is restored from exited." >> /var/hcu/hcu-abnormal-status.log
else
        echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is neither running, nor exited." >> /var/hcu/hcu-abnormal-status.log
        sudo systemctl stop hcu
        sudo systemctl start hcu
	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is restored from neither running nor exited." >> /var/hcu/hcu-abnormal-status.log
fi

