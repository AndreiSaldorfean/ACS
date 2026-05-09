#!/bin/bash
# With LVPT
BASEDIR=$(pwd)
mkdir -p "$BASEDIR/results"
COMMON="-fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:256:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:8:8:l -cache:il1lat 2 -cache:dl2 ul2:4096:256:2:l -cache:dl2lat 5 -cache:il2lat 5 -mem:lat 270 4 -issue:width 8 -commit:width 32 -decode:width 32 -lsq:size 64 -rob:size 256 -iq:size 64 -rf:size 128 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4 -lvpt:size 8192 -lvpt:memaddr 0 -lvpt:assoc 8 -lvpt:history 2 -lvpt:access 1"

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
    "$BASEDIR/sim-outorder" -redir:sim "$BASEDIR/results/${name}_with_lvpt_c3.res" $COMMON "$argfile"
    mv "$workdir/results/powerfile.ptrace" "$BASEDIR/results/${name}_with_lvpt_c3.ptrace"
    cd "$BASEDIR"
    rm -rf "$workdir"
}

run_sim applu   "$BASEDIR/results/applu.arg"  &
run_sim equake  "$BASEDIR/results/equake.arg" &
run_sim galgel  "$BASEDIR/results/galgel.arg" &
run_sim lucas   "$BASEDIR/results/lucas.arg"  &
run_sim mesa    "$BASEDIR/results/mesa.arg"   &
run_sim mgrid   "$BASEDIR/results/mgrid.arg"  &

wait
