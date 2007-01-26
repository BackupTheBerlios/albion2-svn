/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "texman.h"

/*! creates the texman object
 */
texman::texman(msg* m, file_io* f) {
	ctextures = 0;
	textures = NULL;

	filter[0] = GL_NEAREST;
	filter[1] = GL_LINEAR;
	filter[2] = GL_LINEAR_MIPMAP_NEAREST;
	filter[3] = GL_LINEAR_MIPMAP_LINEAR;

	cur_texture = NULL;

	// get msg, file_io and core class ...
	texman::m = m;
	texman::f = f;
}

/*! deletes the texman object
 */
texman::~texman() {
	for(unsigned int i = 0; i < ctextures; i++) {
		glDeleteTextures(1, &textures[i].tex);
	}

	delete [] textures;
}

unsigned int texman::add_texture(const char* filename, GLint components, GLenum format, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLenum type) {
	// create a sdl surface and load the texture
	SDL_Surface* tex_surface = IMG_Load(filename);
	if(tex_surface == NULL) {
		m->print(msg::MERROR, "texman.cpp", "add_texture(): error loading texture file \"%s\"!", filename);
		return 0;
	}

	cur_texture = tex_surface;
	unsigned int ret = add_texture(tex_surface->pixels, tex_surface->w, tex_surface->h, components, format, filtering, wrap_s, wrap_t, type, filename);
	cur_texture = NULL;

	// delete the sdl surface, b/c it isn't needed any more
	SDL_FreeSurface(tex_surface);

	return ret;
}

/*! adds a texture and returns its (internal?) number
 *  @param filename the textures file name
 *  @param components the textures components (number of colors)
 *  @param format the textures format (GL_RGB, GL_LUMINANCE, ...)
 */
unsigned int texman::add_texture(void* pixel_data, unsigned int width, unsigned int height, GLint components, GLenum format, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLenum type, const char* filename) {
	// if "automatic filtering" is specified, use standard filtering (as set in config.xml)
	if(filtering == texman::TF_AUTOMATIC) filtering = standard_filtering;

	// look if we already loaded this texture
	for(unsigned int i = 0; i < ctextures; i++) {
		if((strcmp(filename, "") == 0 || strcmp(textures[i].filename.c_str(), filename) == 0) &&
			textures[i].height == height &&
			textures[i].width == width &&
			components == textures[i].components &&
			format == textures[i].format &&
			filtering == textures[i].filtering &&
			wrap_s == textures[i].wrap_s &&
			wrap_t == textures[i].wrap_t &&
			type == textures[i].type) {
			// we already loaded the texture, so just return its number
			return textures[i].tex;
		}
	}

	// create a new texture element
	texture* new_textures = new texture[ctextures+1];
	for(unsigned int i = 0; i < ctextures; i++) {
		new_textures[i] = textures[i];
	}
	new_textures[ctextures].filename = filename;
	new_textures[ctextures].height = height;
	new_textures[ctextures].width = width;
	new_textures[ctextures].tex = 0;
	new_textures[ctextures].components = components;
	new_textures[ctextures].format = format;
	new_textures[ctextures].filtering = filtering;
	new_textures[ctextures].wrap_s = wrap_s;
	new_textures[ctextures].wrap_t = wrap_t;
	new_textures[ctextures].type = type;
	delete [] textures;
	textures = new_textures;

	// now create/generate an opengl texture and bind it
	glGenTextures(1, &texman::textures[ctextures].tex);
	glBindTexture(GL_TEXTURE_2D, texman::textures[ctextures].tex);

	// build mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering == 0 ? GL_NEAREST : GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter[filtering]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

	if(cur_texture == NULL || components <= cur_texture->format->BytesPerPixel) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, components, width, height, format, type, pixel_data);
	}
	else {
		m->print(msg::MERROR, "texman.cpp", "add_texture(): your chosen components count (%u) exceeds available component count(%u)!", components, cur_texture->format->BytesPerPixel);

		GLint new_format = 0;
		// RGB
		if(cur_texture->format->Rshift == 0 && cur_texture->format->Gshift == 8 &&
			cur_texture->format->Bshift == 16 && cur_texture->format->Ashift == 0) new_format = GL_RGB;
		// RGBA
		else if(cur_texture->format->Rshift == 0 && cur_texture->format->Gshift == 8 &&
			cur_texture->format->Bshift == 16 && cur_texture->format->Ashift == 24) new_format = GL_RGBA;
		// LUMINANCE
		else if(cur_texture->format->Rshift == 0 && cur_texture->format->Gshift == 0 &&
			cur_texture->format->Bshift == 0 && cur_texture->format->Ashift == 0) new_format = GL_LUMINANCE;
		// unknown format, use RGB
		else new_format = GL_RGB;

		gluBuild2DMipmaps(GL_TEXTURE_2D, cur_texture->format->BytesPerPixel, width, height,
			new_format, type, pixel_data);

		new_textures[ctextures].components = cur_texture->format->BytesPerPixel;
		new_textures[ctextures].format = new_format;
	}

	// increment texture count
	ctextures++;

	return textures[ctextures-1].tex;
}

