package.name = "a2e"
package.kind = "dll"
package.language = "c++"

package.files = {
	matchfiles("*.h", "*.cpp")
}

if (OS == "windows") then
	tinsert(package.defines, {"WIN32"})
	tinsert(package.links, {"opengl32", "glu32", "glaux", "SDL", "SDLmain", "SDL_net", "SDL_image", "ftgl_dynamic_MTD", "ode", "OPCODE"})
else
	tinsert(package.includepaths, { "/usr/include/freetype2", "/usr/include/FTGL"})
	tinsert(package.libpaths, {findlib("GL"), findlib("GLU"), findlib("Xxf86vm"), findlib("SDL"), findlib("ftgl"), findlib("ode"), findlib("OPCODE")})
	tinsert(package.buildoptions, "`sdl-config --cflags` -Wall ")
	tinsert(package.linkoptions, "`sdl-config --libs`")
	tinsert(package.links, {"GL", "GLU", "Xxf86vm", "z", "jpeg", "SDL_net", "SDL_image", "ode", "OPCODE", "freetype", "ftgl"})
end