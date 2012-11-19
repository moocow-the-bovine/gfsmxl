/*=============================================================================*\
 * File: gfsmxlConfig.h
 * Author: Bryan Jurish <moocow.bovine@gmail.com>
 * Description: autoconf configuration hack
 *
 * Copyright (c) 2008 Bryan Jurish.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *=============================================================================*/

/**
 * \file gfsmxlConfig.h
 * \brief safely include autoheader preprocessor defines
 *
 * \file gfsmxlConfigNoAuto.h
 * \brief Undefine any autoheader preprocessor defines
 *
 * \file gfsmxlConfigAuto.h
 * \brief autoheader-generated preprocessor defines
 */

/* 
 * Putting autoheader files within the #ifndef/#endif idiom (below)
 * is potentially a BAD IDEA, since we might need to (re-)define
 * the autoheader-generated preprocessor symbols (e.g. after
 * (#include)ing in some config.h from another autoheader package
 */
#include <gfsmxlConfigNoAuto.h>
#include <gfsmxlConfigAuto.h>

/* 
 * Define a sentinel preprocessor symbol _GFSM_CONFIG_H, just
 * in case someone wants to check whether we've already
 * (#include)d this file ....
 */
#ifndef _GFSMXL_CONFIG_H
#define _GFSMXL_CONFIG_H

#if defined(GFSMXL_DEBUG_ENABLED) && !defined(GFSMXL_CLC_FH_STATS)
# define GFSMXL_CLC_FH_STATS 1
#endif

#if defined(GFSMXL_DEBUG_ENABLED)
# define GFSMXL_DEBUG_EVAL(code) code
#else
# define GFSMXL_DEBUG_EVAL(code)
#endif

#endif /* _GFSMXL_CONFIG_H */
