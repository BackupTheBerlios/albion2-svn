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

#include "scene.h"

/*! scene constructor
 */
scene::scene(engine* e, shader* s) {
	scene::clights = 0;

	scene::position = new vertex3();
	scene::position->x = 0.0f;
	scene::position->y = 0.0f;
	scene::position->z = 0.0f;

	scene::mambient = new float[4];
	scene::mdiffuse = new float[4];
	scene::mspecular = new float[4];
	//scene::mshininess = new float[4];
	scene::mshininess = 64;

	scene::mambient[0] = 1.0f;
	scene::mambient[1] = 1.0f;
	scene::mambient[2] = 1.0f;
	scene::mambient[3] = 1.0f;

	scene::mdiffuse[0] = 1.0f;
	scene::mdiffuse[1] = 1.0f;
	scene::mdiffuse[2] = 1.0f;
	scene::mdiffuse[3] = 1.0f;

	/*scene::mspecular[0] = 0.2f;
	scene::mspecular[1] = 0.2f;
	scene::mspecular[2] = 0.2f;
	scene::mspecular[3] = 1.0f;*/
	scene::mspecular[0] = 1.0f;
	scene::mspecular[1] = 1.0f;
	scene::mspecular[2] = 1.0f;
	scene::mspecular[3] = 1.0f;

	/*scene::mshininess[0] = 1.0f;
	scene::mshininess[1] = 1.0f;
	scene::mshininess[2] = 1.0f;
	scene::mshininess[3] = 1.0f;*/
	scene::mshininess = 16;

	scene::is_light = false;


	// get classes
	scene::e = e;
	scene::s = s;
	scene::c = e->get_core();
	scene::m = e->get_msg();
	scene::exts = e->get_ext();
	scene::r = e->get_rtt();

	flip = exts->is_fbo_support();


	if(e->get_init_mode() == engine::GRAPHICAL) {
		scene::skybox_tex = 0;
		scene::max_value = 0.0f;
		scene::render_skybox = false;
		proj_mat = new matrix4(1.0734261f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.4312348f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0011435f, -8.0045740f,
								0.0f, 0.0f, 1.0f, 0.0f);
		mview_mat = new matrix4();


		cur_exposure = 0;
		fframe_time = 0.0f;
		iframe_time = SDL_GetTicks();

		float screen_w = (float)e->get_screen()->w;
		float screen_h = (float)e->get_screen()->h;
		unsigned int q = 5;
		float xInc = 1.0f / screen_w;
		float yInc = 1.0f / screen_h;
		tcs_line_h = new float[q*2];
		tcs_line_v = new float[q*2];
		// h
		for(unsigned int i = 0; i < q; i++) {
			tcs_line_h[i*2+0] = (-2.0f * xInc) + ((float)i * xInc);
			tcs_line_h[i*2+1] = 0.0f;
		}
		// v
		for(unsigned int i = 0; i < q; i++) {
			tcs_line_v[i*2+0] = 0.0f;
			tcs_line_v[i*2+1] = (-2.0f * yInc) + ((float)i * yInc);
		}

		// create buffer
		if(e->get_hdr()) {
			// hdr buffer
			scene_buffer = r->add_buffer(e->get_screen()->w, e->get_screen()->h, rtt::TF_POINT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, true);
			render_buffer = r->add_buffer(e->get_screen()->w, e->get_screen()->h, rtt::TF_POINT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT, true);

			// since nvidia geforce 6 and 7 cards don't support real GL_RGB(A)16 (just fake one that is actually GL_RGB(A)8),
			// we have to use GL_RGB16F_ARB for those cards (but they unfortunately don't support filtering of this format, so
			// use a twice as big blur buffer, so the pixelated/grainy effect isn't as noticeable)
			if(e->get_hdr_rgba16()) {
				blur_buffer1 = r->add_buffer(e->get_screen()->w/2, e->get_screen()->h/2, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false);
				blur_buffer2 = r->add_buffer(e->get_screen()->w/4, e->get_screen()->h/4, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false);
				blur_buffer3 = r->add_buffer(e->get_screen()->w/4, e->get_screen()->h/4, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false);
				average_buffer = r->add_buffer(64, 64, rtt::TF_BILINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false);
			}
			else {
				blur_buffer1 = r->add_buffer(e->get_screen()->w/2, e->get_screen()->h/2, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGBA, GL_FLOAT, false);
				blur_buffer2 = r->add_buffer(e->get_screen()->w/2, e->get_screen()->h/2, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGBA, GL_FLOAT, false);
				blur_buffer3 = r->add_buffer(e->get_screen()->w/2, e->get_screen()->h/2, rtt::TF_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGBA, GL_FLOAT, false);
				average_buffer = r->add_buffer(64, 64, rtt::TF_BILINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGBA, GL_FLOAT, false);
			}

			exposure_buffer[0] = r->add_buffer(1, 1, rtt::TF_POINT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGB, GL_FLOAT, false);
			exposure_buffer[1] = r->add_buffer(1, 1, rtt::TF_POINT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB16F_ARB, GL_RGB, GL_FLOAT, false);

			// clear exposure buffer #0 (since we'll read/use it before we write sth into it)
			r->start_draw(exposure_buffer[0]);
			r->clear();
			r->stop_draw();
		}
		else if(exts->is_fbo_support()) {
			// normal scene buffer
			scene_buffer = r->add_buffer(e->get_screen()->w, e->get_screen()->h, rtt::TF_POINT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, true);
		}
	}
}

