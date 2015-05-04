#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Declare a pair structure for the sample pairs.
struct pair{
	short left;
	short right;
};

void removocals(FILE *sourcewav, FILE *destwav){
	int input_1, output_1, input_2, i, writer_1, writer_2;
	long size;
	char buffer[44];
	struct pair pairs;
	
	// Finds the size of the sourcewav file.
	fseek(sourcewav, 0, SEEK_END);
	size = ftell(sourcewav);
	fseek(sourcewav, 0, SEEK_SET);
	
	// Reads and writes the header samples.
	input_1 = fread(buffer, 1, 44, sourcewav);
  	if(input_1 != 44){
  	    fprintf(stderr, "\n fread error! The program exits!\n");
  	    exit(1);
  	    }
	output_1 = fwrite(buffer, 1, input_1, destwav);
  	if(output_1 != input_1){
  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
  	    exit(1);
  	    }
	
	// Read the rest of the samples as shorts.
	short *array = (short *)malloc(size-44);
	input_2 = fread(array, sizeof(short), (size-44)/sizeof(short), sourcewav);
  	if(input_2 != (size-44)/sizeof(short)){
  	    fprintf(stderr, "\n fread error! The program exits!\n");
  	    exit(1);
  	    }
	
	// Computes combined samples and writes two copies of them into the destwav file.
	for(i = 0; i < (size-44)/sizeof(short); i=i+2){
	pairs.left = array[i];
	pairs.right = array[i+1];
	short combined = (pairs.left - pairs.right) / 2;
	writer_1 = fwrite(&combined, sizeof(combined) , 1, destwav);
  	if(writer_1 != 1){
  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
  	    exit(1);
  	    }
	writer_2 = fwrite(&combined, sizeof(combined) , 1, destwav);
  	if(writer_2 != 1){
  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
  	    exit(1);
  	    }
	}
	free(array);
}

int main(int argc, char **argv){
	FILE *sourcewav, *destwav;
	sourcewav = fopen(argv[1], "rb");
	destwav = fopen(argv[2], "wb");
	
	if(argc != 3){
		fprintf(stderr, "\n Invalid input!\n");
		exit(1);
	}
	
	if(sourcewav == NULL){
	    fprintf(stderr, "\n The input file doesn't exist or cannot be opened!\n");
	    exit(1);
	    }
		
	if(destwav == NULL){
		fprintf(stderr, "\n The output file cannot be opened!\n");
		exit(1);
		}
	
	removocals(sourcewav, destwav);
	
	fclose(sourcewav);
	fclose(destwav);
	
	return 0;
}