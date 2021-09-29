# GNU Makefile

TARGET = test.out

CC      =  gcc
SODIR   = ../
INCLUDE = -I ../src
CFLAGS  = -g3 -Wall $(INCLUDE)
LDFLAGS = -Wl,-rpath=$(SODIR) -L$(SODIR) -lheaptrace -lm

SRCDIR  = .
SRCS    = $(SRCDIR)/test.c
OBJS    = $(SRCS:.c=.o)
RM      = rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

clean:
	$(RM) $(TARGET) $(OBJS) $(SRCS:.c=.d) core
