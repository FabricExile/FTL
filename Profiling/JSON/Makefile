.PHONY : all

ifeq ($(OS),Windows_NT)
	UNAME_S := Windows
else
  UNAME_S := $(shell uname -s)
endif

CXX_FLAGS = -O2 -g -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
ifeq ($(UNAME_S),Darwin)
	CXX_FLAGS += -std=c++03 -stdlib=libstdc++
endif

all : decode

decode : decode.cpp Makefile ../../../FTL/OrderedStringMap.h ../../../FTL/JSONValue.h ../../../FTL/SmallString.h ../../../FTL/JSONDec.h
	g++ -o $@ decode.cpp -I../../.. $(CXX_FLAGS)
