#pragma once
/*
 *  Copyright (C) 2001-2026 Jared Ivey
 *
 *  This file is part of Project Greenshift
 *
 *  OSI Certified Open Source Software
 *
 *  Project Greenshift is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; version 2 only.
 *
 *  Project Greenshift is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define SAFE_FREE( buf )         {if( (buf) != NULL ) { free( buf ); buf = NULL; }}
#define SAFE_DELETE( buf )       {if( (buf) != NULL ) { delete( buf ); buf = NULL; }}
#define SAFE_DELETE_ARRAY( buf ) {if( (buf) != NULL ) { delete[]( buf ); buf = NULL; }}
#define SAFE_DELETE_ARRAY_OF_POINTERS( buf, len )   \
    {   \
        for(int sdaopIndex=0; sdaopIndex < len; sdaopIndex++)   \
        {   \
            SAFE_DELETE_ARRAY( (buf) [sdaopIndex] );\
        }   \
    }
