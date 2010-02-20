/** \file freemcan-common.h
 * \brief Common text user interface functions (interface)
 *
 * \author Copyright (C) 2010 Hans Ulrich Niedermann <hun@n-dimensional.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */


#ifndef __COMMON_H__
#define __COMMON_H__

#if 0
#include "include/system.h"
#endif

#include <stdbool.h>


/** signal handler function prototype */
typedef void (*sighandler_t)(int);

/** flag set by SIGINT handler */
extern bool sigint;

/** flag set by SIGTERM handler */
extern bool sigterm;


#endif /* !__COMMON_H__ */
