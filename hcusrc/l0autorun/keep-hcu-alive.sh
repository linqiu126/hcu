#!/bin/bash -x
PATH="/bin:/usr/bin:/usr/sbin:/sbin"
status=`systemctl status hcu | grep /var/hcu/hcu | awk '{ print $4}'`
if [ $status == "/var/hcu/hcu" ]
	then
	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is running." >> /dev/null
	#exit
else
        echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is neither running, nor exited." >> /var/hcu/hcu-abnormal-status.log
        systemctl stop hcu
        systemctl start hcu
	echo $(date +%Y-%m-%d_%H:%M:%S)" hcu is restored from neither running nor exited." >> /var/hcu/hcu-abnormal-status.log
fi

#Monitor MQTT服务
if ((pgrep -f "node /var/www/html/localui/mqttserver.js") > 0); then
	echo "Server is running!" >> /dev/null
else
	node /var/www/html/localui/mqttserver.js >> /dev/null &
fi
if ((pgrep -f "node /var/www/html/localui/client_uirouter.js") > 0); then
	echo "Client is running!" >> /dev/null
else
	node /var/www/html/localui/client_uirouter.js >> /dev/null &
fi


