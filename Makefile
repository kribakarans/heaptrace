
# GNU Makefile

# Final binary name
TARGET  =  libheaptrace.so

# Compiler flags
CC      =  gcc
WFLAGS  = -Wall #-Wextra
INCLUDE = -I./include
CFLAGS  = -fPIC -g3 $(WFLAGS) $(INCLUDE)
LDFLAGS = -shared -rdynamic

SRCDIR  = ./src
OBJDIR  = ./obj
TESTDIR = ./test

# Makefile specifics
RM           = rm -f
LINK         = ln -sf
CTAGDIR      = .ktags
SYSLIBPATH   = $(HOME)/.lib
SYSINCLUDE   = $(HOME)/.include
INSTALL_DIR  = /usr/bin/install -d
INSTALL_BIN  = /usr/bin/install -D -m 744
INSTALL_FILE = /usr/bin/install -D -m 644

# Collect all .c source code
SRCS  = $(SRCDIR)/heaptrace.c $(SRCDIR)/heaptable.c $(SRCDIR)/htmalloc.c \
        $(SRCDIR)/hthooks.c   $(SRCDIR)/prime.c

# Build object filename to store in OBJDIR
OBJS  = $(SRCS:%.c=$(OBJDIR)/%.o)

# Create .d files containing dependencies
DEPS  = $(OBJS:%.o=%.d)

all: info tags $(TARGET)

info:
	@printf "\nBuild dependencies:"
	@printf "\nTARGET   =  $(TARGET)"
	@printf "\nCOMPILER =  $(CC)"
	@printf "\nCFLAGS   = $(CFLAGS)"
	@printf "\nINCLUDE  = $(INCLUDE)"
	@printf "\nDEPS     = $(DEPS)"
	@printf "\nOBJECTS  = $(OBJS)\n"
	@printf "\nCompiling source files:\n"

$(TARGET): $(OBJS)
	@printf "\nLinking object files:\n"
	$(CC) $(LDFLAGS) -o $@ $^
	@printf "\nBuild completed.\n\n"

# Include all .d files
-include $(DEPS)

# Build target for every single object file.
# The potential dependency on header files is covered by calling `-include $(DEPS)`.
# The -MMD flags in GCC create dependency(.d) file with same name as .o file
$(OBJDIR)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

test:
	$(MAKE) -C test

install: all
	@printf  "Installing library $(TARGET) ...\n"
	@$(INSTALL_DIR)  $(SYSLIBPATH)
	@$(INSTALL_DIR)  $(SYSINCLUDE)
	$(INSTALL_BIN)   $(TARGET) $(SYSLIBPATH)
	@$(INSTALL_FILE) $(SRCDIR)/heaptrace.h $(SYSINCLUDE)
	@echo "Done."

uninstall:
	@printf  "Removing library $(TARGET) ...\n"
	$(RM) $(SYSLIBPATH)/$(TARGET)
	@$(RM) $(SYSINCLUDE)/heaptrace.h

tags:
	@mkdir  -p $(CTAGDIR)
	@rm     -f $(CTAGDIR)/cscope.files $(CTAGDIR)/cscope.out $(CTAGDIR)/tags
	@find    $(PWD) -type f -name "*.[Scsh]" -print | sort > $(CTAGDIR)/cscope.files
	@cscope -b -i $(CTAGDIR)/cscope.files -f $(CTAGDIR)/cscope.out
	@ctags  -f $(CTAGDIR)/tags -w -L - < $(CTAGDIR)/cscope.files

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS) core -r $(CTAGDIR)
	$(MAKE) -C test clean

.PHONY: all clean test

#EOF
