#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 "0;"
try 42 "42;"
try 21 '5+20-4;'
try 41 ' 12 + 34 - 5 ;'
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 5 "-10+15;"
try 1 "1<2;"
try 0 "1+1 >= 3;"
try 1 "1 + 2 == 3;"
try 3 "a=2;a+1;"
try 3 "return 1+2;return 0;"
try 4 "abc = 1; d = 3; abc + d;"
try 1 "a = 1; if (a == 0) a = 0; a;"
try 0 "a = 1; if (a < 3) a = 0; a;"
try 5 "a = 1; if (a > 3) a = 0; else a = 4; a;"
try 2 "a = 1; if (a < 3) {a = 0; a = 2;} else a = 4; a;"

echo OK
