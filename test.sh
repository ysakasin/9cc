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
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 0 '0;'
try 42 '42;'
try 21 '5+20-4;'
try 41 ' 12 + 34 - 5 ;'
try 47 '5+6*7;'
try 15 '5*(9-6);'
try 4 '(3+5)/2;'
try 10 '-10+20;'
try 12 '-(-22+10);'
try 3 '-1*-3;'
try 0 '0==1;'
try 1 '0==0;'
try 1 '5 == 3 + 2;'
try 1 '0!=1;'
try 0 '0!=0;'
try 0 '5 != 3 + 2;'
try 1 '2 < 3;'
try 0 '3 < 2;'
try 0 '2 < 2;'
try 1 '2 <= 3;'
try 0 '3 <= 2;'
try 1 '2 <= 2;'
try 0 '2 > 3;'
try 1 '3 > 2;'
try 0 '2 >= 3;'
try 1 '3 >= 2;'
try 4 'a=4; a;'
try 4 'z=4; z;'
try 3 'a=4; a-1;'
try 11 'a=5+6; a;'
try 21 'a = b = 5+6; b = b - 1; a + b;'

echo OK
