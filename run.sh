ALGOR?=QUICK_SORT

for ALGOR in QUICK_SORT SORT_MERGE SORT_NETWORK_4 SORT_NETWORK_8 HISTOGRAM
do
    ALGORITHM=${ALGOR} make all
    wait
    sudo ./bin/host_code -w 0 -e 1 > profile/${ALGOR}_tl.txt
    wait
    make clean
done