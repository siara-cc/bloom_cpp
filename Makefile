C = gcc
CXXFLAGS = -pthread -march=native
CXX = g++
CXXFLAGS = -pthread -std=c++11 -march=native
#OBJS = build/sqlite.o
INCLUDES = -I./src

opt: CXXFLAGS += -O3 -funroll-loops -DNDEBUG
opt: bloom_test

debug: CXXFLAGS += -g -O0 -fno-inline
debug: bloom_test

bloom_test: tests/bloom_test.cpp src/*.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o tests/bloom_test tests/bloom_test.cpp

clean:
	rm -f *.blm
	rm -f tests/*.blm
	rm -rf *.dSYM
	rm -rf tests/*.dSYM
	rm -f tests/bloom_test

#build/.o: src/imain.cpp src/*.h
#   $(CXX) $(CXXFLAGS) $(INCLUDES) -c src/imain.cpp -o build/imain.o
