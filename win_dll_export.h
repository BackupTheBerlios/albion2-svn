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

/*! @brief header for win32 dll export
 *  @author flo
 *  @version 0.1
 *  @date 2004/07/30
 *  @todo -
 *
 *  This is the Win32 DLL export Header
 */

#ifdef WIN32
#define snprintf _snprintf
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef A2E_EXPORTS
#define A2E_API __declspec(dllexport)
#else
#define A2E_API __declspec(dllimport)
#endif // A2E_API_EXPORT
#else
#define A2E_API 
#endif // WIN32
