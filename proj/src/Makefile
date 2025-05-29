# name of the program (Minix service)
PROG=proj

# Set search paths for source files
.PATH: ${.CURDIR}/
.PATH: ${.CURDIR}/engine
.PATH: ${.CURDIR}/game

# source code files to be compiled
SRCS = main.c game.c utils.c timer.c mouse.c graphics.c kbc.c game_engine.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -I.


# list of library dependencies
DPADD += ${LIBLCF} ${LIBLM}
LDADD += -llcf -lxpm -llm

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>