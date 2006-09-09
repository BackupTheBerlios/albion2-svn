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
rtt::rtt(msg* m, core* c, gfx* g, ext* exts) {
	// get classes
	rtt::c = c;
	rtt::m = m;
	rtt::g = g;
	rtt::exts = exts;

	current_buffer = NULL;
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

rtt::fbo* rtt::add_buffer(unsigned int width, unsigned int height) {
	if(exts->is_fbo_support()) {
		buffers.push_back(*new rtt::fbo*());
		buffers.back() = new rtt::fbo();
		buffers.back()->width = width;
		buffers.back()->height = height;

		exts->glGenFramebuffersEXT(1, &buffers.back()->fbo_id);

		glGenTextures(1, &buffers.back()->tex_id);
		glBindTexture(GL_TEXTURE_2D, buffers.back()->tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		GLuint status = exts->glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			m->print(msg::MERROR, "rtt.cpp", "add_buffer(): framebuffer didn't pass status check (%u)!", status);
		}
	}
	return buffers.back();
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
	}
}

void rtt::stop_draw() {
	if(exts->is_fbo_support()) {
		exts->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
