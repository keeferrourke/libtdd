# libctest Makefile
# Keefer Rourke <mail@krourke.org>
#
# This Makefile creates dynamic and static library binaries for `libctest`.
# Libraries are output to $PWD/lib
#
# See LICENSE file included at the project root.
 
# specify the shell, in case the SHELL variable is not set or is not
# inherited from the environment
SHELL = /bin/bash

# set suffix list, to prevent confusion between different make programs
# line 15 clears an implied suffix list, and line 16 sets a new one
.SUFFIXES:
.SUFFIXES: .c .h .o

# library project structure
CC	= gcc
AR	= ar
WD	= $(PWD)
INCLDIR	= $(WD)/include
SRCDIR	= $(WD)/src
OBJDIR	= $(WD)/obj
OUTDIR	= $(WD)/lib

# DOCDIR and TEXDIR must match the appropriate directories specified in the
# Doxyfile; TEXDIR is a subdirectory of DOCDIR
DOCDIR = docs/
TEXDIR = latex/

# library output
_LIB	= libctest
LIB	= $(addprefix $(OUTDIR)/, $(_LIB))

# files; here all object files will be stored in OBJDIR, with the same names
# as corresponding c files from SRCDIR
SRC	= $(wildcard $(SRCDIR)/*.c)
_OBJS	= $(patsubst $(SRCDIR)/%.c, %.o, $(SRC))
OBJS	= $(addprefix $(OBJDIR)/, $(_OBJS))

# compilation flags
CFLAGS	= -Wall -std=c99 -pedantic -g
INCLUDE = -I$(INCLDIR)

.PHONY: all lib

# this target compiles the final library binaries and generates documentation
all: lib docs docs-clean

lib: $(OBJS) $(OUTDIR)
	@$(CC) -shared -o $(LIB).so $<
	@$(AR) crs $(LIB).a $<
	@echo "Compiled libctest to $(OUTDIR)"

# automatically compile all objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(OBJDIR)
	@$(CC) $(CFLAGS) $(INCLUDE) -fpic -c $< -o $@

# the following targets simply ensure that the expected directories exist
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OUTDIR):
	@mkdir -p $(OUTDIR)
 
# generate docs with doxygen
# this is intended to be used with a Doxyfile that specified LaTeX output
# modify as required for different documentation formats
#
# inelegant, but if doxygen fails for some reason, it is not the end of the
# world
.PHONY: docs docs-clean
docs: Doxyfile
	 @doxygen 2>/dev/null 1>&2
	-@echo 'Generating application internal documentation...'
#	generate PDF from LaTeX sources
	-@cd $(DOCDIR)$(TEXDIR) && $(MAKE) 2>/dev/null 1>&2
	-@mv $(DOCDIR)$(TEXDIR)refman.pdf $(DOCDIR)
	-@echo 'Generated application internal documentation.'

docs-clean:
	@cd $(DOCDIR)$(TEXDIR) && $(MAKE) clean

$(DOCDIR):
	@mkdir -p $(DOCDIR)

# clean entire project directory
.PHONY: clean cleaner
clean:
	-@rm -rf $(OBJDIR) $(DOCDIR)

cleaner:
	-@rm -rf $(OUTDIR)

# check code quality
.PHONY: cppcheck memcheck
cppcheck:
	cppcheck --enable=all --language=c --std=c99 \
		--suppress=missingInclude --suppress=unusedFunction \
		$(SRC) -i $(INCLDIR)

# print target; useful for debugging this Makefile
print-% :
	@echo $($*)

