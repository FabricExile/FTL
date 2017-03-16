.PHONY : all

all : decode testSmallString

decode : decode.cpp Makefile ../../../FTL/OrderedStringMap.h ../../../FTL/JSONValue.h ../../../FTL/SmallString.h ../../../FTL/JSONDec.h
	g++ -o $@ decode.cpp -I../../.. -O2 -g -std=c++03 -stdlib=libstdc++

testSmallString : testSmallString.cpp Makefile ../../../FTL/SmallString.h
	g++ -o $@ testSmallString.cpp -I../../.. -O0 -g -std=c++03 -stdlib=libstdc++
