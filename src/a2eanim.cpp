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

#include "a2eanim.h"

#define MAX_OBJS 32

/*! there is no function currently
 */
a2eanim::a2eanim() {
	joint_count = 0;
	base_joint_count = 0;
	mesh_count = 0;
	canimations = 0;
	current_animation = 0;
	current_frame = 0;
	timer = SDL_GetTicks();

	joints = NULL;
	meshes = NULL;
	animations = NULL;
	base_joints = NULL;

	is_material = false;
	material = NULL;

	is_draw_joints = false;
}

/*! there is no function currently
 */
a2eanim::~a2eanim() {
	m.print(msg::MDEBUG, "a2eanim.cpp", "freeing a2eanim stuff");

	if(a2eanim::joints != NULL) {
		delete a2eanim::joints;
	}

	if(a2eanim::meshes != NULL) {
		delete a2eanim::meshes;
	}

	if(a2eanim::base_joints != NULL) {
		delete [] a2eanim::base_joints;
	}

	if(a2eanim::animations != NULL) {
		delete [] a2eanim::animations;
	}

	if(a2eanim::material != NULL) {
		delete a2eanim::material;
	}

	m.print(msg::MDEBUG, "a2eanim.cpp", "a2eanim stuff freed");
}

/*! draws the model
 */
void a2eanim::draw() {
	// frame stuff
	if(a2eanim::current_frame < a2eanim::animations[current_animation]->frame_count &&
		SDL_GetTicks() - a2eanim::timer > a2eanim::current_frame * a2eanim::animations[current_animation]->frame_time) {
		a2eanim::current_frame = (unsigned int)((float)(SDL_GetTicks() - a2eanim::timer) /
			a2eanim::animations[current_animation]->frame_time);
	}

	if(a2eanim::current_frame >= a2eanim::animations[current_animation]->frame_count) {
		a2eanim::current_frame = 0;
		a2eanim::timer = SDL_GetTicks();
	}

	// build the bones
	for(unsigned int i = 0; i < a2eanim::base_joint_count; i++) {
		a2eanim::build_bone(a2eanim::current_frame, base_joints[i], &base_joints[i]->position, &base_joints[i]->orientation);
	}

	// skin the mesh
	a2eanim::skin_mesh();

	// ... and here we go finally - render the model :)
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	if(a2eanim::is_material) {
		glBindTexture(GL_TEXTURE_2D, *(a2eanim::material->get_texture(0)));
	}

	glFrontFace(GL_CW);
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		glBegin(GL_TRIANGLES);
		for(unsigned int j = 0; j < a2eanim::meshes[i].triangle_count; j++) {
			glTexCoord2f(a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i1].u,
				a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i1].v);
			glVertex3f(a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i1].x,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i1].y,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i1].z);
			glTexCoord2f(a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i2].u,
				a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i2].v);
			glVertex3f(a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i2].x,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i2].y,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i2].z);
			glTexCoord2f(a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i3].u,
				a2eanim::meshes[i].tex_coords[a2eanim::meshes[i].indices[j].i3].v);
			glVertex3f(a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i3].x,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i3].y,
				a2eanim::meshes[i].vertices[a2eanim::meshes[i].indices[j].i3].z);
		}
		glEnd();
	}
	glFrontFace(GL_CCW);

	glPopMatrix();

	// draw joints if flag is set
	if(a2eanim::is_draw_joints) {
		a2eanim::draw_joints();
	}
}

/*! draws the models joints
 */
void a2eanim::draw_joints() {
	for(unsigned int i = 0; i < (unsigned int)a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent != NULL) {
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, 0.0f);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);

			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
				glVertex3f(a2eanim::joints[i].parent->position.x,
					a2eanim::joints[i].parent->position.y,
					a2eanim::joints[i].parent->position.z);
				glVertex3f(a2eanim::joints[i].position.x,
					a2eanim::joints[i].position.y,
					a2eanim::joints[i].position.z);
			glEnd();

			glEnable(GL_LIGHTING);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);
			glPopMatrix();
		}
	}
}

/*! loads a .a2m model file
 *  @param filename the name of the .a2m model file
 */
