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
 
#ifndef __CHAT_H__
#define __CHAT_H__

#include <iostream>
#include <SDL.h>
#include <engine.h>
#include <msg.h>
#include <core.h>
#include <net.h>
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

void get_msg();
void send_msg(char* message, unsigned int length);

bool client_active = false;
bool done = false;

Uint8 *keys;

#endif
