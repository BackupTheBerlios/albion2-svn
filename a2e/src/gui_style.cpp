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

#include "gui_style.h"

/*! there is no function currently
 */
gui_style::gui_style(engine* e) {
	gui_style::e = e;
	gui_style::c = e->get_core();
	gui_style::g = e->get_gfx();
	gui_style::m = e->get_msg();
	gui_style::x = e->get_xml();
	gui_style::screen = e->get_screen();
	gui_style::f = e->get_file_io();

	buffer = new stringstream(stringstream::in | stringstream::out);

	state_types.push_back(*new string("NORMAL"));
	state_types.push_back(*new string("CLICKED"));
	state_types.push_back(*new string("BACKGROUND"));

	r1 = new gfx::rect();
	p1 = new core::pnt();
	p2 = new core::pnt();

	gui_style::load_gui_elements(e->data_path("gui/standard"));
	gui_style::load_color_schemes(e->data_path("gui/color_schemes.xml"));
	set_color_scheme(e->get_color_scheme()->c_str());

	m->print(msg::MDEBUG, "gui_style.cpp", "using color scheme \"%s\" and gui style \"%s\"!",
		e->get_color_scheme()->c_str(), "standard");
}

/*! there is no function currently
 */
gui_style::~gui_style() {
	gui_elements.clear();
	state_types.clear();

	delete buffer;

	delete r1;
	delete p1;
	delete p2;
}

void gui_style::load_color_schemes(const char* cs_file) {
	if(!x->open((char*)cs_file)) {
		m->print(msg::MERROR, "gui_style.cpp", "load_color_schemes(): couldn't open color scheme file %s!", cs_file);
		return;
	}
	else {
		while(x->process()) {
			if(strcmp(x->get_node_name(), "color_schemes") == 0) {
					// process parser ...
					x->process();

					while(strcmp(x->get_node_name(), "color_schemes") != 0) {
						if(strcmp(x->get_node_name(), "scheme") == 0) {
							const char* cs_name = x->get_attribute("name");
							if(cs_name == NULL) {
								m->print(msg::MERROR, "gui_style.cpp", "load_color_schemes(): no scheme name specified!");
								x->close();
								return;
							}

							// process parser ...
							x->process();

							while(strcmp(x->get_node_name(), "scheme") != 0) {
								if(strcmp(x->get_node_name(), "color") == 0) {
									if(x->get_attribute("name") == NULL) {
										m->print(msg::MERROR, "gui_style.cpp", "load_color_schemes(): no color name specified!");
										x->close();
										return;
									}
									if(x->get_attribute("value") == NULL) {
										m->print(msg::MERROR, "gui_style.cpp", "load_color_schemes(): no color value specified!");
										x->close();
										return;
									}

									buffer->setf(stringstream::hex, stringstream::basefield);
									*buffer << "0x" << x->get_attribute("value");
									*buffer >> color_schemes[cs_name][x->get_attribute("name")];
									buffer->setf(stringstream::dec, stringstream::basefield);
									c->reset(buffer);
								}
								// process parser ...
								x->process();
							}
						}

						// process parser ...
						x->process();
					}
			}
		}
		x->close();
	}
}

/*! sets the style color scheme
 *  @param scheme the scheme we want to set
 */
bool gui_style::set_color_scheme(const char* scheme) {
	if(!color_schemes.count(scheme)) {
		m->print(msg::MERROR, "gui_style.cpp", "set_color_scheme(): color scheme \"%s\" doesn't exist!", scheme);
		return false;
	}
	cur_scheme = scheme;
	return true;
}

unsigned int gui_style::get_color(const char* name) {
	unsigned int ret = 0;
	if(color_schemes[cur_scheme].count(name)) {
		ret = color_schemes[cur_scheme][name];
	}
	else {
		m->print(msg::MERROR, "gui_style.cpp", "get_color(): color \"%s\" doesn't exist in current color scheme \"%s\"!", name, cur_scheme.c_str());
	}
	return ret;
}

