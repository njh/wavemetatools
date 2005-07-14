/*
    util.c
    
    Copyright (C) 2005  Nicholas J. Humfrey
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "util.h"

u_int32_t my_swap32(u_int32_t x) {
    return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

u_int16_t my_swap16(u_int16_t x) {
    return((x<<8)|(x>>8));
}

u_int32_t
read_uint32( FILE* file, const char * err_str )
{
    u_int32_t x = 0;
    
    int res = fread( &x, 4, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to read '%s'.\n", err_str);
        exit(3);
    }
    
#ifdef WORDS_BIGENDIAN
    return my_swap32( x );
#else
    return x;
#endif
}


u_int16_t
read_uint16( FILE* file, const char * err_str )
{
    u_int16_t x = 0;
    
    int res = fread( &x, 2, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to read '%s'.\n", err_str);
        exit(3);
    }

#ifdef WORDS_BIGENDIAN
    return my_swap16( x );
#else
    return x;
#endif
}


u_int8_t
read_uint8( FILE* file, const char * err_str )
{
    u_int8_t x = 0;
    
    int res = fread( &x, 1, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to read '%s'.\n", err_str);
        exit(3);
    }

    return x;
}



void
write_uint32( FILE* file, u_int32_t x, const char * err_str )
{
	u_int32_t y = my_swap32( x );

    int res = fwrite( &y, 4, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to write '%s'.\n", err_str);
        exit(3);
    }
}


void
write_uint16( FILE* file, u_int16_t x, const char * err_str )
{
	u_int16_t y = my_swap16( x );

    int res = fwrite( &y, 2, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to write '%s'.\n", err_str);
        exit(3);
    }
}


void
write_uint8( FILE* file, u_int8_t x, const char * err_str )
{
    int res = fwrite( &x, 1, 1, file );
    if (res!=1) {
        fprintf(stderr, "Failed to write '%s'.\n", err_str);
        exit(3);
    }
}

