#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./lilcc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

echo start debuging
assert 9 "1 + 40 - 32 "

echo OK!
