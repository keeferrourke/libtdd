# libctest Makefil
# Keefer Rourke <mail@krourke.org>
#
# This Makefile creates a dynamic C library called `libparser.so` and outputs
# it to the root of the project directory.
 
# specify the shell, in case the SHELL variable is not set or is not
# inherited from the environment
SHELL = /bin/bash

# set suffix list, to prevent confusion between different make programs
# line 11 clears an implied suffix list, and line 12 sets a new one
.SUFFIXES:
.SUFFIXES: .c .h .o

# library project structure
CC	= gcc
AR	= ar
WD	= $(PWD)/
INCLDIR	= $(WD)/include
SRCDIR	= $(WD)/src
OBJDIR	= $(WD)/obj
OUTDIR	= $(WD)/lib

# library output
_LIB	= libctest.so
LIB	= $(addprefix $(OUTDIR)/, $(_LIB))

# files; here all object files will be stored in OBJDIR, with the same names
# as corresponding c files from SRCDIR
SRC	= $(wildcard $(SRCDIR)/*.c)
_OBJS	= $(patsubst $(SRCDIR)/%.c, %.o, $(SRC))
OBJS	= $(addprefix $(OBJDIR)/, $(_OBJS))

# compilation flags
CFLAGS	= -Wall -std=c11 -pedantic -g
INCLUDE = -I$(INCLDIR)

.PHONY: all lib

# this target compiles the final library archives
all: lib

lib: $(OBJS) $(OUTDIR)
	$(CC) -shared -o $(LIB) $<

# automatically compile all objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -fpic -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)
 

# clean entire project directory
.PHONY: clean cleaner
clean:
	- rm -rf $(OBJDIR)

cleaner:
	- rm -rf $(OUTDIR)

# check code quality
.PHONY: cppcheck memcheck
cppcheck:
	cppcheck --enable=all --language=c --std=c99 \
		--suppress=missingInclude --suppress=unusedFunction \
		$(SRC) -i $(INCLDIR)

# print target; useful for debugging this Makefile
print-% :
	@echo $($*)

