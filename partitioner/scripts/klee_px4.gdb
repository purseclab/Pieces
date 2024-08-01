file klee
run -disable-verify=true --write-smt2s -solver-backend=z3 --check-div-zero=false --check-overshift=false --optimize=false --search=lloop --libc=baremetal --entry-point=pid_calculate --allocate-determ --allocate-determ-start-address=0x10000 --debug-dump-stp-queries /home/arslan/projects/Pieces/partitioner/out/symex_temp.bc