void gui_style::load_gui_elements(const char* list) {
	gui_elements.clear();
	char* tmpc = new char[512];
	string s1, s2;
	string fname = string(list) + "/list.a2gui";
	f->open_file(fname.c_str(), file_io::OT_READ);
	while(!f->eof()) {
		f->get_line(tmpc);
		*buffer << tmpc;
		*buffer >> s1;
		*buffer >> s2;
		gui_elements[s1].filename = s2;
		c->reset(buffer);
	}
	f->close_file();
	delete [] tmpc;

	bool state_found;
	for(map<string, gui_element>::iterator iter = gui_elements.begin(); iter != gui_elements.end(); iter++) {
		state_found = false;
		fname = string(list) + "/" + iter->second.filename;
		if(!x->open((char*)fname.c_str())) {
			m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): couldn't open gui element file %s!", iter->second.filename.c_str());
			return;
		}
		else {
			while(x->process()) {
				if(strcmp(x->get_node_name(), "gui_element") == 0) {
					if(x->get_attribute("type") == NULL) {
						m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gui_elements type specified!");
						x->close();
						return;
					}
					if(iter->first != x->get_attribute("type")) {
						m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): gui_elements is of the wrong type (%s)!", x->get_attribute("type"));
						x->close();
						return;
					}

					// process parser ...
					x->process();

					while(strcmp(x->get_node_name(), "gui_element") != 0) {
						if(strcmp(x->get_node_name(), "state") == 0) {
							if(x->get_attribute("type") != NULL) {
								state_found = false;
								for(vector<string>::iterator siter = state_types.begin(); siter != state_types.end(); siter++) {
									if(*siter == x->get_attribute("type")) {
										iter->second.gfx_types.push_back(*new vector<gfx_type>);
										iter->second.states.push_back(*new state());
										iter->second.states.back().state_name = x->get_attribute("type");

										state_found = true;
										break;
									}
								}

								if(!state_found) {
									m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): unknown state type (%s)!", x->get_attribute("type"));
									x->close();
									return;
								}
							}

							// process parser ...
							x->process();

							while(strcmp(x->get_node_name(), "state") != 0) {
								if(strcmp(x->get_node_name(), "gfx") == 0) {
									iter->second.gfx_types.back().push_back(*new gfx_type());
									const char* t_att = x->get_attribute("type");
									const char* pc_att = x->get_attribute("primary_color");
									const char* sc_att = x->get_attribute("secondary_color");
									if(t_att == NULL) {
										m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gfx type specified!");
										x->close();
										return;
									}
									if(x->get_attribute("bbox") == NULL) {
										m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gfx bbox specified!");
										x->close();
										return;
									}
									if(x->get_attribute("correct") == NULL) {
										m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gfx correction values specified!");
										x->close();
										return;
									}

									if(strcmp(t_att, "image") != 0) {
										if(pc_att == NULL) {
											m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gfx primary color specified!");
											x->close();
											return;
										}

										iter->second.gfx_types.back().back().primary_color = pc_att;
									}

									if(strcmp(t_att, "rect_2col") == 0 || strcmp(t_att, "hfade") == 0 ||
										strcmp(t_att, "vfade") == 0 || strcmp(t_att, "dfade") == 0) {
										if(sc_att == NULL) {
											m->print(msg::MERROR, "gui_style.cpp", "load_gui_elements(): no gfx secondary color specified!");
											x->close();
											return;
										}

										iter->second.gfx_types.back().back().secondary_color = sc_att;
									}

									*buffer << x->get_attribute("bbox");
									for(unsigned int i = 0; i < 4; i++) {
										*buffer >> tmp;
										if(tmp == "x1") iter->second.gfx_types.back().back().bbox[i] = 0;
										else if(tmp == "y1") iter->second.gfx_types.back().back().bbox[i] = 1;
										else if(tmp == "x2") iter->second.gfx_types.back().back().bbox[i] = 2;
										else if(tmp == "y2") iter->second.gfx_types.back().back().bbox[i] = 3;
										else if(tmp == "center") iter->second.gfx_types.back().back().bbox[i] = 4;
									}
									c->reset(buffer);

									*buffer << x->get_attribute("correct");
									for(unsigned int i = 0; i < 4; i++) {
										*buffer >> iter->second.gfx_types.back().back().correct[i];
									}
									c->reset(buffer);

									if(strcmp(t_att, "point") == 0) {
										iter->second.gfx_types.back().back().type = GT_POINT;
									}
									else if(strcmp(t_att, "line") == 0) {
										iter->second.gfx_types.back().back().type = GT_LINE;
									}
									else if(strcmp(t_att, "rect") == 0) {
										iter->second.gfx_types.back().back().type = GT_RECT;
									}
									else if(strcmp(t_att, "rect_filled") == 0) {
										iter->second.gfx_types.back().back().type = GT_RECT_FILLED;
									}
									else if(strcmp(t_att, "rect_2col") == 0) {
										iter->second.gfx_types.back().back().type = GT_RECT_2COL;
									}
									else if(strcmp(t_att, "image") == 0) {
										iter->second.gfx_types.back().back().type = GT_IMG;
									}
									else if(strcmp(t_att, "text") == 0) {
										iter->second.gfx_types.back().back().type = GT_TEXT;
									}
									else if(strcmp(t_att, "hfade") == 0) {
										iter->second.gfx_types.back().back().type = GT_HFADE;
									}
									else if(strcmp(t_att, "vfade") == 0) {
										iter->second.gfx_types.back().back().type = GT_VFADE;
									}
									else if(strcmp(t_att, "dfade") == 0) {
										iter->second.gfx_types.back().back().type = GT_DFADE;
									}
								}

								// process parser ...
								x->process();
							}
						}

						// process parser ...
						x->process();
					}
				}
			}
			x->close();
		}
	}
}

