target remote :3333
load
b main
monitor tpiu config internal itm.fifo uart off 168000000
monitor itm ports on
c
