package.name = "chat-server"
package.kind = "exe"
package.language = "c++"

package.target = "../bin/chat"

package.files = {
	"chat_server.cpp",
	"chat_server.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32"})
	tinsert(package.links, {"opengl32", "glu32", "glaux", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode"})
else
	tinsert(package.includepaths, { "/usr/include/freetype2", "/usr/include/FTGL", "../../src"})
	tinsert(package.libpaths, {"../../bin", findlib("GL"), findlib("GLU"), findlib("Xxf86vm"), findlib("SDL"), findlib("ftgl"), findlib("ode")})
	tinsert(package.buildoptions, "`sdl-config --cflags` -Wall ")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl", "a2e"})
end