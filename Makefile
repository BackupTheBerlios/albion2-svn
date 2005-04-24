# Makefile autogenerated by premake
# Options:
#   CONFIG=[Debug|Release]

ifndef CONFIG
  CONFIG=Debug
endif

export CONFIG

.PHONY: all clean a2e obj2a2m chat_server gui model_loader move

all: a2e obj2a2m chat_server gui model_loader move

Makefile: ./premake.lua ./src/premake.lua ./tools/premake.lua ./samples/chat/premake.lua ./samples/gui/premake.lua ./samples/model/premake.lua ./samples/move/premake.lua
	@echo ==== Regenerating Makefiles ====
	@premake --cc gcc --target gnu

a2e:
	@echo ==== Building a2e ====
	@$(MAKE) --no-print-directory -C ./src/
obj2a2m:
	@echo ==== Building obj2a2m ====
	@$(MAKE) --no-print-directory -C ./tools/
chat_server: a2e
	@echo ==== Building chat_server ====
	@$(MAKE) --no-print-directory -C ./samples/chat/
gui: a2e
	@echo ==== Building gui ====
	@$(MAKE) --no-print-directory -C ./samples/gui/
model_loader: a2e
	@echo ==== Building model_loader ====
	@$(MAKE) --no-print-directory -C ./samples/model/
move: a2e
	@echo ==== Building move ====
	@$(MAKE) --no-print-directory -C ./samples/move/
clean:
	@$(MAKE) --no-print-directory -C ./src/ clean
	@$(MAKE) --no-print-directory -C ./tools/ clean
	@$(MAKE) --no-print-directory -C ./samples/chat/ clean
	@$(MAKE) --no-print-directory -C ./samples/gui/ clean
	@$(MAKE) --no-print-directory -C ./samples/model/ clean
	@$(MAKE) --no-print-directory -C ./samples/move/ clean
