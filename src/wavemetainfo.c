/*
    wavemetainfo.c
    Display metadata about a RIFF/WAVE file in a RFC822 style format
    
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
#include <stdint.h>

#include "config.h"
#include "util.h"

// Windows clipboard types
#define CF_TEXT             1
#define CF_BITMAP           2
#define CF_METAFILEPICT     3
#define CF_SYLK             4
#define CF_DIF              5
#define CF_TIFF             6
#define CF_OEMTEXT          7
#define CF_DIB              8
#define CF_PALETTE          9
#define CF_PENDATA          10
#define CF_RIFF             11
#define CF_WAVE             12
#define CF_UNICODETEXT      13
#define CF_ENHMETAFILE      14
#define CF_HDROP            15
#define CF_LOCALE           16
#define CF_DIBV5            17
#define CF_MAX              18
#define CF_OWNERDISPLAY     0x0080
#define CF_DSPTEXT          0x0081
#define CF_DSPBITMAP        0x0082
#define CF_DSPMETAFILEPICT  0x0083
#define CF_DSPENHMETAFILE   0x008E



// Globals
int debug = 0;
uint32_t byteRate = 0;
uint32_t audioDataLen = 0;


typedef struct {
 	char Version[4];			// Version of the data structure
    char Title[64];				// ASCII title of cart audio sequence
    char Artist[64];			// ASCII artist or creator name
    char CutID[64];				// ASCII cut number identification
    char ClientID[64];			// ASCII client identification
    char Category[64];			// ASCII Category ID, SA, NEWS, etc
    char Classification[64];	// ASCII Classifcataion of auxiliary key
    char OutCue[64];			// ASCII out cue text
    char StartDate[10];			// ASCII YYYY-MM-DD
    char StartTime[8];			// ASCII hh:mm:ss
    char EndDate[10];			// ASCII YYYY-MM-DD
    char EndTime[8];			// ASCII hh:mm:ss
    char ProducerAppID[64];		// Name of vendor or application
    char ProducerAppVersion[64];// Version of producer application
    char UserDef[64];			// User defined text
    //DWORD LevelReference;	// Sample value for 0 dB reference
    //CART_TIMER PostTimer[8];	// 8 time markers after head
    //CHAR Reserved[276];		// Reserved for future expansion
    //CHAR URL[1024];			// Uniform resource locator
    //CHAR TagText[];			// Free form text for scripts or tags
} cart_extension_t;


typedef struct {
 	char Description[256];			// ASCII: Description of the sound sequence
    char Originator[32];			// ASCII: Name of the originator
    char OriginatorReference[32];	// ASCII: Reference of the originator
    char OriginationDate[10];		// ASCII: Origination Date yyyy-mm-dd
    char OriginationTime[8];		// ASCII: Origination Date hh-mm-ss 
    //DWORD TimeReferenceLow;
    //DWORD TimeReferenceHigh;
    //WORD Version;
    //CHAR UMID[64];
    //CHAR Reserved[190];
    //CHAR CodingHistory[];
 } bext_extension_t;


void
read_print_text(FILE* file, uint32_t strLen)
{
    char* data = malloc(strLen+1);
    if (fread(data, strLen, 1, file)!=1)
        handle_error("unable to read text");
    data[strLen] = 0;
    printf("%s\n", data);
    free(data);
}



// 'fmt ' 
void
proccessFmtChunk( FILE *file, uint32_t chunkSize )
{
    uint16_t audioFormat;

    audioFormat = read_uint16( file, "fmt-audio-format" );
    printf("fmt-audio-format: ");
    if (audioFormat == 1)         printf("PCM\n");
    else if (audioFormat == 80)   printf("MPEG\n");
    else if (audioFormat == 85)   printf("MPEG Layer 3\n");
    else if (audioFormat == 257)  printf("MULAW\n");
    else if (audioFormat == 258)  printf("ALAW\n");
    else if (audioFormat == 259)  printf("ADPCM\n");
    else                          printf("Unknown (%d) \n", audioFormat);
    
    printf("fmt-num-channels: %d\n", read_uint16( file, "fmt-num-channels" ) );
    printf("fmt-sample-rate: %d\n", read_uint32( file, "fmt-sample-rate" ) );
    printf("fmt-byte-rate: %d\n", byteRate = read_uint32( file, "fmt-byte-rate" ) );
    printf("fmt-block-align: %d\n", read_uint16( file, "fmt-block-align" ) );
    
    if (audioFormat == 1) {
        printf("fmt-sample-size: %d\n", read_uint16( file, "fmt-sample-size" ) );
    }
}


// 'data' 
void
proccessDataChunk( FILE *file, uint32_t chunkSize )
{
    printf("data-seek: 0x%6.6lx\n", ftell(file));
    printf("data-size: 0x%6.6xx\n", chunkSize);
    audioDataLen = chunkSize;
}


// 'bext' 
void
proccessBextChunk( FILE *file, uint32_t chunkSize)
{
    bext_extension_t bext;
    
    // Zero the memory
    memset( &bext, 0, sizeof(bext) );
    
    // Read in the chunk
   	if (fread(&bext, sizeof(bext), 1, file)!=1)
      handle_error("unable to read bext strings");
   		
	printf("bext-description: %s\n", bext.Description);
	printf("bext-originator: %s\n", bext.Originator);
	printf("bext-originator-ref: %s\n", bext.OriginatorReference);
	printf("bext-origination-date: %10.10s\n", bext.OriginationDate);
	printf("bext-origination-time: %8.8s\n", bext.OriginationTime);

    //printf("bext-time-reference: %d\n", read_uint64( file, "bext-time-reference" ));
    //printf("bext-version: %d\n", read_uint16( file, "bext-version" ));
    //printf("bext-umid: %d\n", read_uint64( file, "bext-umid" ));
}


// 'mext' 
void
proccessMextChunk( FILE *file, uint32_t chunkSize)
{
    printf("mext-sound-information: 0x%2.2x\n", read_uint16( file, "mext-sound-information" ));
    printf("mext-frame-size: %d\n", read_uint16( file, "mext-frame-size" ));
    printf("mext-ancillary-data-length: %d\n", read_uint16( file, "mext-ancillary-data-length" ));
    printf("mext-ancillary-data-def: %d\n", read_uint16( file, "mext-ancillary-data-def" ));
    read_uint16( file, "mext-reserved" );
}


// 'fact' 
void
proccessFactChunk( FILE *file, uint32_t chunkSize )
{
    printf("fact-sample-count: %d\n", read_uint32( file, "fact-sample-count" ) );
}


// 'DISP' 
void
proccessDISPChunk( FILE *file, uint32_t chunkSize )
{
    uint32_t   type;
    
    type = read_uint32( file, "DISP Chunk Type" );
    if (debug) fprintf(stderr, "DISPtype %d\n", type);
    
    // Remove the length of the type bits
    chunkSize -= sizeof(type);
    
    switch(type) {
        case CF_TEXT:
            printf("disp-title: ");
            read_print_text(file, chunkSize);
        break;
        
        default:
            fprintf(stderr, "Warning: Unknown DISP chunk type.\n");
        break;
    }
}


// 'cart' - CartChunk/aes46-2002
void
proccessCartChunk( FILE *file, uint32_t chunkSize )
{
    cart_extension_t cart;
    int n;
    
    // Zero the memory
    memset( &cart, 0, sizeof(cart) );
    
    // Read in the chunk
   	if (fread(&cart, sizeof(cart), 1, file)!=1)
      handle_error("unable to read CartChunk strings");
   		
	printf("cart-version: %4.4s\n", cart.Version);
	printf("cart-title: %s\n", cart.Title);
	printf("cart-artist: %s\n", cart.Artist);
	printf("cart-cutid: %s\n", cart.CutID);
	printf("cart-clientid: %s\n", cart.ClientID);
	printf("cart-category: %s\n", cart.Category);
	printf("cart-classification: %s\n", cart.Classification);
	printf("cart-outcue: %s\n", cart.OutCue);
	printf("cart-startdate: %10.10s\n", cart.StartDate);
	printf("cart-starttime: %8.8s\n", cart.StartTime);
	printf("cart-enddate: %10.10s\n", cart.EndDate);
	printf("cart-endtime: %8.8s\n", cart.EndTime);
	printf("cart-producerappid: %s\n", cart.ProducerAppID);
	printf("cart-producerappversion: %s\n", cart.ProducerAppVersion);
	printf("cart-userdef: %s\n", cart.UserDef);
	
	// Read and display the 32bit Level Reference
	printf("cart-levelreference: %d\n", read_uint32( file, "CartChunk level reference" ) );
	
	// Read in the 8 post timer references
	for(n=0;n<8;n++) {
		char usage[5];
		uint32_t value;
	
		if (fread(&usage, 4, 1, file)!=1)
		  handle_error("unable to read timer ID");
		value = read_uint32( file, "cart post timer value" );

		// Cut spaces off end of timer ID
		if (usage[3] == 0x20) usage[3] = 0x00;
		usage[4] = 0x00;	
		
		if (usage[0]!=0 || usage[1]!=0 ||
		    usage[1]!=0 || usage[3]!=0 ) {
			printf("cart-timer-%s: %d\n", usage, value);
		}
	}
	
	// ** URL Text **

	// ** Tag Text **
}


// 'LIST' 
void
proccessLISTChunk( FILE *file, uint32_t chunkSize )
{
    char    list_type[4];

    // Check the type of list
    if (fread(&list_type, sizeof(list_type), 1, file)!=1)
        handle_error("unable to read LIST type");

    if (memcmp("INFO", &list_type, sizeof(list_type))!=0) {
        fprintf(stderr, "Warning: Unsupported LIST type '%4.4s'.\n", (char*)list_type);
        return;
    }


    // Remove the length of the type bits
    chunkSize -= sizeof(list_type);



    while(chunkSize) {
        char    info_type[4];
        uint32_t   subSize;
        int i = 1;
        
        if (fread(&info_type, sizeof(info_type), 1, file)!=1)
            handle_error("unable to read INFO sub-type");
        
        subSize= read_uint32( file, "INFO sub-size" );
        
        // Make lowercase and replace weird characters
        for(i=0; i<4; i++) {
            if (info_type[i] >= 0x41 && info_type[i] <= 0x5A) {
                info_type[i] += 0x20;
            } else if (info_type[i] < 0x40 || info_type[i] > 0x7E) {
                info_type[i] = '?';
            }
        }
            
        if (debug) {
            printf("info_type %4.4s\n", (char*)info_type);
            printf("INFOsubsize %x\n", subSize);
        }
        
        // Display the data (if it is a string)
        printf("info-%4.4s: ", (char*)info_type); 
        read_print_text( file, subSize );

        // Remove the length of the sub-sub type and size 
        chunkSize -= sizeof(info_type)+sizeof(subSize)+subSize;
    }

}









// Reads in a sub chunk starting at offset
// and returns the postion of the next sub chunk
int
proccessSubChunk(FILE* file, int seek)
{
    char type[4];
    uint32_t chunkSize;
    uint32_t nextSubChunk;

    // DEBUGGING
    if (debug) fprintf(stderr, "  Sub-Chunk at: %8.8x\n", seek);
    
    // # For some unknown reason the data in the
    // # WAVE data chunk is sometimes a byte or two too long
    // # fortunately they are always NULL bytes, so we can
    // # just ignore them
    do {
        // Seek to the start of the sub chunk
        if (fseek(file, seek, SEEK_SET)!=0)
            handle_error("unable to seek to start of sub chunk");

        // Read in the sub chunk type
        if (fread(&type, sizeof(type), 1, file)!=1)
            handle_error("unable to read sub chunk type");
        
        // skip a byte if the sub-chunk type starts with a null byte
        if (type[0] == 0) {
            if (debug) fprintf(stderr, "Warning: Sub Chunk type started with a null char\n");
            ++seek;
        }
    } while (type[0] == 0);
    
    // DEBUGGING
    if (debug) fprintf(stderr, "  Sub-Chunk ID: %4.4s\n", (char*)&type);
    
    // Read in the length of the subchunk
    chunkSize = read_uint32(file, "sub chunk size");
    if (debug) fprintf(stderr, "  Sub-Chunk Size: %8.8x\n", chunkSize);
    
    // Work out the location of the next chunk
    nextSubChunk = seek+8+chunkSize;
    //printf("  Next Sub-Chunk: %8.8x\n", nextSubChunk);
    

    // Check the sub chunk type
    if (memcmp("data", &type, sizeof(type))==0) {
        proccessDataChunk( file, chunkSize );
    } else if (memcmp("fmt ", &type, sizeof(type))==0) {
        proccessFmtChunk( file, chunkSize );
    } else if (memcmp("bext", &type, sizeof(type))==0) {
        proccessBextChunk( file, chunkSize );
    } else if (memcmp("mext", &type, sizeof(type))==0) {
        proccessMextChunk( file, chunkSize );
    } else if (memcmp("fact", &type, sizeof(type))==0) {
        proccessFactChunk( file, chunkSize );
    } else if (memcmp("DISP", &type, sizeof(type))==0) {
        proccessDISPChunk( file, chunkSize );
    } else if (memcmp("LIST", &type, sizeof(type))==0) {
        proccessLISTChunk( file, chunkSize );
    } else if (memcmp("cart", &type, sizeof(type))==0) {
       proccessCartChunk( file, chunkSize );
    } else if (memcmp("JUNK", &type, sizeof(type))==0) {
        // Ignore
    } else {
        fprintf(stderr, "Warning: Unhandled sub-chunk type '%4.4s'.\n", (char*)&type);
    }

    // DEBUGGING
    if (debug) fprintf(stderr, "\n");


    return nextSubChunk;
}



// Reads in a chunk starting at offset
// and returns the postion of the next chunk
int
proccessChunk(FILE* file, int seek)
{
    char type[4];
    char format[4];
    uint32_t chunkSize;
    uint32_t nextChunk;
    uint32_t subSeek;

    // DEBUGGING
    if (debug) printf("Chunk at: %8.8x\n", seek);

    // Seek to the start of the chunk
    if (seek != fseek(file, seek, SEEK_SET))
        handle_error("unable to seek to start of chunk");

    // Read in the chunk type
    if (fread(&type, sizeof(type), 1, file)!=1)
        handle_error("unable to read chunk type");

    // DEBUGGING
    if (debug) printf("Chunk ID: %4.4s\n", (char*)&type);

    // Make sure it is RIFF
    if (memcmp("RIFF", &type, sizeof(type))!=0) {
        fprintf(stderr, "Error: unsupport chunk type: %4.4s\n", type);
        exit(2);
    }
    
    // Read in the length of the chunk
    chunkSize = read_uint32(file, "chunk size");
    if (debug) printf("Chunk Size: %8.8x\n", chunkSize);

    
    // Work out the location of the next chunk
    nextChunk = seek+8+chunkSize;


    // DEBUGGING
    if (debug) printf("Next Chunk: %8.8x\n", nextChunk);
    
    // Read in the format of the chunk
    if (fread(&format, sizeof(format), 1, file)!=1)
        handle_error("unable to read chunk format");


    // DEBUGGING
    if (debug) printf("Chunk Format: %4.4s\n", (char*)&format);
    
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
    fprintf(stderr, "Usage: %s [-d] <filename.wav>\n\n", progname);
    exit(1);
}


int
main(int argc, char **argv)
{
    uint32_t seek = 0;
    struct stat fileInfo;
    FILE * file = NULL;
    char * filename = NULL;
    int opt;
    
    while ((opt = getopt(argc, argv, "dh")) != -1) {
        switch (opt) {
            case 'd':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Unknown option '%c'.\n", (char)opt);
            case 'h':
                usage( argv[0] );
                break;
        }
    }

    if (argc-optind!=1) usage( argv[0] );
    

    // Initialise the globals
    filename = argv[optind];
    byteRate = 0;
    audioDataLen = 0;
    
    
    // Display the filename
    if (debug) fprintf(stderr, "Filename %s\n", filename);

    
    // Get the length of the file
    if(stat(filename, &fileInfo))   handle_error("unable to stat file");

    // Open the file
    file = fopen(filename, "r");
    if (file==NULL) handle_error("unable to open file");

    // Get chunks until the next chunk is
    // beyond the end of the file
    while (seek < fileInfo.st_size) {
        seek = proccessChunk( file, seek );
    }
    
    // Display the length (time) of the file
    if (byteRate)   printf("wave-duration: %d\n", (int)((float)audioDataLen/byteRate * 1000.0f));
    else            printf("wave-duration: unknown\n");
    
    // Close the file
    fclose(file);
    
    // Success !
    return 0;
}

