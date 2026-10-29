CXX	= g++ 		# main compiler
## optional flags
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra #c++ standard version optimize performance enable-common-warning enable-additional-warning

#define source file
DC_SRC = divide_conquer/merge_sort.cpp

#define all target ain enrty points
all: smartcity tests bench # make sure these three targets are completed


