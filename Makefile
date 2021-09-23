
# GNU Makefile

TARGET  =  libktrace.so

CC      =  gcc
WFLAGS  = -Wall -Wno-deprecated-declarations #-Wextra
CFLAGS  = -fPIC -g3 $(WFLAGS)
LDFLAGS = -shared

SRCDIR  = ./src
OBJDIR  = ./obj
TESTDIR = ./test
RM      =  rm -f

SRCS = $(SRCDIR)/heap_trace.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET) test

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

clean:
	$(RM) $(TARGET) $(OBJS) $(SRCS:.c=.d) core
	$(MAKE) -C test clean

test:
	$(MAKE) -C test

.PHONY: all clean test

#EOF
