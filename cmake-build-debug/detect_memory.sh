#!/bin/bash
process="enPAC_2020_3_0"		#进程名字
PID=$(ps x | grep $process | grep -v grep | awk '{print $1}')
size=` cat /proc/${PID}/status | grep RSS | tr -cd "[0-9]"`
echo $size