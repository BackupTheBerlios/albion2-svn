package.name = "a2e"
package.language = "c++"
package.target = "a2e/lib/a2e"

package.files = {
	matchfiles("src/*.h", "src/*.cpp")
}

if (OS == "windows") then
	package.kind = "dll"
	package.defines = {"WIN32", "_CONSOLE", "A2E_EXPORTS", "_CRT_SECURE_NO_DEPRECATE"}
	package.config["Debug"].defines = { "_DEBUG" }
	package.config["Release"].defines = { "NDEBUG" }
	package.config["Release_AMD"].defines = { "NDEBUG" }
	package.links = {"opengl32", "glu32", "glaux", "odbc32", "odbccp32", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode"}
else
	package.kind = "lib"
	package.includepaths = {"/usr/include", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include/ode", "/usr/include/opcode", "/usr/include/opcode/ICE"}
	package.libpaths = {findlib("GL"), findlib("GLU"), findlib("Xxf86vm"), findlib("SDL"), findlib("ftgl"), findlib("ode"), findlib("lua"), findlib("xml2"), findlib("gomp")}
	package.buildoptions = { "`sdl-config --cflags` -Wall -fopenmp -pipe -floop-optimize" }
	package.config["Release"].buildoptions = { " -march=i686" }
	package.config["Release_AMD"].buildoptions = { "-march=athlon64 -ffast-math -ftracer -O3 -msse3 -frename-registers -fweb -finline-functions" }
	package.linkoptions = { "-fPIC `sdl-config --libs`" }
	package.links = {"stdc++", "GL", "GLU", "Xxf86vm", "z", "jpeg", "SDL", "SDL_net", "SDL_image", "ode", "freetype", "ftgl", "lua", "xml2", "gomp"}
end
