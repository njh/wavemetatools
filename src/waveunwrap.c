/*
    waveunwrap.c
    
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


// Globals
#define READ_BUFFER_SIZE 2048
FILE * output = NULL;


void
handle_error(char* errStr)
{
    fprintf(stderr, errStr);
    //if (errNum) fprintf(stderr, "\n\t%x\n", errNum);
    
    exit(2);
}


// 'data' 
void
proccessDataChunk( FILE *input, u_int32_t chunkSize )
{
	char * read_buffer = malloc( READ_BUFFER_SIZE );
	int read_size = READ_BUFFER_SIZE;
	
	if (!read_buffer)
		handle_error( "Unable to allocate memory for read buffer.\n" );

	// Copy data from input file to output file
	// in 2k blocks, until we get to the end of the chunk
	while( chunkSize ) {
		if (chunkSize < read_size) read_size = chunkSize;
		
		// Read bytes from input file
		if (fread( read_buffer, read_size, 1, input )!=1)
			handle_error( "Unable to read from input file.\n" );

		
		// Write bytes to output file
		if (fwrite( read_buffer, read_size, 1, output )!=1)
			handle_error( "Unable to write bytes to output file.\n" );
		
		chunkSize -= read_size;
	}

	// Free up the read buffer memory
	free( read_buffer );
}


// Reads in a sub chunk starting at offset
// and returns the postion of the next sub chunk
int
proccessSubChunk(FILE* file, int seek)
{
    char type[4];
    u_int32_t chunkSize;
    u_int32_t nextSubChunk;

    // # For some unknown reason the data in the
    // # WAVE data chunk is sometimes a byte or two too long
    // # fortunately they are always NULL bytes, so we can
    // # just ignore them
    do {
        // Seek to the start of the sub chunk
        if (fseek(file, seek, SEEK_SET)!=0)
            handle_error("Error: unable to seek to start of sub chunk\n");

        // Read in the sub chunk type
        if (fread(&type, sizeof(type), 1, file)!=1)
            handle_error("Error: unable to read sub chunk type\n");
        
        // skip a byte if the sub-chunk type starts with a null byte
        if (type[0] == 0) ++seek;
    } while (type[0] == 0);
    
    // Read in the length of the subchunk
    chunkSize = read_uint32(file, "sub chunk size");
    nextSubChunk = seek+8+chunkSize;

    // Check the sub chunk type
    if (memcmp("data", &type, sizeof(type))==0) {
    	proccessDataChunk( file, chunkSize );
    } else {
    	// Ignore all other chunks
    }
    
    return nextSubChunk;
}



// Reads in a chunk starting at offset
// and returns the postion of the next chunk
int
proccessChunk(FILE* file, int seek)
{
    char type[4];
    char format[4];
    u_int32_t chunkSize;
    u_int32_t nextChunk;
    u_int32_t subSeek;

    // Seek to the start of the chunk
    if (seek != fseek(file, seek, SEEK_SET))
        handle_error("Error: unable to seek to start of chunk\n");

    // Read in the chunk type
    if (fread(&type, sizeof(type), 1, file)!=1)
        handle_error("Error: unable to read chunk type\n");

     // Make sure it is RIFF
    if (memcmp("RIFF", &type, sizeof(type))!=0) {
        fprintf(stderr, "Error: unsupport chunk type: %4.4s\n", type);
        exit(2);
    }
    
    // Read in the length of the chunk
    chunkSize = read_uint32(file, "chunk size");
    nextChunk = seek+8+chunkSize;


    // Read in the format of the chunk
    if (fread(&format, sizeof(format), 1, file)!=1)
        handle_error("Error: unable to read chunk format\n");

   // Make sure it is WAVE
    if (memcmp("WAVE", &format, sizeof(format))!=0) {
        fprintf(stderr, "Error: unsupported chunk format: %4.4s\n", format);
        exit(2);
    }

    // Read in the sub chunks
    subSeek = ftell(file);
    while(subSeek < nextChunk) {
    
        subSeek = proccessSubChunk( file, subSeek );
    
    }
    
    return nextChunk;
}


/* Display how to use this program */
static int usage( const char * progname )
{
    fprintf(stderr, "Wave Meta Tools version %s\n", VERSION);
    fprintf(stderr, "Usage: %s <input.wav> <output>\n\n", progname);
    exit(1);
}


int
main(int argc, char **argv)
{
    u_int32_t seek = 0;
    struct stat fileInfo;
    FILE * input = NULL;
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


    // Get chunks until the next chunk is
    // beyond the end of the file
    while (seek < fileInfo.st_size) {
        seek = proccessChunk( input, seek );
    }
    
    // Close the file
    fclose(input);
    fclose(output);
    
    // Success !
    return 0;
}
