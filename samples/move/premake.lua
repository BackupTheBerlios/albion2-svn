package.name = "move"
package.kind = "exe"
package.language = "c++"

package.target = "../samples/bin/move"

package.files = {
	"move.cpp",
	"move.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32", "_CONSOLE"})
	tinsert(package.config["Debug"].defines, { "_DEBUG" })
	tinsert(package.config["Release"].defines, { "NDEBUG" })
	tinsert(package.links, {"opengl32", "glu32", "glaux", "odbc32", "odbccp32", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode", "a2e"})
else
	tinsert(package.includepaths, {"../../src", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include", "/usr/include/ode", "/usr/include/Ice"})
	tinsert(package.libpaths, {findlib("Xxf86vm"), "jpeglib", "zlib", "../../bin",})
	tinsert(package.buildoptions, "-Os -c `sdl-config --cflags`")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "a2e", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl"})
end