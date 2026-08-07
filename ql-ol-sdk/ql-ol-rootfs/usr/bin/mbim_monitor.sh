#!/bin/sh

while [ true ]
do
    pid=`pidof mbimd`
    if [ $? -ne 0 ]; then
	killall mbimd
	/usr/bin/mbimd &
    fi
    sleep 1
done
