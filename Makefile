
float_multiply:
	gcc -m64 -fPIC  -o float_multiply float_multiply.c

all: float_multiply


clean:
	rm -rf float_multiply
