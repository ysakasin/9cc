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

try_stdout() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s foo.o
  actual=`./tmp`

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
try 6 'foo = 1; bar = 2 + 3; foo + bar;'
try 5 'return 5; return 8; 10;'
try 14 'a = 3; b = 5 * 6 - 8; return a + b / 2; 100;'
try 4 'a = 1; if (a == 1) a = 4; a;'
try 3 'a = 3; if (a == 1) a = 4; a;'
try 4 'a = 1; if (a == 1) a = 4; else a = 2; a;'
try 2 'a = 3; if (a == 1) a = 4; else a = 2; a;'
try 11 'a = 1; while (a < 10) a = a + 2; return a;'
try 55 'a = 0; i = 20; for (i = 1; i <= 10; i = i + 1) a = a + i; return a;'
try 11 'a = 1; while (a < 10) {a = a + 1; a = a + 1;} return a;'

try_stdout "OK" 'foo();'
try_stdout "10" 'bar(4, 1+5);'
try_stdout "100" 'baz(2*50);'

echo OK
