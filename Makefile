# Makefile for shared library
CC = gcc
# CFLAGS = -fPIC -Wall -Wextra -O2 -g
CFLAGS = -Wall -Wextra -O2 -g
# LDFLAGS = -shared
RM = rm -f
# TARGET_LIB = bin/CSP.so
TARGET = CSP
RND_GEN = generate_input

# SRCS = src/CSP.c src/grid.c src/patterns.c
SRCS = src/CSP.c src/read_input.c src/patterns.c src/render.c src/traverse.c src/arrstuff.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
# all: ${TARGET_LIB}
all: ${TARGET}

# $(TARGET_LIB): $(OBJS)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${OBJS} $(SRCS:.c)

.PHONY: rnd
rnd: $(RND_GEN)

$(RND_GEN): src/generate_input.c
	$(CC) $(CFLAGS) src/generate_input.c -o $(RND_GEN) $
