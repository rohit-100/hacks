#/bin/bash


val=$( ps -eo pcpu| sort -r -k1 | awk '{ if(NR>1) sum +=$1 } END{print sum}')
req=70

val=${val%.*}
echo "total = $val"
if [ $val -gt $req ]
then
	notify-send "cpu uses is higher" " look into Top"
fi

####   command to be run on terminal watch -n 10 ./file.sh 
