#######################################################################
# Variablen
#######################################################################
OBJS = main.o \
       msg.o \
       core.o \
       net.o

CC         = gcc
LD         = gcc
CC_FLAGS   = -Wall -Os -c `sdl-config --cflags`
LD_FLAGS   = -lstdc++ `sdl-config --libs` -lSDL_net -o
EXECUTABLE = test

#######################################################################
# Files
#######################################################################
test: $(OBJS)
	$(LD) $(OBJS) $(LD_FLAGS) $(EXECUTABLE)


main.o: main.cpp main.h
	$(CC) main.cpp $(CC_FLAGS)

msg.o: msg.cpp msg.h
	$(CC) msg.cpp $(CC_FLAGS)
	
core.o: core.cpp core.h
	$(CC) core.cpp $(CC_FLAGS)

net.o: net.cpp net.h
	$(CC) net.cpp $(CC_FLAGS)

#######################################################################
# Other
#######################################################################
all: test docu

new: clean all

release: all strip
	rm -f $(OBJS)

clean:
	rm -f $(OBJS) $(EXECUTABLE)
	rm -fr doc/html
	rm -fr doc/latex

strip:
	strip test

docu:
	doxygen
