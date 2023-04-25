ALGOR?=QUICK_SORT

for ALGOR in QUICK_SORT SORT_NETWORK_4 SORT_NETWORK_8 HISTOGRAM
do
    ALGORITHM=${ALGOR} make all
    wait
    sudo ./bin/host_code -w 0 -e 1 > profile/nested_loop_tl${k}.txt
    wait
    make clean
done