/*! scene destructor
 */
scene::~scene() {
	m->print(msg::MDEBUG, "scene.cpp", "freeing scene stuff");

	delete scene::position;
	delete [] scene::mambient;
	delete [] scene::mdiffuse;
	delete [] scene::mspecular;
	//delete [] scene::mshininess;

	m->print(msg::MDEBUG, "scene.cpp", "deleting models");
	models.clear();
	amodels.clear();

	delete proj_mat;
	delete mview_mat;

	// if hdr is supported, than fbo's are supported too, so we don't need an extra delete in the "hdr fbo delete branch"
	if(exts->is_fbo_support()) r->delete_buffer(scene_buffer);
	if(e->get_hdr()) {
		r->delete_buffer(render_buffer);
		r->delete_buffer(blur_buffer1);
		r->delete_buffer(blur_buffer2);
		r->delete_buffer(blur_buffer3);
		r->delete_buffer(average_buffer);
		r->delete_buffer(exposure_buffer[0]);
		r->delete_buffer(exposure_buffer[1]);
	}

	delete [] tcs_line_h;
	delete [] tcs_line_v;

	m->print(msg::MDEBUG, "scene.cpp", "scene stuff freed");
}

/*! draws the scene
 */
void scene::draw() {
	start_draw();
	stop_draw();
}

/*! starts drawing the scene
 */
