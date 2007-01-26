package.name = "mapeditor"
package.kind = "exe"
package.language = "c++"

package.target = "tools/bin/mapeditor"

package.files = {
	matchfiles("*.h", "*.cpp")
}

if (OS == "windows") then
	package.defines = {"WIN32", "_CONSOLE"}
	package.config["Debug"].defines = { "_DEBUG" }
	package.config["Release"].defines = { "NDEBUG" }
	package.links = {"opengl32", "glu32", "glaux", "odbc32", "odbccp32", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode", "a2e"}
else
	package.includepaths = {"../../a2e/src", "/usr/include", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include/ode", "/usr/include/opcode", "/usr/include/opcode/ICE"}
	package.libpaths = {"jpeglib", "zlib", "../../lib", findlib("GL"), findlib("GLU"), findlib("SDL"), findlib("ftgl"), findlib("ode"), findlib("lua"), findlib("xml2"), findlib("gomp")}
	package.buildoptions = { "`sdl-config --cflags` -Wall -fopenmp -pipe -floop-optimize" }
	package.config["Release"].buildoptions = { " -march=i686" }
	package.config["Release_AMD"].buildoptions = { "-march=athlon64 -ffast-math -ftracer -O3 -msse3 -frename-registers -fweb -finline-functions" }
	package.linkoptions = { "`sdl-config --libs`" }
	package.links = {"a2e", "GL", "GLU", "z", "jpeg", "SDL_net", "SDL_image", "ode", "freetype", "ftgl", "lua", "xml2", "gomp"}
end
