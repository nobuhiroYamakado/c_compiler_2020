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

assert 0 "return 0;"
assert 4 "return 4;"
assert 42 "return (42);"
assert 21 "return 5+20-4;"
assert 41 "return  12 + 34 - 4 - 1 ;"
assert 47 "return 5+6*7;"
assert 15 "return 5*(13-10);"
assert 4 "return (3+5)/2;"
assert 10 "return -10+20;"
assert 103 "return +(48/2)+3*(-8)+(-(-106+3));"

assert 0 "return 0==1;"
assert 1 "return 42 == 42;"
assert 1 "return 1 != 2;"
assert 0 "return 41!=41;"

assert 1 "return 3 > 2;"
assert 0 "return 1>1;"
assert 0 "return 2>3;"
assert 1 "return 3 >= 2;"
assert 1 "return 1>=1;"
assert 0 "return 2>=3;"

assert 0 "return 3 < 2;"
assert 0 "return 1<1;"
assert 1 "return 2<3;"
assert 0 "return 3 <= 2;"
assert 1 "return 1<=1;"
assert 1 "return 2<=3;"

assert 0 "return 4 <= 2;"
assert 2 "return (42 < 43)+(1 >= 1);"

assert 1 "return (1); (2); (3);"
assert 2 "1; return 2; 3;"
assert 3 "1; 2; return 3;"

assert 3 "a=3; return a;"
assert 8 "a=3; z=5; return a+z;"
assert 6 "a=b=3; return a+b;"

echo OK
