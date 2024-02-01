.PHONY: all compile run clean

all: compile run

compile:
	g++ cpp_test.cpp --std=c++17 -o cpp_test.out

run:
	./cpp_test.out

clean:
	rm cpp_test.out

pybind11:
	g++ cpp_test.cpp -L ../pybind11 -I ../pybind11/include -I /home/dino/anaconda3/envs/kbmod/include/python3.10/Python.h --std=c++17 -o cpp_test.out


