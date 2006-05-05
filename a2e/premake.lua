package.name = "a2e"
package.language = "c++"
package.target = "a2e/lib/a2e"

package.files = {
	matchfiles("src/*.h", "src/*.cpp")
}

if (OS == "windows") then
	package.kind = "dll"
	tinsert(package.defines, {"WIN32", "_CONSOLE", "A2E_EXPORTS", "_CRT_SECURE_NO_DEPRECATE"})
	tinsert(package.config["Debug"].defines, { "_DEBUG" })
	tinsert(package.config["Release"].defines, { "NDEBUG" })
	tinsert(package.links, {"opengl32", "glu32", "glaux", "odbc32", "odbccp32", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode"})
else
	package.kind = "lib"
	tinsert(package.includepaths, {"/usr/include", "/usr/include/freetype2", "/usr/include/FTGL", "/usr/include/ode", "/usr/include/opcode", "/usr/include/opcode/ICE"})
	tinsert(package.libpaths, {findlib("GL"), findlib("GLU"), findlib("Xxf86vm"), findlib("SDL"), findlib("ftgl"), findlib("ode"), findlib("OPCODE"), findlib("lua"), findlib("xml2")})
	tinsert(package.buildoptions, "`sdl-config --cflags` -Wall")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "SDL", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl", "lua", "lualib", "xml2"})
end