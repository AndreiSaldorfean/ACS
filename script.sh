#!/bin/bash

# ./simplesim-3.0/sim-LVPred -redir:sim applu_simout_bpred.res -redir:prog applu_bpred -max:inst 50000000 -contor:LD 1 -bpred 2lev -bpred:2lev 1 1024 10 0 ./inputs/ss_files/cc1.ss < ./inputs/in_files/1stmt.i

./simplesim-3.0/sim-LVPred -redir:sim cc1_simout.res -redir:prog cc1_progout.res -max:inst 500000000 -nice 1  -bpred 2lev -bpred:2lev 1 1024 10 0 -contor:LD 1 ./inputs/ss_files/cc1.ss < ./inputs/in_files/1stmt.i