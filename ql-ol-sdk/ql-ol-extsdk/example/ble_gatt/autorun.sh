#!/bin/sh
echo "=================autorun.sh is called====================="

echo"=========================================Going to prepare test APP========================================="

libpath="/home/victor/P4_wksC/depot3/Qualcomm/MDM9x07/MCU_R08/apps_proc/poky/build/tmp-glibc/work/mdm9607-oe-linux-gnueabi/ql-ol-extsdk/git-r01/package/usr/lib/libql_common_api.a"
libdes="/home/victor/wks/TskA004B/ql-ol-sdk/ql-ol-extsdk/lib/libql_common_api.a"
if [ ! -e ${libpath} ]
then
	echo "libql_common_api.a does not exist"
	exit
else
	echo "Going to copy lib to SDK"
	rm ${libdes}
	cp ${libpath} ${libdes}
fi
hdfpath="/home/victor/P4_wksC/depot3/Qualcomm/MDM9x07/MCU_R08/apps_proc/quectel-openlinux-extsdk/include/ql_ble.h"
hdfdes="/home/victor/wks/TskA004B/ql-ol-sdk/ql-ol-extsdk/include/ql_ble.h"
if [ ! -e ${hdfpath} ]
then
	echo "head file ql_ble.h does not exist"
	exit
else
	rm ${hdfdes}
	cp ${hdfpath} ${hdfdes}
fi
cd "/home/victor/wks/TskA004B/ql-ol-sdk/ql-ol-extsdk/example/ble_gatt"
pwd
rm ble_gatt_client
make
ls *
cp ble_gatt_client /home/victor/wks/TskA004B/tmp/ble_gatt_client
echo "===========================================Going to copy quectel_ble_service==========================="
qblepath="/home/victor/P4_wksC/depot3/Qualcomm/MDM9x07/MCU_R08/apps_proc/poky/build/tmp-glibc/prebuilt/mdm9607/synergy/usr/bin/quectel_ble_service"
qbledespath="/home/victor/wks/TskA004B/tmp/quectel_ble_service"

if [ ! -e ${qbledespath} ]
then
	rm ${qbledespath}
fi

if [ ! -e ${qblepath} ]
then
	echo "${qblepath} doens not exist"
	exit
else
	cp ${qblepath} ${qbledespath}
	ls ${qbledespath} -l
fi

cp /home/victor/wks/TskA004B/tmp ~/buf/TskA004B -R

