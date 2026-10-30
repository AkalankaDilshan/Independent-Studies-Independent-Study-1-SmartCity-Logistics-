# main compiler
CXX	= g++
## optional flags
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra #c++ standard version optimize performance enable-common-warning enable-additional-warning

#define source file
DC_SRC = divide_conquer/merge_sort.cpp

#define all target ain enrty points
all: smartcity tests bench # make sure these three targets are completed


## define targets
smartcity: main.cpp $(DC_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@		

# $^	All prerequisite files listed after the colon
# $@	The target name,
# Compile and merge sort
#g++ -std=c++17 -O2 -Wall -Wextra \
    main.cpp divide_conquer/merge_sort.cpp \
    -o smartcity


tests: tests/test_all.cpp $(DC_SRC) 	#Compile the test program
	$(CXX) $(CXXFLAGS) $^ -o run_tests	#Run the tests
	./run_tests							#Run the tests	 						

#Compile the test program & run like this
#g++ -std=c++17 -O2 -Wall -Wextra \
    tests/test_all.cpp divide_conquer/merge_sort.cpp \
    -o run_tests

#Same as a test
bench: bench/benchmarks.cpp $(DC_SRC)
	mkdir -p results
	$(CXX) $(CXXFLAGS) $^ -o run_bench
	./run_bench

clean:
	rm -f smartcity run_tests run_bench   # removes 3 executable files created by Makefile

.PHONY: all tests bench clean #avoid same name