#######################################################################
# Variablen
#######################################################################
OBJS = gui_sample.o \
       chat_server.o \
       chat_client.o \
       model_loader.o
       
CC         = gcc
LD         = gcc
CC_FLAGS   = -Os -c -I../ `sdl-config --cflags`
LD_FLAGS   = -L"/usr/X11R6/lib" -L"jpeglib" -L"zlib" -L../bin -lGL -lGLU -lXxf86vm -lz -ljpeg -la2e `sdl-config --libs` -lSDL_net -lSDL_ttf -lSDL_image -o

#######################################################################
# Files
#######################################################################
gui_sample.o: gui_sample.cpp gui_sample.h
	$(CC) gui_sample.cpp $(CC_FLAGS)

chat_server.o: chat_server.cpp chat_server.h
	$(CC) chat_server.cpp $(CC_FLAGS)

chat_client.o: chat_client.cpp chat_client.h
	$(CC) chat_client.cpp $(CC_FLAGS)

model_loader.o: model_loader.cpp model_loader.h
	$(CC) model_loader.cpp $(CC_FLAGS)

gui_sample: gui_sample.o
	$(LD) gui_sample.o $(LD_FLAGS) gui_sample

chat_server: chat_server.o
	$(LD) chat_server.o $(LD_FLAGS) chat_server

chat_client: chat_client.o
	$(LD) chat_client.o $(LD_FLAGS) chat_client

model_loader: model_loader.o
	$(LD) model_loader.o $(LD_FLAGS) model_loader
