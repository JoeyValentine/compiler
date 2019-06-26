CC = gcc
LEX = flex

TARGET = 20141510

CSRCS = main.c util.c
LCODE = $(wildcard *.l)
LSRC = tiny.c
BISONCODE = cm.y
BSRC = cm.tab.c
BHDR = cm.tab.h
BOUT = cm.output
SRCS = $(CSRCS) $(LCODE:%.l=%.c) $(BSRC)
OBJS = $(SRCS:%.c=%.o)

$(TARGET) : $(LSRC) $(BSRC)
	$(CC) -o $@ $(SRCS) -ly -ll

$(BSRC) : $(LSRC) $(BISONCODE)
	bison -o $(BSRC) -vd $(BISONCODE)

$(LSRC) : $(LCODE)
	$(LEX) -o $(LSRC) $(LCODE)

.PHONY : clean
clean:
	rm -f $(TARGET) $(LSRC) $(BSRC) $(BHDR) $(BOUT)
