#######################################################################
# Variablen
#######################################################################
OBJS = gui_sample.o
       
CC         = gcc
LD         = gcc
CC_FLAGS   = -Os -c -I../ `sdl-config --cflags`
LD_FLAGS   = -L../bin -la2e `sdl-config --libs` -lSDL_net -lSDL_ttf -o

#######################################################################
# Files
#######################################################################
gui_sample.o: gui_sample.cpp gui_sample.h
	$(CC) gui_sample.cpp $(CC_FLAGS)

gui_sample: gui_sample.o
	$(LD) gui_sample.o $(LD_FLAGS) gui_sample