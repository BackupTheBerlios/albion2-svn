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

#include "chat_server.h"
#include <iostream>

/*!
 * \mainpage
 *
 * \author flo
 *
 * \date July - December 2004
 *
 * Albion 2 Engine Sample - Chat Server Sample
 */

int main(int argc, char *argv[])
{
	bool is_networking = false;

	if(n.init()) {
		if(n.create_server(net::TCP, 1337, 32)) {
			is_networking = true;
		}
	}

	while(!done)
	{
		if(is_networking) {
			// server stuff
			SDLNet_CheckSockets(n.socketset, ~0);

			if(SDLNet_SocketReady(n.tcp_ssock)) {
				n.handle_server();
			}

			n.check_events();
		}
		else {
			done = true;
		}
	}

	n.exit();

	return 0;
}
