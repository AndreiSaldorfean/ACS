# Without LVPT
./sim-outorder -redir:sim ./results/gcc_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 gcc.arg
cd results
mv powerfile.ptrace gcc_without_lvpt_c1.ptrace
cd ..

./sim-outorder -redir:sim ./results/bzip2_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 bzip2.arg
cd results
mv powerfile.ptrace bzip2_without_lvpt_c1.ptrace
cd ..

./sim-outorder -redir:sim ./results/gzip_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 gzip.arg
cd results
mv powerfile.ptrace gzip_without_lvpt_c1.ptrace
cd ..

./sim-outorder -redir:sim ./results/applu_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 applu.arg
cd results
mv powerfile.ptrace applu_without_lvpt_c1.ptrace
cd ..

./sim-outorder -redir:sim ./results/mesa_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 mesa.arg
cd results
mv powerfile.ptrace mesa_without_lvpt_c1.ptrace
cd ..

./sim-outorder -redir:sim ./results/equake_without_lvpt_c1.res -fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 equake.arg
cd results
mv powerfile.ptrace equake_without_lvpt_c1.ptrace
cd ..