void gui_style::render_gui_element(const char* name, const char* state_name, gfx::rect* rectangle, unsigned int x, unsigned int y) {
	vector< vector<gfx_type> >::iterator elem;
	vector<state>::iterator st;

	unsigned int a = 0;
	for(vector<state>::iterator siter = gui_elements[name].states.begin(); siter != gui_elements[name].states.end(); siter++) {
		if(siter->state_name == state_name) {
			elem = gui_elements[name].gfx_types.begin()+a;
			st = siter;
			break;
		}
		a++;
	}

	unsigned int centerx = rectangle->x1 + ((rectangle->x2 - rectangle->x1) / 2);
	unsigned int centery = rectangle->y1 + ((rectangle->y2 - rectangle->y1) / 2);

	for(unsigned int i = 0; i < (unsigned int)elem->size(); i++) {
		switch(elem->at(i).type) {
			case GT_POINT:
				p1->x = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				p1->y = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				g->draw_point(p1, get_color(elem->at(i).primary_color.c_str()));
				break;
			case GT_LINE:
				p1->x = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				p1->y = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				p2->x = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				p2->y = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_line(p1, p2, get_color(elem->at(i).primary_color.c_str()));
				break;
			case GT_RECT:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_rectangle(r1, get_color(elem->at(i).primary_color.c_str()));
				break;
			case GT_RECT_FILLED:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_filled_rectangle(r1, get_color(elem->at(i).primary_color.c_str()));
				break;
			case GT_RECT_2COL:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_2colored_rectangle(r1, get_color(elem->at(i).primary_color.c_str()),
						 get_color(elem->at(i).secondary_color.c_str()));
				break;
			case GT_TEXT:
				if(text != NULL) {
					if(elem->at(i).bbox[0] != 4) p1->x = x + elem->at(i).correct[0] + (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]);
					else p1->x = x + elem->at(i).correct[0] + centerx - (text->get_text_width() / 2);

					if(elem->at(i).bbox[1] != 4) p1->y = y + elem->at(i).correct[1] + (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]);
					else p1->y = y + elem->at(i).correct[1] + centery - (text->get_text_height() / 2);

					text->set_color(get_color(elem->at(i).primary_color.c_str()));
					text->draw(p1->x, p1->y);
				}
				break;
			case GT_IMG:
				if(img != NULL) {
					if(elem->at(i).bbox[0] != 4) p1->x = x + elem->at(i).correct[0] + (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]);
					else p1->x = x + elem->at(i).correct[0] + centerx - (img->get_width() / 2);

					if(elem->at(i).bbox[1] != 4) p1->y = y + elem->at(i).correct[1] + (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]);
					else p1->y = y + elem->at(i).correct[1] + centery - (img->get_height() / 2);

					p2->x = (rectangle->x2 - rectangle->x1) - abs(elem->at(i).correct[0]) - abs(elem->at(i).correct[2]);
					p2->y = (rectangle->y2 - rectangle->y1) - abs(elem->at(i).correct[1]) - abs(elem->at(i).correct[3]);

					img->set_position(p1->x, p1->y);
					img->draw(p2->x, p2->y);
				}
				break;
			case GT_HFADE:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_fade_rectangle(r1, get_color(elem->at(i).primary_color.c_str()),
					get_color(elem->at(i).secondary_color.c_str()), gfx::FT_HORIZONTAL);
				break;
			case GT_VFADE:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_fade_rectangle(r1, get_color(elem->at(i).primary_color.c_str()),
					get_color(elem->at(i).secondary_color.c_str()), gfx::FT_VERTICAL);
				break;
			case GT_DFADE:
				r1->x1 = x + elem->at(i).correct[0] + (elem->at(i).bbox[0] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[0]));
				r1->y1 = y + elem->at(i).correct[1] + (elem->at(i).bbox[1] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[1]));
				r1->x2 = x + elem->at(i).correct[2] + (elem->at(i).bbox[2] == 4 ? centerx : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[2]));
				r1->y2 = y + elem->at(i).correct[3] + (elem->at(i).bbox[3] == 4 ? centery : (unsigned int)*(&rectangle->x1 + elem->at(i).bbox[3]));
				g->draw_fade_rectangle(r1, get_color(elem->at(i).primary_color.c_str()),
					get_color(elem->at(i).secondary_color.c_str()), gfx::FT_DIAGONAL);
				break;
			default:
				break;
		}
	}
}

void gui_style::set_gui_text(gui_text* text) {
	gui_style::text = text;
}

void gui_style::set_image(image* img) {
	gui_style::img = img;
}
