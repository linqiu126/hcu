#!/bin/bash
#set -x

if [ -f "hcu_new" ]; then 
	echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu_new exits."
	sudo systemctl stop hcu
	echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service stopped."
	sudo rm /var/hcu/hcu
	echo "$(date +%Y-%m-%d_%H:%M:%S)  old hcu removed."
	sudo mv /var/hcu/hcu_new /var/hcu/hcu
	sudo chmod 777 /var/hcu/hcu
	echo "$(date +%Y-%m-%d_%H:%M:%S)  new hcu replaced."
	sudo systemctl start hcu
	echo "$(date +%Y-%m-%d_%H:%M:%S)  hcu service started."
fi 