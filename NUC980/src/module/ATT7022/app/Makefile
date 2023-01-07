
SRCS :=  ATT7022App.c  
 
.SUFFIXES : .x .o .c .s

CC := arm-linux-gcc
STRIP := arm-linux-strip

TARGET = ATT7022App

LIBS = -lpthread -lc -lgcc -g
.PHONY:clean all 
all:
	$(CC) -o $(TARGET) -static $(SRCS) $(LIBS)
	$(STRIP) $(TARGET)

clean:
	rm -f *.o
	rm -f *.x
	rm -f *.flat
	rm -f *.map
	rm -f temp
	rm -f *.img
	rm -f $(TARGET)
	rm -f *.gdb	
