.PATH: ${.CURDIR}/drivers/
.PATH: ${.CURDIR}/drivers/keyboard/
.PATH: ${.CURDIR}/drivers/mouse/
.PATH: ${.CURDIR}/drivers/timer/
.PATH: ${.CURDIR}/drivers/serial/
.PATH: ${.CURDIR}/drivers/utils/
.PATH: ${.CURDIR}/drivers/rtc/
.PATH: ${.CURDIR}/drivers/graphic/
.PATH: ${.CURDIR}/strings/
.PATH: ${.CURDIR}/chat/

# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS =  main.c utils.c  keyboard.c mouse.c timer.c serial.c queue.c graphic.c strings.c rtc.c chat.c
# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
# OPTIMIZATION (NO PRINTS) -D__LCOM_OPTIMIZED__
CFLAGS += -pedantic -D__LCOM_OPTIMIZED__

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>