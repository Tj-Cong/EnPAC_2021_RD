#!/bin/bash
#检测本机环境下内存大小
totalmem=`awk '($1 == "MemTotal:"){print $2/1024}' /proc/meminfo`
echo $totalmem
