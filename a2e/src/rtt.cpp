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

#include "rtt.h"

/*! there is no function currently
 */
rtt::rtt(msg* m, core* c, gfx* g, ext* exts, unsigned int screen_width, unsigned int screen_height) {
	// get classes
	rtt::c = c;
	rtt::m = m;
	rtt::g = g;
	rtt::exts = exts;

	current_buffer = NULL;

	filter[0] = GL_NEAREST;
	filter[1] = GL_LINEAR;
	filter[2] = GL_LINEAR_MIPMAP_NEAREST;
	filter[3] = GL_LINEAR_MIPMAP_LINEAR;

	rtt::screen_width = screen_width;
	rtt::screen_height = screen_height;
}

/*! there is no function currently
 */
rtt::~rtt() {
	m->print(msg::MDEBUG, "rtt.cpp", "freeing rtt stuff");

	if(exts->is_fbo_support()) {
		exts->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	for(list<rtt::fbo*>::iterator iter = buffers.begin(); iter != buffers.end(); iter++) {
		glDeleteTextures(1, &(*iter)->tex_id);
		exts->glDeleteFramebuffersEXT(1, &(*iter)->fbo_id);
		delete (*iter);
	}
	buffers.clear();

	m->print(msg::MDEBUG, "rtt.cpp", "rtt stuff freed");
}

rtt::fbo* rtt::add_buffer(unsigned int width, unsigned int height, TEXTURE_FILTERING filtering, GLint wrap_s, GLint wrap_t, GLint internal_format, GLenum format, GLenum type, bool depth) {
	if(exts->is_fbo_support()) {
		buffers.push_back(*new rtt::fbo*());
		buffers.back() = new rtt::fbo();
		buffers.back()->width = width;
		buffers.back()->height = height;

		exts->glGenFramebuffersEXT(1, &buffers.back()->fbo_id);

		glGenTextures(1, &buffers.back()->tex_id);
		glBindTexture(GL_TEXTURE_2D, buffers.back()->tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering == 0 ? GL_NEAREST : GL_LINEAR));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter[filtering]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, NULL);
		if(filtering > TF_LINEAR) {
			buffers.back()->auto_mipmap = true;
			exts->glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		else buffers.back()->auto_mipmap = false;

		current_buffer = buffers.back();
		check_fbo();

		if(depth) {
			buffers.back()->depth = true;
			exts->glGenRenderbuffersEXT(1, &buffers.back()->depth_buffer);
			exts->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffers.back()->depth_buffer);
			exts->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);

			check_fbo();

			exts->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		}
		else buffers.back()->depth = false;

		return buffers.back();
	}
	return NULL;
}

void rtt::delete_buffer(rtt::fbo* buffer) {
	if(exts->is_fbo_support()) {
		glDeleteTextures(1, &buffer->tex_id);
		exts->glDeleteFramebuffersEXT(1, &buffer->fbo_id);
		delete buffer;
		buffers.remove(buffer);
	}
}

void rtt::start_draw(rtt::fbo* buffer) {
	current_buffer = buffer;
	if(exts->is_fbo_support()) {
		exts->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, buffer->fbo_id);
		exts->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, buffer->tex_id, 0);
		if(buffer->depth) exts->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, buffer->depth_buffer);
		else exts->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
		glViewport(0, 0, buffer->width, buffer->height);
	}
}

void rtt::stop_draw() {
	if(exts->is_fbo_support()) {
		check_fbo();

		exts->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		if(current_buffer->auto_mipmap) {
			glBindTexture(GL_TEXTURE_2D, current_buffer->tex_id);
			exts->glGenerateMipmapEXT(GL_TEXTURE_2D);
		}

		glViewport(0, 0, rtt::screen_width, rtt::screen_height);
	}
}

void rtt::start_2d_draw() {
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glViewport(0, 0, current_buffer->width, current_buffer->height);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// we need an orthogonal view (2d) for drawing 2d elements
	glOrtho(0, current_buffer->width, 0, current_buffer->height, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	g->set_flip(true);
}

void rtt::stop_2d_draw() {
	g->set_flip(false);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void rtt::clear() {
	if(exts->is_fbo_support()) {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		if(current_buffer->depth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else glClear(GL_COLOR_BUFFER_BIT);
	}
}

void rtt::copy_buffer(rtt::fbo* src_buffer, rtt::fbo* dest_buffer) {
	if(exts->is_fbo_support()) {
		start_draw(dest_buffer);
		start_2d_draw();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, src_buffer->tex_id);
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(dest_buffer->width, 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(dest_buffer->width, dest_buffer->height);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(0, dest_buffer->height);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		stop_2d_draw();
		stop_draw();
	}
}

void rtt::check_fbo() {
	if(exts->is_fbo_support()) {
		GLint status = exts->glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			m->print(msg::MERROR, "rtt.cpp", "check_fbo(): framebuffer (size: %u*%upx; depth: %i; tex id: %u; fbo id: %u) didn't pass status check!",
				current_buffer->width, current_buffer->height, current_buffer->depth, current_buffer->tex_id, current_buffer->fbo_id);
		}

		switch(status) {
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): unsupported framebuffer (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): incomplete framebuffer attachement (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): missing framebuffer attachement (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): duplicate framebuffer attachement (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): incomplete framebuffer dimensions (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): incomplete framebuffer formats (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): incomplete framebuffer draw buffer (%u)!", status);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): incomplete framebuffer read buffer (%u)!", status);
				break;
			default:
				m->print(msg::MERROR, "rtt.cpp", "check_fbo(): unknown framebuffer error (%u)!", status);
				break;
		}
	}
}
