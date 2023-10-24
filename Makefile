OUT := pil-squasher pil-splitter

CFLAGS ?= -Wall -g -O2
LDFLAGS ?=
prefix ?= /usr/local

SRCS := pil-squasher.c pil-splitter.c
OBJS := $(SRCS:.c=.o)

all: $(OUT)

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^

install: $(OUT)
	install -D -m 755 pil-squasher $(DESTDIR)$(prefix)/bin/pil-squasher
	install -D -m 755 pil-splitter $(DESTDIR)$(prefix)/bin/pil-splitter

clean:
	rm -f $(OUT) $(OBJS)

