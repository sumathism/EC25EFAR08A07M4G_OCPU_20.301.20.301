#!/bin/sh
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of The Linux Foundation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# find_partitions        init.d script to dynamically find partitions
#
FindAndMountUsrdataUBI () {
        partition=$1
        dir=$2
        ubinub=$3
        QuecUsrdataFormatTimes=0
        Var1=0
        Var2=1
        ubiattach_time=0
        device=/dev/ubi${ubinub}_0

   mtd_block_number=`cat $mtd_file | grep -wi $partition | sed 's/^mtd//' | awk -F ':' '{print $1}'`
   echo "MTD : Detected block device : $dir for $partition" > /dev/kmsg
   mkdir -p $dir

   ubiattach -m $mtd_block_number -d  $ubinub /dev/ubi_ctrl

        while [ 1 ]
        do
                if [ -e $device ]
        then
                        break
                else
                        sleep 0.01
                        ubiattach_time=$((ubiattach_time+1))
                        echo "MTD ubiattach wait times= $ubiattach_time" > /dev/kmsg

                        if [ "$ubiattach_time" -gt "100" ]
                        then
                                echo " MTD wait times = $ubiattach_time, > 100 !!!" > /dev/kmsg
                                break
                        fi
                fi
        done
# this is usrdata partition ubi first attach , if ubiattach fail, it is mean usrdata partiotn crashed , need format
        if [ -e $device ] ;then
                echo " MTD ubi attach success !!!" > /dev/kmsg
        else
                sync
                sleep 1
		echo "MTD do ubi detach" > /dev/kmsg
                ubidetach -p /dev/mtd$mtd_block_number
                sleep 1
		echo "MTD do ubi format" > /dev/kmsg
                ubiformat /dev/mtd$mtd_block_number -y
                #sleep 1
                #ubiattach -p /dev/mtd$mtd_block_number
                sleep 2
		echo "MTD do ubi attach" > /dev/kmsg
                ubiattach -m $mtd_block_number -d  $ubinub /dev/ubi_ctrl
                if [ -e $device ];then
                        echo "MTD ubiattach success !!!"  > /dev/kmsg
                else
                        echo "MTD ubiattach failed !!!" > /dev/kmsg
                fi
                sleep 1
		echo "MTD do ubimkvol" > /dev/kmsg
                ubimkvol /dev/ubi${ubinub} -m -N ${dir#*/}
                sleep 1
if [ -e $device  ];then
                        echo  "MTD ubimkvol success !!!" > /dev/kmsg
                else
                        echo "MTD ubimkvol failed !!!" > /dev/kmsg
                fi
                sync
        fi

        mounttimes=0
	waitdevicetimes=0
        while [ 1 ]
    do
        if [ -e $device ]
        then
		mounttimes=$((mounttimes+1))
		echo " MTD $device mount times =$mounttimes" > /dev/kmsg
		if [ "$mounttimes" -gt "10" ]
		then
			echo " MTD mount times =$mounttimes, > 10.  mount failed, need format usr_data partition" > /dev/kmsg
			sleep 0.1
                	ubidetach -p /dev/mtd$mtd_block_number
                	sleep 1
                	ubiformat /dev/mtd$mtd_block_number -y
                	sleep 1
			sys_reboot
		fi
		mount -t ubifs /dev/ubi${ubinub}_0 $dir -o bulk_read
		sleep 0.1
		mountResult1=`mount | grep $dir`

		if [ "" = "$mountResult1" ]
		then
			echo "MTD usrdata mount fail !!!!" > /dev/kmsg
		else
			echo "MTD usrdata mount success !!!!" > /dev/kmsg
			break
		fi
	else
 		sleep 0.01
                waitdevicetimes=$((waitdevicetimes+1))
                echo " MTD $device wait times =$waitdevicetimes" > /dev/kmsg
                if [ "$waitdevicetimes" -gt "100" ]
                then
                	echo " MTD wait times =$waitdevicetimes, > 100.  wait device failed, need format usr_data partition" > /dev/kmsg
                        sync
                        sleep 1
                fi
                
		if [ "$waitdevicetimes" -gt "110" ]
                then
                	echo "MTD exit mount usrdata" > /dev/kmsg
                        sync
                        break

               	fi
	fi
    done
}





FindAndMountVolumeUBI () {
   volume_name=$1
   dir=$2
   if [ ! -d $dir ]
   then
       mkdir -p $dir
   fi
   mount -t ubifs ubi0:$volume_name $dir -o bulk_read
}

BindToData(){
read_only_test_flag=`cat /proc/mounts | grep -i "ubi0:rootfs" | grep rw`
if [ "$read_only_test_flag" == "" ]
then
  datadir_name=$1
  
  mkdir -p $datadir_name/etc
  #mkdir -p $datadir_name/data
  mkdir -p $datadir_name/cache
  mkdir -p /var/volatile/tmp
  if [ ! -f $datadir_name/bind_flag ];then
	cp -a /etc/*  $datadir_name/etc/
	#cp -a /data/* $datadir_name/data/
	cp -a /data_swap/*  /data/
	sync
	touch $datadir_name/bind_flag
	sync
  fi
  mount --bind $datadir_name/etc/   /etc
  #mount --bind $datadir_name/data/  /data
  mount --bind $datadir_name/cache/  /cache
fi

}

mtd_file=/proc/mtd

fstype="UBI"


eval FindAndMountUsrdata${fstype} usr_data /usrdata  2
eval FindAndMountUsrdata${fstype} data /data  3
eval FindAndMountUsrdata${fstype} spi6.0 /mnt 5
#add by [francis],20180507,for add new data parition for EC20C_HA

BindToData usrdata
#add by [francis],20180719,bind dir to usrdata
# quectel modification 20160802 add
wait 

exit 0
