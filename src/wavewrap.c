/*
    wavewrap.c
    
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "util.h"


void
handle_error(char* errStr)
{
    fprintf(stderr, errStr);
    //if (errNum) fprintf(stderr, "\n\t%x\n", errNum);
    
    exit(2);
}


/* Display how to use this program */
static int usage( const char * progname )
{
    fprintf(stderr, "Wave Meta Tools version %s\n", VERSION);
    fprintf(stderr, "Usage: %s <input> <output.wav>\n\n", progname);
    exit(1);
}


int
main(int argc, char **argv)
{
    u_int32_t seek = 0;
    struct stat fileInfo;
    FILE * input = NULL;
	FILE * output = NULL;
    char * inputname = NULL;
    char * outputname = NULL;
    int opt;
    
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            default:
                fprintf(stderr, "Unknown option '%c'.\n", (char)opt);
            case 'h':
                usage( argv[0] );
                break;
        }
    }

    if (argc-optind!=2) usage( argv[0] );
    

    // Initialise the globals
    inputname = argv[optind];
    outputname = argv[optind+1];

    // Get the length of the file
    if(stat(inputname, &fileInfo))   handle_error("Error: unable to stat file\n");

    // Open the input file
    input = fopen(inputname, "r");
    if (input==NULL) handle_error("Error: unable to open input file\n");

    // Open the output file
    output = fopen(outputname, "w");
    if (output==NULL) handle_error("Error: unable to open output file\n");


	


    // Close the file
    fclose(input);
    fclose(output);
    
    // Success !
    return 0;
}

