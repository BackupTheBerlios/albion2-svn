package.name = "gui"
package.kind = "exe"
package.language = "c++"

package.target = "../bin/gui"

package.files = {
	"gui_sample.cpp",
	"gui_sample.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32"})
	tinsert(package.links, {"opengl32", "glu32", "glaux", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode"})
else
	tinsert(package.includepaths, {"../../src", "/usr/include/freetype2", "/usr/include/FTGL"})
	tinsert(package.libpaths, {findlib("Xxf86vm"), "jpeglib", "zlib", "../../bin",})
	tinsert(package.buildoptions, "-Os -c `sdl-config --cflags`")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "a2e", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl"})
end