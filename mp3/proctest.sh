#!/bin/bash
#proctest.sh
inpid=0
echo "Input PID now: "
read  inpid
awkin="/proc/$inpid/status"
cat "$awkin" | awk '/State|^Pid|^PPid/'
echo "	RUID	EUID	SSUID	FSUID"
cat "$awkin" | awk '/Uid/'
echo "	RGID	EGID	SSGID	FSGID"
cat "$awkin" | awk '/Gid/'
cat "$awkin" | awk '/Thread|Tgid/'
awkin="/proc/$inpid/stat"
cat "$awkin" | awk '{print "Priority is: " $18, "\nNiceness is: " $19}'
cat "$awkin" | awk '{print "stime is: " $15, "\nutime is: " $14, "\ncutime is: " $16, "\ncstime is: " $17}'
awkin="/proc/$inpid/status"
cat "$awkin" | awk '/FDSize|voluntary_ctxt_switches|nonvoluntary_ctxt_switches|Cpus_allowed/'
awkin="/proc/$inpid/stat"
cat "$awkin" | awk '{print "CPU last used: " $39}'
awkin="/proc/$inpid/maps"
cat "$awkin" | awk '{print $0}' > memMap.txt