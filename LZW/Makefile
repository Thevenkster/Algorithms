# Mention default target.
all:

	# Implicit rule to compile C++ files.  Modify to your taste.
%.o: %.cpp
	g++ -c -O2 -Wall -Wextra $<

# To ``make the library'', make all its objects using the implicit rule.
library: $(library-objects)

# Conservatively assume that all the objects depend on all the headers.
$(library-objects): $(library-headers)

	# Components of the program.
program =	lzw435.cpp
program-objects =	lzw435

	# Conservatively assume all the program source files depend on all the library
	# headers.  You can change this if it is not the case.
$(program-objects) : $(library-headers)

	# How to link the program.  The implicit rule covers individual objects.
$(program) : $(program-objects) $(library-objects)
	g++ $@ -o $^

all : library $(program)