void a2eanim::load_model(char* filename) {
	file.open_file(filename, true);

	// get file type
	file.get_block(a2eanim::file_type, 8);
	a2eanim::file_type[8] = 0;

	// get model type and abort if it's not 0x01
	char model_type = file.get_char();
	if(model_type != 0x01) {
		m.print(msg::MERROR, "a2eanim.cpp", "non supported model type: %u!", (unsigned int)(model_type & 0xFF));
		return;
	}

	// joint count
	a2eanim::joint_count = file.get_uint();
	// base joint count
	a2eanim::base_joint_count = file.get_uint();

	// get joint data
	a2eanim::joints = new joint[a2eanim::joint_count];
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		// joint number
		a2eanim::joints[i].num = i;
		// joint parent
		a2eanim::joints[i].parent_num = file.get_uint();
		// joint children count
		a2eanim::joints[i].children_count = 0;
		// joint translation
		a2eanim::joints[i].position.x = file.get_float();
		a2eanim::joints[i].position.y = file.get_float();
		a2eanim::joints[i].position.z = file.get_float();
		// joint orientation
		a2eanim::joints[i].orientation.r = file.get_float();
		a2eanim::joints[i].orientation.x = file.get_float();
		a2eanim::joints[i].orientation.y = file.get_float();
		a2eanim::joints[i].orientation.z = file.get_float();
	}

	// object/mesh count
	a2eanim::mesh_count = file.get_uint();

	// get mesh data
	a2eanim::meshes = new mesh[a2eanim::mesh_count];
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		// vertex count
		a2eanim::meshes[i].vertex_count = file.get_uint();

		// vertex data
		a2eanim::meshes[i].tex_coords = new core::coord[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].weight_indices = new unsigned int[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].weight_counts = new unsigned int[a2eanim::meshes[i].vertex_count];
		a2eanim::meshes[i].vertices = new vertex3[a2eanim::meshes[i].vertex_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			// uv coordinates
			a2eanim::meshes[i].tex_coords[j].u = file.get_float();
			a2eanim::meshes[i].tex_coords[j].v = file.get_float();
			// weight index
			a2eanim::meshes[i].weight_indices[j] = file.get_uint();
			// weight count
			a2eanim::meshes[i].weight_counts[j] = file.get_uint();
			// vertices
			a2eanim::meshes[i].vertices[j].x = 0.0f;
			a2eanim::meshes[i].vertices[j].y = 0.0f;
			a2eanim::meshes[i].vertices[j].z = 0.0f;
		}

		// triangle count
		a2eanim::meshes[i].triangle_count = file.get_uint();

		// triangle data
		a2eanim::meshes[i].indices = new core::index[a2eanim::meshes[i].triangle_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].triangle_count; j++) {
			// indices
			a2eanim::meshes[i].indices[j].i1 = file.get_uint();
			a2eanim::meshes[i].indices[j].i2 = file.get_uint();
			a2eanim::meshes[i].indices[j].i3 = file.get_uint();
		}

		// weight count
		a2eanim::meshes[i].weight_count = file.get_uint();

		// weight data
		a2eanim::meshes[i].bone_indices = new int[a2eanim::meshes[i].weight_count];
		a2eanim::meshes[i].weight_strenghts = new float[a2eanim::meshes[i].weight_count];
		a2eanim::meshes[i].weights = new vertex3[a2eanim::meshes[i].weight_count];
		for(unsigned int j = 0; j < a2eanim::meshes[i].weight_count; j++) {
			// bone index
			a2eanim::meshes[i].bone_indices[j] = file.get_int();
			// weight strength
			a2eanim::meshes[i].weight_strenghts[j] = file.get_float();
			// weight itself
			a2eanim::meshes[i].weights[j].x = file.get_float();
			a2eanim::meshes[i].weights[j].y = file.get_float();
			a2eanim::meshes[i].weights[j].z = file.get_float();
		}
	}

	file.close_file();

	// compute parent<->children connections
	a2eanim::base_joints = new joint*[a2eanim::base_joint_count];

	// parent stuff ...
	unsigned int cbj = 0;
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent_num < 0) {
			a2eanim::base_joints[cbj] = &a2eanim::joints[i];
			a2eanim::base_joints[cbj]->parent = NULL;
			cbj++;
		}
		else {
			a2eanim::joints[i].parent = &a2eanim::joints[a2eanim::joints[i].parent_num];
			a2eanim::joints[i].parent->children_count++;
		}
	}

	// children stuff ...
	unsigned int* count = new unsigned int[a2eanim::joint_count];
	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		a2eanim::joints[i].childrens = new joint*[a2eanim::joints[i].children_count];
		count[i] = 0;
	}

	for(unsigned int i = 0; i < a2eanim::joint_count; i++) {
		if(a2eanim::joints[i].parent) {
			a2eanim::joints[i].parent->childrens[count[a2eanim::joints[i].parent_num]] = &a2eanim::joints[i];
			count[a2eanim::joints[i].parent_num]++;
		}
	}
}

/*! loads an .a2a animation file
 *  @param filename the name of the .a2a animation file
 */
