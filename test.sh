#!/bin/bash

assert() {
	expected="$1"
	input="$2"

	./chibicc "$input" > tmp.s
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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 4 - 1 ;"
assert 47 "5+6*7;"
assert 15 "5*(13-10);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 103 "+(48/2)+3*(-8)+(-(-106+3));"
assert 1 "1 != 2;"
assert 0 "1 == 2;"
assert 1 "3 > 2;"
assert 0 "4 <= 2;"
assert 2 "(42 < 43)+(1 >= 1);"
#assert 3 "3;"
#assert 6 "a = 3; b = 2; a*b;"
echo OK
