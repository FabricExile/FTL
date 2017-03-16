.PHONY : all

CXX_FLAGS = -O2 -g -std=c++03 -stdlib=libstdc++
# CXX_FLAGS = -O2 -g -std=c++11 -stdlib=libc++

all : decode testSmallString

decode : decode.cpp Makefile ../../../FTL/OrderedStringMap.h ../../../FTL/JSONValue.h ../../../FTL/SmallString.h ../../../FTL/JSONDec.h
	g++ -o $@ decode.cpp -I../../.. $(CXX_FLAGS)

testSmallString : testSmallString.cpp Makefile ../../../FTL/SmallString.h
	g++ -o $@ testSmallString.cpp -I../../.. $(CXX_FLAGS)
