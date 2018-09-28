#allow not a hard tab
.RECIPEPREFIX = >

#which comiler
CC = gcc

#source code
SOURCE = main.c

#linker flags
LFLAGS =

#target executable
TARGET = exe

debug: $(SOURCE)
> $(CC) $(SOURCE) -Wall -ggdb $(LFLAGS) -o $(TARGET)

release: $(SOURCE)
> $(CC) $(SOURCE) -Wall $(LFLAGS) -o $(TARGET)
