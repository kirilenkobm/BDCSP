CC = gcc
CFLAGS = -fPIC
LDFLAGS = -shared
RM = rm -f
TARGET_LIB = libs/CSP.so

SRCS = src/CSP.c src/grid.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c):%.c
	$(CC) $(CFLAGS) -MM $< >$@

.PHONY: clean
clean:
	-${RM} ${OBJS} $(SRCS:.c)
