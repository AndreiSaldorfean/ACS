#!/bin/bash

cd simplesim-3.0; make sim-spred.elf; cd ..
./simplesim-3.0/sim-spred.elf -redir:sim apsi.res -redir:prog apsi_progout.re -max:inst 20000000 -history 2 -pred 0 -memaddr 0 ./inputs/ss_files/apsi.ss < ./inputs/in_files/apsi.in

# ./simplesim-3.0/sim-LVPred -redir:sim cc1_simout.res -redir:prog cc1_progout.res -max:inst 500000000 -nice 1  -bpred 2lev -bpred:2lev 1 1024 10 0 -contor:LD 1 ./inputs/ss_files/cc1.ss < ./inputs/in_files/1stmt.i
