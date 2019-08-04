#!/usr/bin/env sh

PROGRAM=$1
TEST_D=$2


show_usage() {
    printf "Usage: $0 PROGRAM TEST-DIR\n\
    PROGRAM: The executable file.\n\
    TEST-DIR: Directory containing uCML source codes with .ml extension.\n\

    Example: $0 ./uCML ../tests\n\n";
}


if [ "$PROGRAM" = "" ];then
    echo "Error! Executable not provided.";
    show_usage;
    exit 1;
fi
if [ "$TEST_D" = "" ];then
    echo "Error! Test-files directory not provided.";
    show_usage;
    exit 2;
fi

for f in "$TEST_D/"*.ml;do
#for f in aaa.ml;do
    echo "Testing file \"$f\":"
    "./$PROGRAM" "$f" "$f.ir"
#    "./$PROGRAM" "$f"
    RET=$?
    echo "Return code: $RET"
    echo ""
#    break
done
