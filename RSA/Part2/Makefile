# Mention default target.
all:

	# Implicit rule to compile C++ files.  Modify to your taste.
%.o: %.cpp
	g++ -c -O2 -Wall -Wextra $<

	# Components of the library.
library-objects = \
	../Part1/BigUnsigned.o \
	../Part1/BigInteger.o \
	../Part1/BigIntegerAlgorithms.o \
	../Part1/BigUnsignedInABase.o \
	../Part1/BigIntegerUtils.o \
		sha256.o \

library-headers = \
	../Part1/NumberlikeArray.hh \
	../Part1/BigUnsigned.hh \
	../Part1/BigInteger.hh \
	../Part1/BigIntegerAlgorithms.hh \
	../Part1/BigUnsignedInABase.hh \
	../Part1/BigIntegerLibrary.hh \
		sha256.h \

# To ``make the library'', make all its objects using the implicit rule.
library: $(library-objects)

# Conservatively assume that all the objects depend on all the headers.
$(library-objects): $(library-headers)

	# Components of the program.
program =	pt2.cpp
program-objects =	pt2

	# Conservatively assume all the program source files depend on all the library
	# headers.  You can change this if it is not the case.
$(program-objects) : $(library-headers)

	# How to link the program.  The implicit rule covers individual objects.
$(program) : $(program-objects) $(library-objects)
	g++ $@ -o $^

all : library $(program)
