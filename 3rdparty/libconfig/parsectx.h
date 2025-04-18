/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2013-2025 Hercules Dev Team
   Copyright (C) 2005-2014 Mark A Lindner

   This file is part of libconfig.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#ifndef __libconfig_parsectx_h
#define __libconfig_parsectx_h

#include "libconfig.h"
#include "strbuf.h"

struct parse_context
{
  struct config_t *config;
  struct config_setting_t *parent;
  struct config_setting_t *setting;
  char *name;
  strbuf_t string;
};

#define parsectx_init(C)                        \
  memset((C), 0, sizeof(struct parse_context))
#define parsectx_cleanup(C)                             \
  free((void *)(strbuf_release(&((C)->string))))

#define parsectx_append_string(C, S)            \
  strbuf_append(&((C)->string), (S))
#define parsectx_take_string(C)                 \
  strbuf_release(&((C)->string))

#endif /* __libconfig_parsectx_h */