/*! adds a cubemap texture and returns its (internal?) number
 *  @param filenames the textures file names
 *  @param components the textures components (number of colors)
 *  @param format the textures format (GL_RGB, GL_LUMINANCE, ...)
 */
unsigned int texman::add_cubemap_texture(void** pixel_data, unsigned int width, unsigned int height, GLint components, GLenum format, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLenum type) {
	// if "automatic filtering" is specified, use standard filtering (as set in config.xml)
	if(filtering == texman::TF_AUTOMATIC) filtering = standard_filtering;

	// TODO: look if we already loaded this texture

	// create a new texture element
	texture* new_textures = new texture[ctextures+1];
	for(unsigned int i = 0; i < ctextures; i++) {
		new_textures[i] = textures[i];
	}
	//new_textures[ctextures].filename = filenames[0];
	new_textures[ctextures].filename = "";
	new_textures[ctextures].height = height;
	new_textures[ctextures].width = width;
	new_textures[ctextures].tex = 0;
	new_textures[ctextures].components = components;
	new_textures[ctextures].format = format;
	new_textures[ctextures].filtering = filtering;
	new_textures[ctextures].wrap_s = wrap_s;
	new_textures[ctextures].wrap_t = wrap_t;
	new_textures[ctextures].type = type;
	delete [] textures;
	textures = new_textures;

	// now create/generate an opengl texture and bind it
	glGenTextures(1, &texman::textures[ctextures].tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texman::textures[ctextures].tex);

	// build mipmaps
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (filtering == 0 ? GL_NEAREST : GL_LINEAR));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter[filtering]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);

	GLenum cmap[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	for(unsigned int i = 0; i < 6; i++) {
		//gluBuild2DMipmaps(cmap[i], components, width, height, format, type, pixel_data[i]);
		glTexImage2D(cmap[i], 0, format, width, height, 0, GL_RGBA, type, pixel_data[i]);
	}

	// increment texture count
	ctextures++;

	return textures[ctextures-1].tex;
}

unsigned int texman::add_cubemap_texture(const char** filenames, GLint components, GLenum format, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLenum type) {
	// if "automatic filtering" is specified, use standard filtering (as set in config.xml)
	if(filtering == texman::TF_AUTOMATIC) filtering = standard_filtering;

	// create a sdl surface and load the texture
	SDL_Surface** tex_surface = new SDL_Surface*[6];
	for(unsigned int i = 0; i < 6; i++) {
		tex_surface[i] = new SDL_Surface();
		tex_surface[i] = IMG_Load(filenames[i]);
		if(!tex_surface[i]) {
			m->print(msg::MERROR, "texman.cpp", "add_cubemap_texture(): error loading texture file \"%s\"!", filenames[i]);
			return 0;
		}
	}

	void** pixel_data = new void*[6];
	for(unsigned int i = 0; i < 6; i++) {
		pixel_data[i] = tex_surface[i]->pixels;
	}
	unsigned int ret = add_cubemap_texture(pixel_data, tex_surface[0]->w, tex_surface[0]->h, components, format, filtering, wrap_s, wrap_t, type);
	delete [] pixel_data;

	// delete the sdl surface, b/c it isn't needed any more
	for(unsigned int i = 0; i < 6; i++) {
		if(tex_surface[i]) {
			SDL_FreeSurface(tex_surface[i]);
		}
	}

	return ret;
}

