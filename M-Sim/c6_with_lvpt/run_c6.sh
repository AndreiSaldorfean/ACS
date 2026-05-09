#!/bin/bash
# With LVPT
BASEDIR=$(pwd)
mkdir -p "$BASEDIR/results"
COMMON="-fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:128:8:l -cache:dl1lat 5 -cache:il1 il1:32:256:8:l -cache:il1lat 9 -cache:dl2 ul2:4096:256:8:l -cache:dl2lat 10 -cache:il2lat 10 -mem:lat 270 4 -issue:width 16 -commit:width 32 -decode:width 32 -lsq:size 1024 -rob:size 1024 -iq:size 256 -rf:size 256 -res:ialu 8 -res:imult 8 -res:fpalu 8 -res:fpmult 8 -lvpt:size 8192 -lvpt:memaddr 0 -lvpt:assoc 4 -lvpt:history 2 -lvpt:access 1"

run_sim() {
    local name=$1
    local argfile=$2
    local workdir="$BASEDIR/.workdir_$name"
    rm -rf "$workdir"
    mkdir -p "$workdir/results"
    # Symlink read-only items; create real dirs for benchmark output dirs
    for item in "$BASEDIR"/*; do
        local base
        base=$(basename "$item")
        [[ "$base" == "results" ]] && continue
        [[ "$base" == .workdir_* ]] && continue
        if [[ -d "$item" ]]; then
            # Real dir so benchmarks can write output files into it
            mkdir -p "$workdir/$base"
            ln -s "$item"/* "$workdir/$base/" 2>/dev/null || true
        else
            ln -s "$item" "$workdir/$base"
        fi
    done
    # Pre-create powerfile.ptrace so mv won't fail if sim exits early
    touch "$workdir/results/powerfile.ptrace"
    cd "$workdir"
    "$BASEDIR/sim-outorder" -redir:sim "$BASEDIR/results/${name}_with_lvpt_c6.res" $COMMON "$argfile"
    mv "$workdir/results/powerfile.ptrace" "$BASEDIR/results/${name}_with_lvpt_c6.ptrace"
    cd "$BASEDIR"
    rm -rf "$workdir"
}

run_sim gcc     "$BASEDIR/gcc.arg" &
run_sim bzip2   "$BASEDIR/bzip2.arg" &
run_sim gzip    "$BASEDIR/gzip.arg" &
run_sim applu   "$BASEDIR/applu.arg" &
run_sim mesa    "$BASEDIR/mesa.arg" &
run_sim equake  "$BASEDIR/equake.arg" &

wait