void scene::start_draw() {
	// set light stuff
	for(unsigned int i = 0; i < clights; i++) {
		unsigned int light_num = GL_LIGHT0 + i;
        float pos[] = { lights[i]->get_position()->x, lights[i]->get_position()->y, lights[i]->get_position()->z, 1.0f };
		if(flip) pos[1] *= -1.0f;
		glLightfv(light_num, GL_POSITION, pos);
		glLightfv(light_num, GL_AMBIENT, lights[i]->get_lambient());
		glLightfv(light_num, GL_DIFFUSE, lights[i]->get_ldiffuse());
		glLightfv(light_num, GL_SPECULAR, lights[i]->get_lspecular());
		//glLightfv(light_num, GL_SPOT_DIRECTION, lights[i]->get_spot_direction());
		//glLightf(light_num, GL_SPOT_CUTOFF, lights[i]->get_spot_cutoff());
		glLightf(light_num, GL_CONSTANT_ATTENUATION, lights[i]->get_constant_attenuation());
		glLightf(light_num, GL_LINEAR_ATTENUATION, lights[i]->get_linear_attenuation());
		glLightf(light_num, GL_QUADRATIC_ATTENUATION, lights[i]->get_quadratic_attenuation());
	}

	if(scene::is_light) {
        glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		glMaterialfv(GL_FRONT, GL_AMBIENT, scene::mambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, scene::mdiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, scene::mspecular);
		//glMaterialfv(GL_FRONT, GL_SHININESS, scene::mshininess);
		glMateriali(GL_FRONT, GL_SHININESS, scene::mshininess);
		//glMateriali(GL_FRONT, GL_SHININESS, 128);
	}
	else {
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	// pre processing
	preprocess();

	// render models
	for(list<a2eanim*>::iterator iter = amodels.begin(); iter != amodels.end(); iter++) {
		(*iter)->set_light_position(lights[0]->get_position());
		(*iter)->draw();
	}
	for(list<a2emodel*>::iterator iter = models.begin(); iter != models.end(); iter++) {
		(*iter)->set_light_position(lights[0]->get_position());
		(*iter)->draw();
	}

	// render skybox
	if(render_skybox) {
		mview_mat->rotate(-c->deg_to_rad(e->get_rotation()->x), c->deg_to_rad(e->get_rotation()->y));
		*mview_mat *= *proj_mat;
		mview_mat->invert();

		sb_v0.set(0.0f, 2.0f, 1.0f);
		sb_v1.set(3.0f, -1.0f, 1.0f);
		sb_v2.set(-3.0f, -1.0f, 1.0f);
		sb_v0 *= mview_mat;
		sb_v1 *= mview_mat;
		sb_v2 *= mview_mat;

		if(exts->is_shader_support()) {
			s->use_shader(shader::SKYBOX_HDR);
			s->set_uniform1i(0, 0);
			s->set_uniform1f(1, 166.0f);

			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);

			glBegin(GL_TRIANGLES);
				glTexCoord3fv(&sb_v0.x);
				glVertex4f(0, 2, 1, 1);
				glTexCoord3fv(&sb_v1.x);
				glVertex4f(-3, -1, 1, 1);
				glTexCoord3fv(&sb_v2.x);
				glVertex4f(3, -1, 1, 1);
			glEnd();

			glDisable(GL_TEXTURE_CUBE_MAP);
			s->use_shader(shader::NONE);
		}
		else {
			// TODO: render skybox for no glsl capable graphic cards
		}
	}

	// post processing
	postprocess();

	// render physical objects
	if(exts->is_fbo_support()) {
		r->start_draw(scene_buffer);

		glFrontFace(GL_CW);
	}
	for(list<a2emodel*>::iterator iter = models.begin(); iter != models.end(); iter++) {
		if((*iter)->get_draw_phys_obj()) (*iter)->draw_phys_obj();
	}
	if(exts->is_fbo_support()) {
		glFrontFace(GL_CCW);
		r->stop_draw();
	}

	// "open" scene buffer for non engine internal rendering stuff
	if(exts->is_fbo_support()) {
		r->start_draw(scene_buffer);
		glFrontFace(GL_CW);
	}
}

/*! stops drawing the scene (and output the result (when using fbos ...))
 */
