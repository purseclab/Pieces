file klee
run --write-smt2s -solver-backend=z3 --check-div-zero=false --check-overshift=false --optimize=false --search=fsum --libc=baremetal --entry-point=max --allocate-determ --allocate-determ-start-address=0x10000 --debug-dump-stp-queries symex_temp.bc
