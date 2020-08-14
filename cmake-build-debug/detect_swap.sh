#!/bin/bash
#检测本机的swap大小
swap=`awk '($1 == "SwapTotal:"){print $2/1024}' /proc/meminfo`
echo $swap
