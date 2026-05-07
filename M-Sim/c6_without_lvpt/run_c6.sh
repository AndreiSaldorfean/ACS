# Without LVPT
./sim-outorder -redir:sim ./results/gcc_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 gcc.arg
cd results
mv powerfile.ptrace gcc_without_lvpt_c6.ptrace
cd ..

./sim-outorder -redir:sim ./results/bzip2_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 bzip2.arg
cd results
mv powerfile.ptrace bzip2_without_lvpt_c6.ptrace
cd ..

./sim-outorder -redir:sim ./results/gzip_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 gzip.arg
cd results
mv powerfile.ptrace gzip_without_lvpt_c6.ptrace
cd ..

./sim-outorder -redir:sim ./results/applu_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 applu.arg
cd results
mv powerfile.ptrace applu_without_lvpt_c6.ptrace
cd ..

./sim-outorder -redir:sim ./results/mesa_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 mesa.arg
cd results
mv powerfile.ptrace mesa_without_lvpt_c6.ptrace
cd ..

./sim-outorder -redir:sim ./results/equake_without_lvpt_c6.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 equake.arg
cd results
mv powerfile.ptrace equake_without_lvpt_c6.ptrace
cd ..