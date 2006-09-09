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

/*! there is no function currently
 */
scene::scene(engine* e, shader* s) {
	scene::cmodels = 0;
	scene::camodels = 0;
	scene::clights = 0;

	scene::position = new vertex3();
	scene::position->x = 0.0f;
	scene::position->y = 0.0f;
	scene::position->z = 0.0f;

	scene::mambient = new float[4];
	scene::mdiffuse = new float[4];
	scene::mspecular = new float[4];
	scene::mshininess = new float[4];

	scene::mambient[0] = 1.0f;
	scene::mambient[1] = 1.0f;
	scene::mambient[2] = 1.0f;
	scene::mambient[3] = 1.0f;

	scene::mdiffuse[0] = 1.0f;
	scene::mdiffuse[1] = 1.0f;
	scene::mdiffuse[2] = 1.0f;
	scene::mdiffuse[3] = 1.0f;

	scene::mspecular[0] = 0.2f;
	scene::mspecular[1] = 0.2f;
	scene::mspecular[2] = 0.2f;
	scene::mspecular[3] = 1.0f;

	scene::mshininess[0] = 1.0f;
	scene::mshininess[1] = 1.0f;
	scene::mshininess[2] = 1.0f;
	scene::mshininess[3] = 1.0f;

	scene::is_light = false;

	// get classes
	scene::e = e;
	scene::s = s;
	scene::c = e->get_core();
	scene::m = e->get_msg();
}

/*! there is no function currently
 */
scene::~scene() {
	m->print(msg::MDEBUG, "scene.cpp", "freeing scene stuff");

	delete scene::position;
	delete [] scene::mambient;
	delete [] scene::mdiffuse;
	delete [] scene::mspecular;
	delete [] scene::mshininess;

	m->print(msg::MDEBUG, "scene.cpp", "deleting static models");
	for(unsigned int i = 0; i < cmodels; i++) {
	    delete scene::models[i];
	}

	m->print(msg::MDEBUG, "scene.cpp", "deleting animated models");
	for(unsigned int i = 0; i < camodels; i++) {
	    delete scene::amodels[i];
	}

	m->print(msg::MDEBUG, "scene.cpp", "scene stuff freed");
}

/*! draws the scene
 */
void scene::draw() {
	for(unsigned int i = 0; i < clights; i++) {
		unsigned int light_num = GL_LIGHT0 + i;
        float pos[] = { lights[i]->get_position()->x, lights[i]->get_position()->y, lights[i]->get_position()->z, 1.0f };
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
		glMateriali(GL_FRONT, GL_SHININESS, 128);
	}
	else {
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	for(unsigned int i = 0; i < camodels; i++) {
		amodels[i]->set_light_color(lights[0]->get_lambient());
		amodels[i]->set_light_position(lights[0]->get_position());
		amodels[i]->draw();
	}

	for(unsigned int i = 0; i < cmodels; i++) {
		models[i]->set_light_color(lights[0]->get_lambient());
		models[i]->set_light_position(lights[0]->get_position());
		models[i]->draw();
	}
}

/*! adds a static model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2emodel* model) {
	models[cmodels] = model;
	cmodels++;
}

/*! adds a dynamic model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2eanim* model) {
	amodels[camodels] = model;
	camodels++;
}

/*! removes a model from the scene
 *  @param model pointer to the model
 */
void scene::delete_model(a2emodel* model) {
	unsigned int num = 0;
	for(unsigned int i = 0; i < cmodels; i++) {
		if(models[i] == model) {
			// shouldn't be deleted automatically
			//delete models[i];
			num = i;
			i = cmodels;
		}
		else if(i == (cmodels-1)) {
			m->print(msg::MDEBUG, "scene.cpp", "can't delete model: model doesn't exist!");
			return;
		}
	}

	for(unsigned int i = num; i < (cmodels-1); i++) {
		models[i] = models[i+1];
	}
	models[(cmodels-1)] = NULL;

	// decrease model count
	cmodels--;
}

/*! removes an animated model from the scene
 *  @param model pointer to the animated model
 */
void scene::delete_model(a2eanim* model) {
	unsigned int num = 0;
	for(unsigned int i = 0; i < camodels; i++) {
		if(amodels[i] == model) {
			// shouldn't be deleted automatically
			//delete models[i];
			num = i;
			i = camodels;
		}
		else if(i == (camodels-1)) {
			m->print(msg::MDEBUG, "scene.cpp", "can't delete animated model: model doesn't exist!");
			return;
		}
	}

	for(unsigned int i = num; i < (camodels-1); i++) {
		amodels[i] = amodels[i+1];
	}
	amodels[(camodels-1)] = NULL;

	// decrease model count
	camodels--;
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

	for(unsigned int i = 0; i < cmodels; i++) {
		models[i]->set_position(models[i]->get_position()->x + scene::position->x,
			models[i]->get_position()->y + scene::position->y,
			models[i]->get_position()->z + scene::position->z);
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
void scene::set_mshininess(float* mshininess) {
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
float* scene::get_mshininess() {
	return scene::mshininess;
}

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
