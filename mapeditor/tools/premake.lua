package = newpackage()
package.name = "obj2a2m"
package.kind = "exe"
package.language = "c++"

package.files = {
	"obj2a2m.cpp",
	"obj2a2m.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32"})
	tinsert(package.links, {"opengl32", "glu32", "glaux", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode"})
else
	tinsert(package.includepaths, { "/usr/include/freetype2", "/usr/include/FTGL"})
	tinsert(package.libpaths, {findlib("GL"), findlib("GLU"), findlib("Xxf86vm"), findlib("SDL"), findlib("ftgl"), findlib("ode")})
	tinsert(package.buildoptions, "`sdl-config --cflags` -Wall ")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "SDL_net", "ode", "freetype", "ftgl"})
end

package = newpackage()
package.name = "md5mesh2a2m"
package.kind = "exe"
package.language = "c++"

package.target = "md5mesh2a2m"

package.files = {
	"md5mesh2a2m.cpp",
	"md5mesh2a2m.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32", "_CONSOLE"})
	tinsert(package.config["Debug"].defines, { "_DEBUG" })
	tinsert(package.config["Release"].defines, { "NDEBUG" })
	tinsert(package.links, {"a2e"})
else
	tinsert(package.includepaths, {"../../src", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include", "/usr/include/ode", "/usr/include/Ice"})
	tinsert(package.libpaths, {findlib("Xxf86vm"), "jpeglib", "zlib", "../../bin",})
	tinsert(package.buildoptions, "-Os -c `sdl-config --cflags`")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"a2e"})
end

package = newpackage()
package.name = "md5anim2a2a"
package.kind = "exe"
package.language = "c++"

package.target = "md5anim2a2a"

package.files = {
	"md5anim2a2a.cpp",
	"md5anim2a2a.h"
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32", "_CONSOLE"})
	tinsert(package.config["Debug"].defines, { "_DEBUG" })
	tinsert(package.config["Release"].defines, { "NDEBUG" })
	tinsert(package.links, {"a2e"})
else
	tinsert(package.includepaths, {"../../src", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include", "/usr/include/ode", "/usr/include/Ice"})
	tinsert(package.libpaths, {findlib("Xxf86vm"), "jpeglib", "zlib", "../../bin",})
	tinsert(package.buildoptions, "-Os -c `sdl-config --cflags`")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"a2e"})
end
