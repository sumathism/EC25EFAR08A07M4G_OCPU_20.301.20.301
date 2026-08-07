#! /bin/sh
#
# Copyright (c) 2010-2017 @ Quectel Wireless Solutions Co., Ltd.  All Rights Reserved.
#
# start_z_oemapp_le  init.d script to mount oemapp and start the customer application.

MTD_NAME=oemapp

# get mtd device number
get_mtd_num()
{

    match_val=`cat /proc/mtd | grep -iw $1`
    if [ -n "$match_val" ]
    then
	mtd_name=${match_val%:*}
	echo -n ${mtd_name:3}
	return ${mtd_name:3}
    fi

    echo -n "-1"
    return -1
}

# get ubi number
get_ubi_num()
{
    ubi_num_list=`find /sys/class/ubi -name "ubi[0-9]"`
    for i in $ubi_num_list
    do
	match_val=`cat $i/mtd_num`
	if [ x"$match_val" = x"$1" ]
	then
	    ubi_num=${i##*/}
	    echo -n ${ubi_num:3}
	    return ${ubi_num:3}
	fi
    done

    ubi_num_list=`find /sys/class/ubi -name "ubi[0-9][0-9]"`
    for i in $ubi_num_list
    do
	match_val=`cat $i/mtd_num`
	if [ x"$match_val" = x"$1" ]
	then
	    ubi_num=${i##*/}
	    echo -n ${ubi_num:3}
	    return ${ubi_num:3}
	fi
    done

    echo -n "-1"
    return -1
}

wait_on_file()
{
    local cntmax=10
    local ret=${LE_OK}

    while [ ! -e "$1" ] ; do
        usleep 500000
        cntmax=$( echo $(( ${cntmax} - 1 )) )
        if [ ${cntmax} -eq 0 ] ; then
                ret=${LE_ERR}
                break
        fi
        done

        return ${ret}
}

# mount oemapp partition
mnt_oemapp()
{
    local mountresult=`mount |grep /dev/ubiblock |grep oemapp`

    if [ "" = "$mountresult" ]
    then
	echo "SoftwareSim:the oemapp partition not mount,continue..." > /dev/kmsg
    else
       echo "SoftwareSim:the oemapp partition have mount!!!" > /dev/kmsg
       return 1	
    fi

    mtd_num=`get_mtd_num $MTD_NAME`
    if [ x"$mtd_num" = x"-1" ]
    then
	echo "SoftwareSim:the oemapp partition name does not exist" > /dev/kmsg
	
    fi

    ubiattach /dev/ubi_ctrl -m $mtd_num
    retryCnt=0
    while [ x"$?" != x"0" ]
    do
        ubidetach -m $mtd_num
        usleep 50000
        ubiattach /dev/ubi_ctrl -m $mtd_num
        if [ $retryCnt = 10 ];then
            break
        fi
        let retryCnt+=1
    done
    usleep 50000

    ubi_num=`get_ubi_num $mtd_num`
    if [ x"$ubi_num" = x"-1" ]
    then
	echo "SoftwareSim: ubi attach failed" > /dev/kmsg
	echo -n "-1"
	return -1
    fi

    ubiblock -c /dev/ubi${ubi_num}_0
    if [ x"$?" != x"0" ]
    then
	echo "SoftwareSim: ubi set block failed" > /dev/kmsg
	echo -n "-1"
	return -1
    fi

    wait_on_file /dev/ubiblock${ubi_num}_0
    if [ ! -e /dev/ubiblock${ubi_num}_0 ]
    then
	echo "SoftwareSim: the /dev/ubiblock${ubi_num}_0 does not exist" > /dev/kmsg
	echo -ne "-1"
	return -1
    fi
    
    mount -t squashfs -r /dev/ubiblock${ubi_num}_0 /oemapp
    if [ x"$?" != x"0" ]
    then
	echo "SoftwareSim: mount ubiblock${ubi_num}_0 to /oemapp failed" > /dev/kmsg
	echo -n "-1"
	return -1
    fi
}

umount_oemapp(){
    umount /oemapp
    
    mtd_num=`get_mtd_num $MTD_NAME`
    if [ x"$mtd_num" = x"-1" ]
    then
	    echo "mount_oemapp.sh:the oemapp partition name does not exist" > /dev/kmsg
    fi

    ubi_num=`get_ubi_num $mtd_num`
    if [ x"$ubi_num" = x"-1" ]
    then
	    echo "mount_oemapp.sh get ubi num failed" > /dev/kmsg
	    echo -n "-1"
	    return -1
    fi

    ubiblock -r /dev/ubi${ubi_num}_0

    ubidetach /dev/ubi_ctrl -m $mtd_num
}

if [ "$1"x = "domount"x ];then
    mnt_oemapp
elif [ "$1"x = "unmount"x ];then
    umount_oemapp
else
    echo "mount_oemapp.sh params error"  > /dev/kmsg
fi
