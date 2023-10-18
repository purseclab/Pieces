target remote :1234
load
b main
monitor tpiu config internal itm.fifo uart off 168000000
monitor itm ports on
c
