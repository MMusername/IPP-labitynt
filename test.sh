#!/bin/bash

echo ---TESTY PROGRAMU---
echo nazwa programu: $1
echo 

if [[ $# != 2 ]]
then
    echo zla ilosc danych!
    exit 1
fi

echo
echo ---ROZPOCZYNAM TESTOWANIE BEZ VALGRINDA---
echo

for file in $2/*.in
do
    echo "Test: $(basename -s.in $file)"
    ./$1<$file > $1.out 2> $1.err
    
    diff $1.out $2/$(basename -s.in $file).out >/dev/null
    diff $1.err $2/$(basename -s.in $file).err >/dev/null

    if [ $? -eq 0 ]
    then
        echo "[SUKCES] w tescie: $(basename -s.in $file)"
        echo
    else
        echo "[BLAD] w tescie: $(basename -s.in $file)"
        echo
    fi

    rm $1.out
    rm $1.err

done

echo
echo ---ROZPOCZYNAM TESTOWANIE POD VALGRINDEM---
echo

for file in $2/*.in
do
    echo "Test: $(basename -s.in $file)"
    valgrind -q --error-exitcode=2 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./$1<$file > $1.out 2> $1.err;
    
    Exitcode_valgrind=$?

    diff $1.out $2/$(basename -s.in $file).out >/dev/null
    diff $1.err $2/$(basename -s.in $file).err >/dev/null

    Exitcode_diff=$?

    if [ $Exitcode_diff -eq 0 ]
    then
        echo "[SUKCES] w tescie: $(basename -s.in $file)"
        echo
    else
        if [ $Exitcode_valgrind -eq 2 ]
        then
            echo [WYCIEK PAMIECI]
            echo
            valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all ./$1<$file;
            echo
        fi

        echo "[BLAD] w tescie: $(basename -s.in $file)"
        echo
    fi

    rm $1.out
    rm $1.err

done

echo
echo ---ZAKONCZONO TESTY---
