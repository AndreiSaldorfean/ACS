sim-LVPred: SimpleScalar/PISA Tool Set version 3.0 of November, 2000.
Copyright (c) 1994-2000 by Todd M. Austin.  All Rights Reserved.
This version of SimpleScalar is licensed for academic non-commercial use only.

sim: command line: ./simplesim-3.0/sim-LVPred -redir:sim applu_simout_bpred.res -redir:prog applu_bpred -max:inst 50000000 -contor:LD 1 -bpred 2lev -bpred:2lev 1 1024 10 0 ./inputs/ss_files/applu.ss 

sim: simulation started @ Tue Mar 17 12:54:00 2026, options follow:

sim-bpred: This simulator implements a branch predictor analyzer.

# -config                     # load configuration from a file
# -dumpconfig                 # dump configuration to a file
# -h                    false # print help message    
# -v                    false # verbose operation     
# -d                    false # enable debug message  
# -i                    false # start in Dlite debugger
-seed                       1 # random number generator seed (0 for timer seed)
# -q                    false # initialize and terminate immediately
# -chkpt               <null> # restore EIO trace execution from <fname>
# -redir:sim     applu_simout_bpred.res # redirect simulator output to file (non-interactive only)
# -redir:prog     applu_bpred # redirect simulated program output to file
-nice                       0 # simulator scheduling priority
-max:inst            50000000 # maximum number of inst's to execute
-bpred                   2lev # branch predictor type {nottaken|taken|bimod|2lev|comb}
-bpred:bimod     2048 # bimodal predictor config (<table size>)
-bpred:2lev      1 1024 10 0 # 2-level predictor config (<l1size> <l2size> <hist_size> <xor>)
-bpred:comb      1024 # combining predictor config (<meta_table_size>)
-bpred:ras                  8 # return address stack size (0 for no return stack)
-bpred:btb       512 4 # BTB config (<num_sets> <associativity>)
-contor:LD                  1 # coutner for loads and store

  Branch predictor configuration examples for 2-level predictor:
    Configurations:   N, M, W, X
      N   # entries in first level (# of shift register(s))
      W   width of shift register(s)
      M   # entries in 2nd level (# of counters, or other FSM)
      X   (yes-1/no-0) xor history and address for 2nd level index
    Sample predictors:
      GAg     : 1, W, 2^W, 0
      GAp     : 1, W, M (M > 2^W), 0
      PAg     : N, W, 2^W, 0
      PAp     : N, W, M (M == 2^(N+W)), 0
      gshare  : 1, W, 2^W, 1
  Predictor `comb' combines a bimodal and a 2-level predictor.



sim: ** starting functional simulation w/ predictors **
warning: syscall: sigvec ignored
warning: syscall: sigvec ignored
warning: syscall: sigvec ignored
warning: syscall: sigvec ignored
warning: syscall: sigvec ignored
warning: syscall: sigvec ignored

sim: ** simulation statistics **
counter_loads               9497406 # total number of loads executed
sim_num_insn               50000000 # total number of instructions executed
sim_num_refs               11012504 # total number of loads and stores executed
sim_elapsed_time                  1 # total simulation time in seconds
sim_inst_rate          50000000.0000 # simulation speed (in insts/sec)
sim_num_branches            1185968 # total number of branches executed
sim_IPB                     42.1597 # instruction per branch
bpred_2lev.lookups          1185968 # total number of bpred lookups
bpred_2lev.updates          1185968 # total number of updates
bpred_2lev.addr_hits        1167074 # total number of address-predicted hits
bpred_2lev.dir_hits         1167474 # total number of direction-predicted hits (includes addr-hits)
bpred_2lev.misses             18494 # total number of misses
bpred_2lev.jr_hits             8117 # total number of address-predicted hits for JR's
bpred_2lev.jr_seen             8157 # total number of JR's seen
bpred_2lev.jr_non_ras_hits.PP          379 # total number of address-predicted hits for non-RAS JR's
bpred_2lev.jr_non_ras_seen.PP          411 # total number of non-RAS JR's seen
bpred_2lev.bpred_addr_rate    0.9841 # branch address-prediction rate (i.e., addr-hits/updates)
bpred_2lev.bpred_dir_rate    0.9844 # branch direction-prediction rate (i.e., all-hits/updates)
bpred_2lev.bpred_jr_rate    0.9951 # JR address-prediction rate (i.e., JR addr-hits/JRs seen)
bpred_2lev.bpred_jr_non_ras_rate.PP    0.9221 # non-RAS JR addr-pred rate (ie, non-RAS JR hits/JRs seen)
bpred_2lev.retstack_pushes         7749 # total number of address pushed onto ret-addr stack
bpred_2lev.retstack_pops         7746 # total number of address popped off of ret-addr stack
bpred_2lev.used_ras.PP         7746 # total number of RAS predictions used
bpred_2lev.ras_hits.PP         7738 # total number of RAS hits
bpred_2lev.ras_rate.PP    0.9990 # RAS prediction rate (i.e., RAS hits/used RAS)

