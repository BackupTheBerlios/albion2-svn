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
       gui_style.o \
       gui_input.o \
       gui_list.o \
       gui_list_item.o \
       gui_vbar.o \
       file_io.o \
       a2emodel.o \
       camera.o \
       scene.o \
       ode.o \
       ode_object.o \
       shadow.o \
       light.o \
       image.o \
       gui_check.o \
       gui_window.o

INCLUDE = main.h \
       msg.h \
       core.h \
       net.h \
       gfx.h \
       engine.h \
       gui.h \
       event.h \
       gui_button.h \
       gui_text.h \
       gui_style.h \
       gui_input.h \
       gui_list.h \
       gui_list_item.h \
       gui_vbar.h \
       file_io.h \
       a2emodel.h \
       camera.h \
       scene.h \
       ode.h \
       ode_object.h \
       shadow.h \
       light.h \
       image.h \
       gui_check.h \
       gui_window.h

CC         = gcc
LD         = gcc
CC_FLAGS   = -Wall -Os -c `sdl-config --cflags` -I"/usr/include/freetype2" -I"/usr/include/FTGL"
LD_FLAGS   = -L"/usr/X11R6/lib" -L"jpeglib" -L"zlib" -lstdc++ -lGL -lGLU -lXxf86vm -lz -ljpeg `sdl-config --libs` -lSDL_net -l -lSDL_image -lode -lOPCODE -lfreetype -lftgl -o
BIN        = bin/liba2e.a
LIB        = /usr/local/lib/
INC        = /usr/local/include/

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

gui_input.o: gui_input.cpp gui_input.h
	$(CC) gui_input.cpp $(CC_FLAGS)

gui_list.o: gui_list.cpp gui_list.h
	$(CC) gui_list.cpp $(CC_FLAGS)

gui_list_item.o: gui_list_item.cpp gui_list_item.h
	$(CC) gui_list_item.cpp $(CC_FLAGS)

gui_vbar.o: gui_vbar.cpp gui_vbar.h
	$(CC) gui_vbar.cpp $(CC_FLAGS)

file_io.o: file_io.cpp file_io.h
	$(CC) file_io.cpp $(CC_FLAGS)

a2emodel.o: a2emodel.cpp a2emodel.h
	$(CC) a2emodel.cpp $(CC_FLAGS)

camera.o: camera.cpp camera.h
	$(CC) camera.cpp $(CC_FLAGS)

scene.o: scene.cpp scene.h
	$(CC) scene.cpp $(CC_FLAGS)

ode.o: ode.cpp ode.h
	$(CC) ode.cpp $(CC_FLAGS)

ode_object.o: ode_object.cpp ode_object.h
	$(CC) ode_object.cpp $(CC_FLAGS)

shadow.o: shadow.cpp shadow.h
	$(CC) shadow.cpp $(CC_FLAGS)

light.o: light.cpp light.h
	$(CC) light.cpp $(CC_FLAGS)

image.o: image.cpp image.h
	$(CC) image.cpp $(CC_FLAGS)

gui_check.o: gui_check.cpp gui_check.h
	$(CC) gui_check.cpp $(CC_FLAGS)

gui_window.o: gui_window.cpp gui_window.h
	$(CC) gui_window.cpp $(CC_FLAGS)

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
	cp ./$(BIN) $(LIB)
	cp ./$(INCLUDE) $(INC)
