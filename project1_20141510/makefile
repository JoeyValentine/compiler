CC = gcc
LEX = flex
TARGET = 20141510
LEXSRC = tiny.l
LEXCODE = lex.yy.c
SRCS = main.c util.c $(LEXCODE)

$(TARGET) : $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) -lfl
$(LEXCODE) : $(LEXSRC)
	$(LEX) $(LEXSRC)
clean :
	-rm $(TARGET) $(LEXCODE)