unsigned int texman::add_cubemap_texture(const char* filename, GLint components, GLenum format, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLenum type) {
	// if "automatic filtering" is specified, use standard filtering (as set in config.xml)
	if(filtering == texman::TF_AUTOMATIC) filtering = standard_filtering;

	unsigned int ret = 0;

	int len = (int)strlen(filename);
	void** pixel_data = new void*[6];
	bool hdr_texture = (filename[len-3] == 'h' && filename[len-2] == 'd' && filename[len-1] == 'r');
	SDL_Surface* cubemap;
	unsigned int width = 0, height = 0;
	if(!hdr_texture) {
		// create a sdl surface and load the texture
		cubemap = IMG_Load(filename);
		if(cubemap == NULL) {
			m->print(msg::MERROR, "texman.cpp", "add_texture(): error loading texture file \"%s\"!", filename);
			return 0;
		}
		SDL_Surface** tex_surface = new SDL_Surface*[6];
		for(unsigned int i = 0; i < 6; i++) {
			pixel_data[i] = tex_surface[i]->pixels;
		}
		width = tex_surface[0]->w;
		height = tex_surface[0]->h;
	}
	else {
		// get file data
		stringstream buffer;
		f->open_file(filename, file_io::OT_READ_BINARY);
		unsigned int filesize = f->get_filesize();
		char* file_data = new char[filesize];
		f->get_block(file_data, filesize);
		f->close_file();
		buffer.write(file_data, filesize);

		// get textures width and height
		buffer.seekg((long)buffer.str().find("-Y", 0)+3, ios::beg);
		buffer >> height;
		buffer.seekg((long)buffer.str().find("+X", 0)+3, ios::beg);
		buffer >> width;
		buffer.clear(); // not needed any longer ...

		// read out hdr texture (float) data
		size_t pos = 1 + buffer.tellg();
		unsigned char* cbuffer = new unsigned char[width*height*4];
		unsigned char c;
		unsigned int clen;
		int pos_rgbe[4];
		pos_rgbe[0] = 0;
		pos_rgbe[1] = 1;
		pos_rgbe[2] = 2;
		pos_rgbe[3] = 3;
		for(unsigned int i = 0; i < height; i++) {
			pos += 4;
			for(unsigned int j = 0; j < 4; j++) {
				clen = 0;
				while(clen < width) {
					//ch = buffer.str()[pos]; // needs too much time, probably b/c of boundary checks?
					c = file_data[pos];
					if(c > 0x80) {
						c -= 0x80;
						pos++;
						for(unsigned char k = 0; k < c; k++) {
							//cbuffer[pos_rgbe[j]] = buffer.str()[pos];
							cbuffer[pos_rgbe[j]] = file_data[pos];
							pos_rgbe[j] += 4;
							clen++;
						}
						pos++;
					}
					else {
						pos++;
						for(unsigned char k = 0; k < c; k++) {
							//cbuffer[pos_rgbe[j]] = buffer.str()[pos];
							cbuffer[pos_rgbe[j]] = file_data[pos];
							pos_rgbe[j] += 4;
							pos++;
							clen++;
						}
					}
				}
			}
		}
		delete [] file_data;

		// convert data to floating point
		vertex3* pixel_fdata = new vertex3[width*height];
		for(unsigned int i = 0; i < (width*height*4); i+=4) {
			if(cbuffer[i+3] != 0) {
				pixel_fdata[i/4].set(cbuffer[i], cbuffer[i+1], cbuffer[i+2]);
				pixel_fdata[i/4] *= pow(2.0f, (cbuffer[i+3] - 136));
			}
			//else ... vertex3 x, y and z are set to 0.0f by default
		}
		delete [] cbuffer;

		// now split up the texture into six parts
		// cubemap has a cross form:
		//    [4]
		// [3][5][0]
		//    [1]
		//    [2]

		unsigned int side_width = width / 3;
		unsigned int side_height = height / 4;
		vertex3** sides_v3 = new vertex3*[6];
		for(unsigned int i = 0; i < 6; i++) sides_v3[i] = new vertex3[side_width*side_height];

		unsigned int sx[6], sy[6];
		bool sflip[6];

		sx[0] = side_width*2;
		sy[0] = side_height;
		sflip[0] = true;

		sx[1] = side_width;
		sy[1] = side_height*2;
		sflip[1] = false;

		sx[2] = side_width;
		sy[2] = side_height*3;
		sflip[2] = false;

		sx[3] = 0;
		sy[3] = side_height;
		sflip[3] = true;

		sx[4] = side_width;
		sy[4] = 0;
		sflip[4] = false;

		sx[5] = side_width;
		sy[5] = side_height;
		sflip[5] = true;

		// copy data
		for(unsigned int i = 0; i < 6; i++) {
			// don't flip vertically and horizontally
			if(!sflip[i]) {
				for(unsigned int j = 0; j < side_height; j++) {
					for(unsigned int k = 0; k < side_width; k++) {
						sides_v3[i][j*side_width+k] = pixel_fdata[(sy[i]+j)*width+sx[i]+k];
					}
				}
			}
			// flip vertically and horizontally
			else {
				for(unsigned int j = side_height; j > 0; j--) {
					for(unsigned int k = side_width; k > 0; k--) {
						sides_v3[i][(side_height-j)*side_width+(side_width-k)] = pixel_fdata[(sy[i]+(j-1))*width+sx[i]+(k-1)];
					}
				}
			}
		}
		delete [] pixel_fdata;

		// convert to RGBS16
		float rgb_scale = float(1 << 16);
		float range_scale = float(1 << 16);

		// find largest value in the image
		float max_value = 0;
		for(unsigned int i = 0; i < 6; i++) {
			for(unsigned int j = 0; j < side_width*side_height; j++){
				if(sides_v3[i][j].x > max_value) max_value = sides_v3[i][j].x;
				if(sides_v3[i][j].y > max_value) max_value = sides_v3[i][j].y;
				if(sides_v3[i][j].z > max_value) max_value = sides_v3[i][j].z;
			}
		}

		unsigned short** sides = new unsigned short*[6];
		for(unsigned int i = 0; i < 6; i++) sides[i] = new unsigned short[side_width*side_height*4];

		for(unsigned int i = 0; i < 6; i++) {
			for(unsigned int j = 0; j < side_width*side_height; j++) {
				float r = min(sides_v3[i][j].x, max_value);
				float g = min(sides_v3[i][j].y, max_value);
				float b = min(sides_v3[i][j].z, max_value);

				float max_channel = max(max(r, g), b);

				r /= max_channel;
				g /= max_channel;
				b /= max_channel;
				float range = max_channel / max_value;

				float x = sqrtf(range * range_scale / rgb_scale);
				r *= x;
				g *= x;
				b *= x;
				range /= x;

				sides[i][4 * j + 0] = (unsigned short)(65535 * r);
				sides[i][4 * j + 1] = (unsigned short)(65535 * g);
				sides[i][4 * j + 2] = (unsigned short)(65535 * b);
				sides[i][4 * j + 3] = (unsigned short)(65535 * range);
			}
		}
		delete [] sides_v3;

		ret = add_cubemap_texture((void**)sides, side_width, side_height, 4, GL_RGBA16, texman::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_UNSIGNED_SHORT);

		textures[ctextures-1].max_value = max_value;

		delete [] sides;
	}

	// --- now split up the texture into six parts and put each part into a new sdl surface

	//unsigned int ret = add_cubemap_texture(pixel_data, width, height, components, format, filtering, wrap_s, wrap_t, wrap_r, type);

	// delete all sdl surfaces
	if(!hdr_texture) SDL_FreeSurface(cubemap);

	return ret;
}

/*! returns the texture with the tex number num
 *  @param num the textures num we want to search for
 */
texman::texture* texman::get_texture(unsigned int num) {
	for(unsigned int i = 0; i < ctextures; i++) {
		if(textures[i].tex == num) {
			return &textures[i];
		}
	}
	m->print(msg::MERROR, "texman.cpp", "get_texture(): couldn't find texture #%u", num);
	return 0;
}

void texman::set_filtering(unsigned int filtering) {
	if(filtering > texman::TF_TRILINEAR) {
		m->print(msg::MERROR, "texman.cpp", "set_filtering(): unknown texture filtering mode (%u)!", filtering);
		texman::standard_filtering = texman::TF_POINT;
		return;
	}
	texman::standard_filtering = (texman::TEXTURE_FILTERING)filtering;
}
