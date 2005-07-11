/*--------------------------------------------------------------------------------------/
  bwfinfo.c
  Display the RIFF wave headers from a .wav file
  Tab size: 4

  Nicholas Humfrey <njh@aelius.com>
/--------------------------------------------------------------------------------------*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

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
u_int32_t byteRate = 0;
u_int32_t audioDataLen = 0;






// Helper routines
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
handle_error(char* errStr)
{
	fprintf(stderr, errStr);
	//if (errNum) fprintf(stderr, "\n\t%x\n", errNum);
	
	exit(2);
}

void
read_print_text(FILE* file, u_int32_t strLen)
{
	char* data = malloc(strLen+1);
	if (fread(data, strLen, 1, file)!=1)
		handle_error("Error: unable to read text\n");
	data[strLen] = 0;
	printf("%s\n", data);
	free(data);
}



// 'fmt ' 
void
proccessFmtChunk( FILE *file, u_int32_t chunkSize )
{
	u_int16_t audioFormat;

	audioFormat = read_uint16( file, "fmt-audio-format" );
	printf("fmt-audio-format: ");
	if (audioFormat == 1)		  printf("PCM\n");
	else if (audioFormat == 80)	  printf("MPEG\n");
	else if (audioFormat == 85)	  printf("MPEG Layer 3\n");
	else if (audioFormat == 257)  printf("MULAW\n");
	else if (audioFormat == 258)  printf("ALAW\n");
	else if (audioFormat == 259)  printf("ADPCM\n");
	else						printf("Unknown (%d) \n", audioFormat);
	
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
proccessDataChunk( FILE *file, u_int32_t chunkSize )
{
	printf("data-size: %d\n", chunkSize);
	audioDataLen = chunkSize;
}


// 'bext' 
void
proccessBextChunk( FILE *file, u_int32_t chunkSize)
{
	char description[256];			// ASCII : «Description of the sound sequence»
	char originator[32];			// ASCII : «Name of the originator»
	char originatorReference[32];	// ASCII : «Reference of the originator»
	char originationDate[10];		// ASCII : «yyyy:mm:dd» 
	char originationTime[8];		// ASCII : «hh:mm:ss» 
	
//	u_int32_t timeReferenceLow;			//First sample count since midnight, low word 
//	u_int32_t timeReferenceHigh;		//First sample count since midnight, high word 
//	char reserved[254] ; 			// Reserved for future use, set to "NULL" 
	//char codingHistory[]; 		// ASCII : « History coding » 
	
	if (fread(&description, sizeof(description), 1, file)!=1)
		handle_error("Error: unable to read description\n");
	printf("bext-description: %s\n", description);
	
	if (fread(&originator, sizeof(originator), 1, file)!=1)
		handle_error("Error: unable to read originator\n");
	printf("bext-originator: %s\n", originator);
	
	if (fread(&originatorReference, sizeof(originatorReference), 1, file)!=1)
		handle_error("Error: unable to read originatorReference\n");
	printf("bext-originator-reference: %s\n", originatorReference);
	
	if (fread(&originationDate, sizeof(originationDate), 1, file)!=1)
		handle_error("Error: unable to read originationDate\n");
	printf("bext-origination-date: %s\n", originationDate);
	
	if (fread(&originationTime, sizeof(originationTime), 1, file)!=1)
		handle_error("Error: unable to read originationTime\n");
	printf("bext-origination-time: %s\n", originationTime);
	

	printf("bext-time-reference-low: %d\n", read_uint32( file, "bext-time-reference-low" ));
	printf("bext-time-reference-high: %d\n", read_uint32( file, "bext-time-reference-high" ));
	printf("bext-version: %d\n", read_uint16( file, "bext-version" ));
}

// 'mext' 
void
proccessMextChunk( FILE *file, u_int32_t chunkSize)
{
	printf("mext-sound-information: 0x%2.2x\n", read_uint16( file, "mext-sound-information" ));
	printf("mext-frame-size: %d\n", read_uint16( file, "mext-frame-size" ));
	printf("mext-ancillary-data-length: %d\n", read_uint16( file, "mext-ancillary-data-length" ));
	printf("mext-ancillary-data-def: %d\n", read_uint16( file, "mext-ancillary-data-def" ));
	read_uint16( file, "mext-reserved" );
}


// 'fact' 
void
proccessFactChunk( FILE *file, u_int32_t chunkSize )
{
	printf("fact-sample-count: %d\n", read_uint32( file, "fact-sample-count" ) );
}


// 'DISP' 
void
proccessDISPChunk( FILE *file, u_int32_t chunkSize )
{
	u_int32_t	type;
	
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


// 'LIST' 
void
proccessLISTChunk( FILE *file, u_int32_t chunkSize )
{
	char	list_type[4];

	// Check the type of list
	if (fread(&list_type, sizeof(list_type), 1, file)!=1)
		handle_error("Error: unable to read LIST type\n");

	if (memcmp("INFO", &list_type, sizeof(list_type))!=0) {
		fprintf(stderr, "Warning: Unsupported LIST type '%4.4s'.\n", (char*)list_type);
		return;
	}

	// Remove the length of the type bits
	chunkSize -= sizeof(list_type);



	while(chunkSize) {
		char	infoType[4];
		u_int32_t	subSize;
		int	isStr = 1;
		
		if (fread(&infoType, sizeof(infoType), 1, file)!=1)
			handle_error("Error: unable to read INFO sub-type\n");
		
		subSize= read_uint32( file, "INFO sub-size" );
			
		if (debug) {
			printf("INFOtype %4.4s\n", (char*)infoType);
			printf("INFOsubsize %x\n", subSize);
		}

		// Extract the INFO sub item
//		if (memcmp("ISRF", &infoType, sizeof(infoType))==0) {
//			printf("info-intro-time: ");
//		} else if (memcmp("IMED", &infoType, sizeof(infoType))==0) {
//			printf("info-segue-time: ");
		if (memcmp("HKST", &infoType, sizeof(infoType))==0) {
			printf("info-hook-start: ");
		} else if (memcmp("HKEN", &infoType, sizeof(infoType))==0) {
			printf("info-hook-end: ");
		} else if (memcmp("BFAD", &infoType, sizeof(infoType))==0) {
			printf("info-segue-fade: ");
		} else if (memcmp("BCPR", &infoType, sizeof(infoType))==0) {
			printf("info-copyright: ");
		} else if (memcmp("ISRC", &infoType, sizeof(infoType))==0) {
			printf("info-source: ");
		} else if (memcmp("ISRF", &infoType, sizeof(infoType))==0) {
			printf("info-source-form: ");
		} else if (memcmp("IART", &infoType, sizeof(infoType))==0) {
			printf("info-artist: ");
		} else if (memcmp("INAM", &infoType, sizeof(infoType))==0) {
			printf("info-name: ");
		} else if (memcmp("IGRE", &infoType, sizeof(infoType))==0) {
			printf("info-genre: ");
		} else if (memcmp("IBPM", &infoType, sizeof(infoType))==0) {
			printf("info-bpm: ");
		} else if (memcmp("IALB", &infoType, sizeof(infoType))==0) {
			printf("info-album: ");
		} else if (memcmp("IYER", &infoType, sizeof(infoType))==0) {
			printf("info-year: ");
		} else if (memcmp("ICMT", &infoType, sizeof(infoType))==0) {
			printf("info-comments: ");
		} else if (memcmp("ICOP", &infoType, sizeof(infoType))==0) {
			printf("info-copyright: ");
		} else if (memcmp("ICRD", &infoType, sizeof(infoType))==0) {
			printf("info-creation-date: ");
		} else if (memcmp("IGNR", &infoType, sizeof(infoType))==0) {
			printf("info-genre: ");
		} else if (memcmp("IKEY", &infoType, sizeof(infoType))==0) {
			printf("info-keywords: ");
		} else if (memcmp("IPUB", &infoType, sizeof(infoType))==0) {
			printf("info-publisher: ");
		} else if (memcmp("ICOM", &infoType, sizeof(infoType))==0) {
			printf("info-composer: ");
		} else if (memcmp("IURL", &infoType, sizeof(infoType))==0) {
			printf("info-url: ");
		} else if (memcmp("IENG", &infoType, sizeof(infoType))==0) {
			printf("info-engineer: ");
		} else if (memcmp("ITCH", &infoType, sizeof(infoType))==0) {
			printf("info-technician: ");
		} else if (memcmp("ITRK", &infoType, sizeof(infoType))==0) {
			printf("info-track: ");
		} else if (memcmp("IARL", &infoType, sizeof(infoType))==0) {
			printf("info-archival-location: ");
		} else if (memcmp("ICMS", &infoType, sizeof(infoType))==0) {
			printf("info-commissioned: ");
		} else if (memcmp("ICRD", &infoType, sizeof(infoType))==0) {
			printf("info-creation-date: ");
		} else if (memcmp("IMED", &infoType, sizeof(infoType))==0) {
			printf("info-medium: ");
		} else if (memcmp("IPRD", &infoType, sizeof(infoType))==0) {
			printf("info-product: ");
		} else if (memcmp("ISBJ", &infoType, sizeof(infoType))==0) {
			printf("info-subject: ");
		} else if (memcmp("ISFT", &infoType, sizeof(infoType))==0) {
			printf("info-software: ");
		} else {
			fprintf(stderr, "Warning: Unknown INFO sub-chunk type '%4.4s'.\n", (char*)&infoType);
			
			// Skip the chunk's data
			if (fseek(file, subSize, SEEK_CUR)!=0)	
				handle_error("Error: unable to seek to skip data\n");
				
			isStr = 0;	// Don't display
		}
		
		// Display the data (if it is a string)
		if (isStr) read_print_text( file, subSize );

		// Remove the length of the sub-sub type and size 
		chunkSize -= sizeof(infoType)+sizeof(subSize)+subSize;
	}

}









// Reads in a sub chunk starting at offset
// and returns the postion of the next sub chunk
int
proccessSubChunk(FILE* file, int seek)
{
	char type[4];
	u_int32_t chunkSize;
	u_int32_t nextSubChunk;

	// DEBUGGING
	if (debug) fprintf(stderr, "  Sub-Chunk at: %8.8x\n", seek);
	
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
	u_int32_t chunkSize;
	u_int32_t nextChunk;
	u_int32_t subSeek;

	// DEBUGGING
	if (debug) printf("Chunk at: %8.8x\n", seek);

	// Seek to the start of the chunk
	if (seek != fseek(file, seek, SEEK_SET))
		handle_error("Error: unable to seek to start of chunk\n");

	// Read in the chunk type
	if (fread(&type, sizeof(type), 1, file)!=1)
		handle_error("Error: unable to read chunk type\n");

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
		handle_error("Error: unable to chunk format\n");


	// DEBUGGING
	if (debug) printf("Chunk Format: %4.4s\n", (char*)&format);
	
	// Make sure it is WAVE
	if (memcmp("WAVE", &format, sizeof(format))!=0) {
		fprintf(stderr, "Error: unsupport chunk format: %4.4s\n", format);
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
	fprintf(stderr, "bwfinfo version %s\n", VERSION);
	fprintf(stderr, "Displays information about a WAVE file in RFC822 style format.\n\n");
	fprintf(stderr, "Usage: %s [-d] <filename.wav>\n\n", progname);
	exit(1);
}


int
main(int argc, char **argv)
{
	u_int32_t seek = 0;
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
	if(stat(filename, &fileInfo))	handle_error("Error: unable to stat file\n");

	// Open the file
	file = fopen(filename, "r");
	if (file==NULL) handle_error("Error: unable to open file\n");

	// Get chunks until the next chunk is
	// beyond the end of the file
	while (seek < fileInfo.st_size) {
		seek = proccessChunk( file, seek );
	}
	
	// Display the length (time) of the file
	if (byteRate)	printf("wave-duration: %f\n", (float)audioDataLen/byteRate);
	else			printf("wave-duration: unknown\n");
	
	// Close the file
	fclose(file);
	
	// Success !
	return 0;
}



