#!/bin/bash
# written by @neoapps-dev :)
compiler_found=false
compiler=""
for c in gcc tdm-gcc tcc; do
    if command -v "$c" &>/dev/null; then
        compiler_found=true
        compiler="$c"
        break
    fi
done
if ! $compiler_found; then
    echo "Could not find any compiler in PATH."
    exit 1
else
    for bits in 32 64; do
        if ! "$compiler" src/newtrodit.c -o "newtrodit_x${bits}" -O2 -luser32 -lkernel32 -m"$bits"; then
            echo "Compilation error for Newtrodit x${bits}"
            read -p "Press Enter to exit..." dummy
            exit 1
        else
            echo "Compilation successful for Newtrodit x${bits}"
        fi
    done
    read -p "Press Enter to exit..." dummy
fi
