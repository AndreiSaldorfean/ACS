#!/bin/bash
# Without LVPT
BASEDIR=$(pwd)
mkdir -p "$BASEDIR/results"
COMMON="-fastfwd 300000000 -max:inst 1000000000 -cache:dl1 dl1:64:64:2:l -cache:dl1lat 2 -cache:il1 il1:512:64:1:l -cache:il1lat 2 -cache:dl2 ul2:1024:128:8:l -cache:dl2lat 6 -cache:il2lat 6 -mem:lat 270 4 -issue:width 4 -commit:width 32 -decode:width 8 -lsq:size 64 -rob:size 128 -iq:size 128 -rf:size 64 -res:ialu 8 -res:imult 8 -res:fpalu 2 -res:fpmult 4"

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
    "$BASEDIR/sim-outorder" -redir:sim "$BASEDIR/results/${name}_without_lvpt_c1.res" $COMMON "$argfile"
    mv "$workdir/results/powerfile.ptrace" "$BASEDIR/results/${name}_without_lvpt_c1.ptrace"
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
