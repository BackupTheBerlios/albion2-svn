package.name = "model"
package.kind = "exe"
package.language = "c++"

package.target = "../bin/model"

package.files = {
	"model_loader.cpp",
	"model_loader.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32"})
	tinsert(package.links, {"opengl32", "glu32", "glaux", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode", "OPCODE"})
else
	tinsert(package.includepaths, {"../../src", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include", "/usr/include/ode", "/usr/include/Ice"})
	tinsert(package.libpaths, {findlib("Xxf86vm"), "jpeglib", "zlib", "../../bin",})
	tinsert(package.buildoptions, "-Os -c `sdl-config --cflags`")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "a2e", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl"})
end