void a2eanim::add_animation(char* filename) {
	// create new animation
	animation** anis = new animation*[a2eanim::canimations + 1];
	for(unsigned int i = 0; i < a2eanim::canimations; i++) {
		anis[i] = animations[i];
	}
	// delete old animation pointers
	if(animations) { delete animations; }


	// load animation file
	file.open_file(filename, true);

	// create new animation
	animation* ani = new animation();

	// get file type
	char* file_type = new char[7];
	file.get_block(file_type, 7);
	delete file_type;

	// joint count
	ani->joint_count = file.get_uint();

	// get joint data
	ani->joint_parents = new int[ani->joint_count];
	ani->joint_flags = new unsigned int[ani->joint_count];
	ani->joint_start_indices = new unsigned int[ani->joint_count];
	ani->joint_base_translation = new vertex3[ani->joint_count];
	ani->joint_base_orientation = new quaternion[ani->joint_count];
	for(unsigned int i = 0; i < ani->joint_count; i++) {
		// joint parent
		ani->joint_parents[i] = file.get_int();
		// joint flags
		ani->joint_flags[i] = file.get_uint();
		// joint start index
		ani->joint_start_indices[i] = file.get_uint();
		// joint base translation
		ani->joint_base_translation[i].x = file.get_float();
		ani->joint_base_translation[i].y = file.get_float();
		ani->joint_base_translation[i].z = file.get_float();
		// joint base orientation
		ani->joint_base_orientation[i].r = file.get_float();
		ani->joint_base_orientation[i].x = file.get_float();
		ani->joint_base_orientation[i].y = file.get_float();
		ani->joint_base_orientation[i].z = file.get_float();
	}

	// frame count
	ani->frame_count = file.get_uint();
	// frame time
	ani->frame_time = file.get_float();
	// animated components count
	ani->animated_components_count = file.get_uint();

	// write frame data
	ani->frames = new float*[ani->frame_count];
	for(unsigned int i = 0; i < ani->frame_count; i++) {
		// animated components data
		ani->frames[i] = new float[ani->animated_components_count];
		for(unsigned int j = 0; j < ani->animated_components_count; j++) {
			ani->frames[i][j] = file.get_float();
		}
	}

	// close animation file
	file.close_file();


	// assign new animation
	anis[a2eanim::canimations] = ani;

	// increment animation count
	a2eanim::canimations++;

	// reassign animation pointers
	animations = anis;
}

void a2eanim::skin_mesh() {
	vertex3 position;
	for(unsigned int i = 0; i < a2eanim::mesh_count; i++) {
		for(unsigned int j = 0; j < a2eanim::meshes[i].vertex_count; j++) {
			a2eanim::meshes[i].vertices[j].x = 0.0f;
			a2eanim::meshes[i].vertices[j].y = 0.0f;
			a2eanim::meshes[i].vertices[j].z = 0.0f;

			for(unsigned int k = 0; k < a2eanim::meshes[i].weight_counts[j]; k++) {
				int base_index = a2eanim::meshes[i].weight_indices[j];
				int bone_index = a2eanim::meshes[i].bone_indices[base_index + k];

				position = a2eanim::joints[bone_index].orientation.rotate(a2eanim::meshes[i].weights[base_index + k]);
				vertex3* tmp = new vertex3(position + joints[bone_index].position);
				tmp->x *= a2eanim::meshes[i].weight_strenghts[base_index + k];
				tmp->y *= a2eanim::meshes[i].weight_strenghts[base_index + k];
				tmp->z *= a2eanim::meshes[i].weight_strenghts[base_index + k];
				a2eanim::meshes[i].vertices[j] += *tmp;
				delete tmp;
				/*a2eanim::meshes[i].vertices[j] += (position + joints[bone_index].position) * a2eanim::meshes[i].weight_strenghts[base_index + k];*/
			}
		}
	}
}

void a2eanim::build_bone(unsigned int frame, joint* jnt, vertex3* parent_translation, quaternion* parent_oriantation) {
	vertex3 translation = animations[current_animation]->joint_base_translation[jnt->num];
	quaternion orientation = animations[current_animation]->joint_base_orientation[jnt->num];

	unsigned int flags = animations[current_animation]->joint_flags[jnt->num];
	unsigned int start_index = animations[current_animation]->joint_start_indices[jnt->num];
	unsigned int n = 0;

	// get animated translation and orientation stuff (if specific flag is set)
	if(flags & 1) { translation.x = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 2) { translation.y = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 4) { translation.z = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 8) { orientation.x = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 16) { orientation.y = animations[current_animation]->frames[frame][start_index + n]; n++; };
	if(flags & 32) { orientation.z = animations[current_animation]->frames[frame][start_index + n]; n++; };

	orientation.compute_r();

	// now assign the new computed translation and orientation values
	if(jnt->parent_num < 0) {
		jnt->position = translation;
		jnt->orientation = orientation;
	}
	else {
		jnt->position = parent_oriantation->rotate(translation);
		jnt->position += *parent_translation;
		jnt->orientation = *parent_oriantation * orientation;
	}

	// build the bones of all childs
	for(unsigned int i = 0; i < jnt->children_count; i++) {
		a2eanim::build_bone(frame, jnt->childrens[i], &jnt->position, &jnt->orientation);
	}
}

void a2eanim::set_material(a2ematerial* material) {
	a2eanim::material = material;
	a2eanim::is_material = true;
}

/*! sets the bool if the models joints are drawn
 *  @param state the new state
 */
void a2eanim::set_draw_joints(bool state) {
	a2eanim::is_draw_joints = state;
}

/*! returns a true if the models joints are drawn
 */
bool a2eanim::get_draw_joints() {
	return a2eanim::is_draw_joints;
}
