CXX = mpic++
RUN = mpirun
NP = -np 2
STD = -std=c++0x
INCLUDE = -I../../include -L../../include
PROG = squares_example
FILES = sum_of_squares.cpp
TXTFILE = ./data.txt

all: squares_example run_squares clean

squares_example:
	 $(CXX) $(STD) $(INCLUDE) -o $(PROG) $(FILES)

run_squares:
	$(RUN) $(NP) $(PROG) $(TXTFILE)

clean:
	rm $(PROG)
