/**
 * Gedare Bloom
 * create-prog.c
 *
 * Converts a hex code listing of the Text and Data segments into a program
 * in the format expected by the simulator.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define DEBUG

static void read_and_write(FILE *in, FILE *out, size_t count)
{
	char *buf;
	size_t bufsz = 100;
	uint32_t val;
	int i = 0;

	buf = malloc(bufsz);
	assert(buf);

	/* Read a line of hex from input file and write to out file */
	while (getline(&buf, &bufsz, in) > 0) {
		buf[strnlen(buf, bufsz)-1] = 0; /* replace \n with 0 */
		val = (uint32_t)strtol(buf, NULL, 16);
#if defined(DEBUG)
		printf("%s = %X\n", buf, val);
#endif
		fwrite(&val, sizeof(uint32_t), 1, out);
		i++;
	}
	assert( i <= count );

	val = 0;
	while( i++ < count ) {
		fwrite(&val, sizeof(uint32_t), 1, out);
	}

	free(buf);
}

void usage(char *name)
{
	printf("Usage: %s [text segment] [data segment] [output program]\n\
where:\n\
\t[text segment] is a file containing hexadecimal encoding of the code\n\
\t[data segment] is a file containing hexadecimal encoding of the data\n\
\t[output program] is a file to put the program in the expected format.\n",
	 	name);
	exit(1);
}

int main( int argc, char *argv[] )
{
	FILE *text, *data, *output;

	if ( argc < 4 ) usage(argv[0]);

	/* Open the text segment file for reading */
	data = fopen(argv[1], "r");
	text = fopen(argv[2], "r");
	output = fopen(argv[3], "w");

	assert(data);			// TODO: Fixed the argument problem that Dr. Bloom had 
	assert(text);
	assert(output);

	read_and_write(data, output, 2048);	
	read_and_write(text, output, 1024);		

	fclose(data);
	fclose(text);
	fclose(output);

	return 0;
}
