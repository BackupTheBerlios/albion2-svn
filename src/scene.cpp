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
#include "msg.h"
#include <math.h>

/*! there is no function currently
 */
scene::scene() {
	scene::cmodels = 0;
	scene::clights = 0;
	scene::position = new core::vertex3();
	scene::position->x = 0.0f;
	scene::position->y = 0.0f;
	scene::position->z = 0.0f;

	scene::mambient = new float[4];
	scene::mdiffuse = new float[4];
	scene::mspecular = new float[4];
	scene::mshininess = new float[4];
	scene::mambient[0] = scene::mambient[1] = scene::mambient[2] = scene::mambient[3] = 0.0f;
	scene::mdiffuse[0] = scene::mdiffuse[1] = scene::mdiffuse[2] = scene::mdiffuse[3] = 0.0f;
	scene::mspecular[0] = scene::mspecular[1] = scene::mspecular[2] = scene::mspecular[3] = 0.0f;
	scene::mshininess[0] = scene::mshininess[1] = scene::mshininess[2] = scene::mshininess[3] = 0.0f;

	/*sphere = new a2emodel();
	sphere->load_model("sphere.a2m");*/

	scene::is_light = false;
}

/*! there is no function currently
 */
scene::~scene() {
	m.print(msg::MDEBUG, "scene.cpp", "freeing scene stuff");

	delete scene::position;
	delete scene::mambient;
	delete scene::mdiffuse;
	delete scene::mspecular;
	delete scene::mshininess;

	delete sphere;

	m.print(msg::MDEBUG, "scene.cpp", "scene stuff freed");
}

/*! draws the scene
 */
void scene::draw() {
	for(unsigned int i = 0; i < clights; i++) {
        float pos[] = { lights[i]->get_position()->x, lights[i]->get_position()->y, lights[i]->get_position()->z };
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
		
		/*core::vertex3* scale = (core::vertex3*)malloc(sizeof(core::vertex3));
		scale->x = 1.0f;
		scale->y = 1.0f;
		scale->z = 1.0f;
		scene::draw_sphere(lights[i]->get_position(), scale);
		free(scale);*/
	}

	if(scene::is_light) {
        glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		glMaterialfv(GL_FRONT, GL_AMBIENT, scene::mambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, scene::mdiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, scene::mspecular);
		glMaterialfv(GL_FRONT, GL_SHININESS, scene::mshininess);
	}
	else {
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	/*glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);*/
	//LightModelfv(LIGHT_MODEL_AMBIENT, &globalAmbient);

	for(unsigned int i = 0; i < cmodels; i++) {
		models[i]->draw_model();
	}

	/*glDepthMask(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);*/
	//LightModelfv(LIGHT_MODEL_AMBIENT, &zero);


}

/*! adds a model to the scene
 *  @param model pointer to the model
 */
void scene::add_model(a2emodel* model) {
	models[cmodels] = model;
	cmodels++;
}

/*! removes a model from the scene
 *  @param model pointer to the model
 */
void scene::delete_model(a2emodel* model) {
	unsigned int num = 0;
	//cout << "getting number ..." << endl;
	for(unsigned int i = 0; i < cmodels; i++) {
		if(models[i] == model) {
			//cout << "deleting model #" << i << endl;
			//delete models[i];
			//cout << "model deleted" << endl;
			num = i;
			i = cmodels;
		}
		else if(i == (cmodels-1)) {
			m.print(msg::MDEBUG, "scene.cpp", "can't delete model: model doesn't exist!");
			return;
		}
	}
	//cout << "got number: " << num << endl;

	for(unsigned int i = num; i < (cmodels-1); i++) {
		models[i] = models[i+1];
	}
	models[(cmodels-1)] = NULL;
	//cout << "objects repointed" << num << endl;

	// decrease model count
	cmodels--;
}

/*! adds a light to the scene
 *  @param light pointer to the light
 */
void scene::add_light(light* light) {
	// enable light automatically if we had no light before
	if(scene::clights == 0) {
		scene::is_light = true;
	}

	lights[clights] = light;
	float pos[] = { lights[clights]->get_position()->x, lights[clights]->get_position()->y, lights[clights]->get_position()->z };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lights[clights]->get_lambient());
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[clights]->get_ldiffuse());
	glLightfv(GL_LIGHT0, GL_SPECULAR, lights[clights]->get_lspecular());
	glEnable(GL_LIGHT0);

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
core::vertex3* scene::get_position() {
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

/*! for debugging purposes - draws a sphere
 *  @param pos the spheres position
 *  @param size the spheres size
 */
void scene::draw_sphere(core::vertex3* pos, core::vertex3* size) {
	/*scene::sphere->set_position(pos->x, pos->y, pos->z);
	scene::sphere->set_scale(size->x, size->y, size->z);
	scene::sphere->draw_model();*/
}
