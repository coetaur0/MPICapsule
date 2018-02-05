CXX = mpic++
RUN = mpirun
NP = -np 2
STD = -std=c++0x
INCLUDE = -I../../include -L../../include
PROG = wordcount
FILES = wordcount.cpp
TXTFILE = ../../../data/examplefile.txt
RESULT = ./wordcount_result.txt

all: wordcount_example run_wordcount clean

wordcount_example:
	 $(CXX) $(STD) $(INCLUDE) -o $(PROG) $(FILES)

run_wordcount:
	$(RUN) $(NP) $(PROG) $(TXTFILE) >> $(RESULT)

clean:
	rm $(PROG)
