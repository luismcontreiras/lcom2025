# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = lab5.c graphics.c kbc.c utils.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -DLAB5

# list of library dependencies
DPADD += ${LIBLCF} ${LIBLM}
LDADD += -llcf -lxpm -llm

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>