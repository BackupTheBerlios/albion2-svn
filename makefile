#######################################################################
# Variablen
#######################################################################
OBJS = main.o \
       msg.o \
       core.o \
       net.o \
       gfx.o \
       engine.o \
       gui.o \
       event.o \
       gui_button.o \
       gui_text.o \
       gui_style.o

CC         = gcc
LD         = gcc
CC_FLAGS   = -Wall -Os -c `sdl-config --cflags`
LD_FLAGS   = -lstdc++ `sdl-config --libs` -lSDL_net -lSDL_ttf -o
BIN        = bin/liba2e.a

#######################################################################
# Files
#######################################################################
a2e: $(OBJS)
	ar r $(BIN) $(OBJS)
	ranlib $(BIN)

main.o: main.cpp main.h
	$(CC) main.cpp $(CC_FLAGS)

msg.o: msg.cpp msg.h
	$(CC) msg.cpp $(CC_FLAGS)
	
core.o: core.cpp core.h
	$(CC) core.cpp $(CC_FLAGS)

net.o: net.cpp net.h
	$(CC) net.cpp $(CC_FLAGS)

gfx.o: gfx.cpp gfx.h
	$(CC) gfx.cpp $(CC_FLAGS)

engine.o: engine.cpp engine.h
	$(CC) engine.cpp $(CC_FLAGS)

gui.o: gui.cpp gui.h
	$(CC) gui.cpp $(CC_FLAGS)

event.o: event.cpp event.h
	$(CC) event.cpp $(CC_FLAGS)

gui_button.o: gui_button.cpp gui_button.h
	$(CC) gui_button.cpp $(CC_FLAGS)

gui_text.o: gui_text.cpp gui_text.h
	$(CC) gui_text.cpp $(CC_FLAGS)
	
gui_style.o: gui_style.cpp gui_style.h
	$(CC) gui_style.cpp $(CC_FLAGS)

#######################################################################
# Other
#######################################################################
all: $(OBJS) $(BIN) docu

new: clean all

release: all strip
	rm -f $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
	rm -fr doc/html
	rm -fr doc/latex

strip:
	strip $(BIN)

docu:
	doxygen

install:
	cp $(BIN) $(USR)/lib/$(BIN)
	cp $(INCLUDE) $(USR)/include/