perl -0777 -pe 's,/\*.*?\*/,,gs' tt.txt > hh.c
sed -e 's/\/\/.*/d/' hh.c > tt.txt