void scene::stop_draw() {
	// close scene buffer
	if(exts->is_fbo_support()) {
		glFrontFace(GL_CCW);
		glScalef(1.0, 1.0, 1.0);
		r->stop_draw();
	}

	// and finally render scene buffer, if hdr rendering or normal fbo rendering is used
	if(exts->is_fbo_support()) {
		// render scene buffer
		e->start_2d_draw();
		glEnable(GL_TEXTURE_2D);
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glFrontFace(GL_CW);

		glBindTexture(GL_TEXTURE_2D, scene_buffer->tex_id);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(e->get_screen()->w, 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(e->get_screen()->w, e->get_screen()->h);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(0, e->get_screen()->h);
		glEnd();

		glFrontFace(GL_CCW);
		glDisable(GL_TEXTURE_2D);
		e->stop_2d_draw();
	}
}

/*! adds a static model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2emodel* model) {
	models.push_back(model);
}

/*! adds an animated model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2eanim* model) {
	amodels.push_back(model);
}

/*! removes a model from the scene
 *  @param model pointer to the model
 */
void scene::delete_model(a2emodel* model) {
	for(list<a2emodel*>::iterator iter = models.begin(); iter != models.end(); iter++) {
		if((*iter) == model) {
			// shouldn't be deleted automatically
			//delete (*iter);
			models.erase(iter);
			return;
		}
	}
	m->print(msg::MERROR, "scene.cpp", "can't delete model: model doesn't exist!");
}

/*! removes an animated model from the scene
 *  @param model pointer to the animated model
 */
void scene::delete_model(a2eanim* model) {
	for(list<a2eanim*>::iterator iter = amodels.begin(); iter != amodels.end(); iter++) {
		if((*iter) == model) {
			// shouldn't be deleted automatically
			//delete (*iter);
			amodels.erase(iter);
			return;
		}
	}
	m->print(msg::MERROR, "scene.cpp", "can't delete animated model: model doesn't exist!");
}

/*! adds a light to the scene
 *  @param light pointer to the light
 */
void scene::add_light(light* light) {
	// enable light automatically if we had no light before
	if(scene::clights == 0) {
		scene::is_light = true;
	}

	// num = 0x4000 (GL_LIGHT0) + current amount of lights
	unsigned int light_num = GL_LIGHT0 + clights;
	lights[clights] = light;
	float pos[] = { lights[clights]->get_position()->x, lights[clights]->get_position()->y, lights[clights]->get_position()->z, 1.0f };
	glLightfv(light_num, GL_POSITION, pos);
	glLightfv(light_num, GL_AMBIENT, lights[clights]->get_lambient());
	glLightfv(light_num, GL_DIFFUSE, lights[clights]->get_ldiffuse());
	glLightfv(light_num, GL_SPECULAR, lights[clights]->get_lspecular());
	//glLightfv(light_num, GL_SPOT_DIRECTION, lights[clights]->get_spot_direction());
	//glLightf(light_num, GL_SPOT_CUTOFF, lights[clights]->get_spot_cutoff());
	//glLightf(light_num, GL_SPOT_EXPONENT, 25.0f);
	// ( Kc + Kl*d + Kq*d*d )
	glLightf(light_num, GL_CONSTANT_ATTENUATION, lights[clights]->get_constant_attenuation());
    glLightf(light_num, GL_LINEAR_ATTENUATION, lights[clights]->get_linear_attenuation());
    glLightf(light_num, GL_QUADRATIC_ATTENUATION, lights[clights]->get_quadratic_attenuation());
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glEnable(light_num);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	clights++;
}

/*! sets the scenes position
 *  @param x x coordinate
 *  @param y y coordinate
 *  @param z z coordinate
 */
void scene::set_position(float x, float y, float z) {
	scene::position->x = x;
	scene::position->y = y;
	scene::position->z = z;

	for(list<a2emodel*>::iterator iter = models.begin(); iter != models.end(); iter++) {
		(*iter)->set_position((*iter)->get_position()->x + scene::position->x,
							(*iter)->get_position()->y + scene::position->y,
							(*iter)->get_position()->z + scene::position->z);
	}
}

/*! sets the scenes ambient material
 *  @param mambient the scenes  material (vertex4)
 */
void scene::set_mambient(float* mambient) {
	scene::mambient = mambient;
}

/*! sets the scenes diffuse material
 *  @param mdiffuse the scenes diffuse material (vertex4)
 */
void scene::set_mdiffuse(float* mdiffuse) {
	scene::mdiffuse = mdiffuse;
}

/*! sets the scenes specular material
 *  @param mspecular the scenes specular material (vertex4)
 */
void scene::set_mspecular(float* mspecular) {
	scene::mspecular = mspecular;
}

/*! sets the scenes shininess material
 *  @param mshininess the scenes shininess material (vertex4)
 */
/*void scene::set_mshininess(float* mshininess) {
	scene::mshininess = mshininess;
}*/
void scene::set_mshininess(int mshininess) {
	scene::mshininess = mshininess;
}

/*! sets the light flag
 *  @param state the state of the light flag we want to set
 */
void scene::set_light(bool state) {
	scene::is_light = state;
}

/*! returns the scenes position
 */
vertex3* scene::get_position() {
	return scene::position;
}

/*! returns the scenes ambient material
 */
float* scene::get_mambient() {
	return scene::mambient;
}

/*! returns the scenes diffuse material
 */
float* scene::get_mdiffuse() {
	return scene::mdiffuse;
}

/*! returns the scenes specular material
 */
float* scene::get_mspecular() {
	return scene::mspecular;
}

/*! returns the scenes shininess material
 */
/*float* scene::get_mshininess() {
	return scene::mshininess;
}*/

/*! returns true if the light delete flag is set
 */
bool scene::get_light() {
	return scene::is_light;
}

/*! creates an a2eanim object and returns it
 */
a2eanim* scene::create_a2eanim() {
	a2eanim* a2ea = new a2eanim(e, s);
	return a2ea;
}

/*! creates an a2emodel object and returns it
 */
a2emodel* scene::create_a2emodel() {
	a2emodel* a2em = new a2emodel(e, s);
	return a2em;
}

/*! sets the skybox texture
 *  @param tex the texture id
 */
void scene::set_skybox_texture(unsigned int tex) {
	scene::skybox_tex = tex;
	max_value = e->get_texman()->get_texture(tex)->max_value;
}

/*! returns the skybox texture id
 */
unsigned int scene::get_skybox_texture() {
	return scene::skybox_tex;
}

/*! sets the flag if a skybox is rendered
 *  @param state the new state
 */
void scene::set_render_skybox(bool state) {
	scene::render_skybox = state;
}

/*! returns the render skybox flag
 */
bool scene::get_render_skybox() {
	return scene::render_skybox;
}

/*! scene draw preprocessing
 */
void scene::preprocess() {
	// hdr rendering
	if(e->get_hdr()) {
		r->start_draw(render_buffer);
		r->clear();

		glScalef(1.0, -1.0, 1.0);
		glFrontFace(GL_CW);
	}
	// normal rendering using a fbo
	else if(exts->is_fbo_support()) {
		r->start_draw(scene_buffer);
		r->clear();

		glScalef(1.0, -1.0, 1.0);
		glFrontFace(GL_CW);
	}
}

/*! scene draw postprocessing
 */
void scene::postprocess() {
	// hdr rendering
	if(e->get_hdr()) {
		glFrontFace(GL_CCW);
		glScalef(1.0, 1.0, 1.0);

		// convert
		r->start_draw(blur_buffer1);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::CONVERT_HDR);
		s->set_uniform2f(0, 0.5f / (float)render_buffer->width, 0.5f / (float)render_buffer->height);
		s->set_uniform1i(1, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, render_buffer->tex_id);

		glBegin(GL_QUADS);
			glVertex2f(-1, -1);
			glVertex2f( 1, -1);
			glVertex2f( 1,  1);
			glVertex2f(-1,  1);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();


		// 5x5 line blur
		r->start_draw(blur_buffer3);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::BLURLINE5);
		s->set_uniform2fv(0, tcs_line_h, 5);
		s->set_uniform1i(1, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, blur_buffer1->tex_id);

		glBegin(GL_QUADS);
			s->set_attribute2f(0, 0.0f, 0.0f);
			glVertex2f(-1.0f, -1.0f);

			s->set_attribute2f(0, 1.0f, 0.0f);
			glVertex2f(1.0f, -1.0f);

			s->set_attribute2f(0, 1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);

			s->set_attribute2f(0, 0.0f, 1.0f);
			glVertex2f(-1.0f, 1.0f);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();

		// v

		r->start_draw(blur_buffer2);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::BLURLINE5);
		s->set_uniform2fv(0, tcs_line_v, 5);
		s->set_uniform1i(1, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, blur_buffer3->tex_id);

		glBegin(GL_QUADS);
			s->set_attribute2f(0, 0.0f, 0.0f);
			glVertex2f(-1.0f, -1.0f);

			s->set_attribute2f(0, 1.0f, 0.0f);
			glVertex2f(1.0f, -1.0f);

			s->set_attribute2f(0, 1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);

			s->set_attribute2f(0, 0.0f, 1.0f);
			glVertex2f(-1.0f, 1.0f);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();

		for(unsigned int i = 0; i < 1; i++) {
			// h

			r->start_draw(blur_buffer3);
			r->clear();
			r->start_2d_draw();
			s->use_shader(shader::BLURLINE5);
			s->set_uniform2fv(0, tcs_line_h, 5);
			s->set_uniform1i(1, 0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, blur_buffer2->tex_id);

			glBegin(GL_QUADS);
				s->set_attribute2f(0, 0.0f, 0.0f);
				glVertex2f(-1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 0.0f);
				glVertex2f(1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 1.0f);
				glVertex2f(1.0f, 1.0f);

				s->set_attribute2f(0, 0.0f, 1.0f);
				glVertex2f(-1.0f, 1.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			s->use_shader(shader::NONE);
			r->stop_2d_draw();
			r->stop_draw();

			// v

			r->start_draw(blur_buffer2);
			r->clear();
			r->start_2d_draw();
			s->use_shader(shader::BLURLINE5);
			s->set_uniform2fv(0, tcs_line_v, 5);
			s->set_uniform1i(1, 0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, blur_buffer3->tex_id);

			glBegin(GL_QUADS);
				s->set_attribute2f(0, 0.0f, 0.0f);
				glVertex2f(-1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 0.0f);
				glVertex2f(1.0f, -1.0f);

				s->set_attribute2f(0, 1.0f, 1.0f);
				glVertex2f(1.0f, 1.0f);

				s->set_attribute2f(0, 0.0f, 1.0f);
				glVertex2f(-1.0f, 1.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			s->use_shader(shader::NONE);
			r->stop_2d_draw();
			r->stop_draw();
		}


		// average
		r->start_draw(average_buffer);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::AVERAGE_HDR);
		s->set_uniform1i(0, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, blur_buffer2->tex_id);

		glBegin(GL_TRIANGLES);
			glVertex2f( 0,  2);
			glVertex2f(-3, -1);
			glVertex2f( 3, -1);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();


		fframe_time = (float)(SDL_GetTicks() - iframe_time) / 1000.0f;
		iframe_time = SDL_GetTicks();
		cur_exposure = 1 - cur_exposure;
		r->start_draw(exposure_buffer[cur_exposure]);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::EXPOSURE_HDR);
		s->set_uniform1i(0, 0);
		s->set_uniform1i(1, 1);
		s->set_uniform1f(2, fframe_time);
		exts->glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, average_buffer->tex_id);
		glEnable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D, exposure_buffer[1-cur_exposure]->tex_id);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLES);
			glVertex2f( 0,  2);
			glVertex2f(-3, -1);
			glVertex2f( 3, -1);
		glEnd();

		exts->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();


		// hdr
		r->start_draw(scene_buffer);
		r->clear();
		r->start_2d_draw();
		s->use_shader(shader::HDR);
		s->set_uniform1i(0, 0);
		s->set_uniform1i(1, 1);
		s->set_uniform1i(2, 2);
		exts->glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, render_buffer->tex_id);
		glEnable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D, blur_buffer2->tex_id);
		glEnable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE2_ARB);
		glBindTexture(GL_TEXTURE_2D, exposure_buffer[cur_exposure]->tex_id);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLES);
			glVertex2f( 0,  2);
			glVertex2f(-3, -1);
			glVertex2f( 3, -1);
		glEnd();

		exts->glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		exts->glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
		s->use_shader(shader::NONE);
		r->stop_2d_draw();
		r->stop_draw();
	}
	// normal rendering using a fbo
	else if(exts->is_fbo_support()) {
		glFrontFace(GL_CCW);
		r->stop_draw();
	}
}
