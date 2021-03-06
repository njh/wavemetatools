/*
    util.h
    
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


#ifndef _UTIL_H
#define _UTIL_H

void handle_error(const char* errStr);

uint32_t my_swap32(uint32_t x);
uint16_t my_swap16(uint16_t x);

uint32_t read_uint32( FILE* file, const char * err_str );
uint16_t read_uint16( FILE* file, const char * err_str );
uint8_t  read_uint8( FILE* file, const char * err_str );

void write_uint32( FILE* file, uint32_t x, const char * err_str );
void write_uint16( FILE* file, uint16_t x, const char * err_str );
void write_uint8( FILE* file, uint8_t x, const char * err_str );

#endif //_UTIL_H
