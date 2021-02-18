#!/bin/sh
module="chardevice"
device="chardevice"

/sbin/rmmod $module $* || exit 1

rm -f /dev/${device} /dev/${device}
