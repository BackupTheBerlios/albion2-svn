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
 
#ifndef __CHAT_CLIENT_H__
#define __CHAT_CLIENT_H__

#include <iostream>
#include <SDL.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
#include <gfx.h>
#include <gui.h>
#include <gui_button.h>
#include <gui_text.h>
#include <gui_input.h>
#include <gui_list.h>
#include <gui_list_item.h>
#include <gui_vbar.h>
#include <gui_style.h>
#include <event.h>
using namespace std;

unsigned int max_clients;

struct client {
	bool is_active;
	char name[32+1];
};

client* clients;

char* client_name;
unsigned int client_num;

msg m;
net n;
core c;
engine e;
gfx agfx;
gui agui;
event aevent;

SDL_Surface* sf;

void get_msg();
void send_msg(char* message, unsigned int length);
void add_msg(char* msg, ...);
void add_user(char* name);

bool client_active = false;
bool done = false;

bool is_networking = false;
bool is_send = false;
bool is_rec = false;
bool is_server = false;

Uint8 *keys;

SDL_Event ievent;

unsigned int refresh_time;

// current msg list box id
unsigned int lid = 1;

// current user list box id
unsigned int uid = 1;

gui_text* info_text;
gui_button* send_button;
gui_list* msg_list;
gui_list* client_list;
gui_input* msg_box;

#endif
