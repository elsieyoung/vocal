#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<ctype.h>
#include <stdlib.h>

#define HEADER_SIZE 44

void addecho(FILE *sourcewav, FILE *destwav, int delay, int volume_scale){
	int input_1, output_1, input_echo, output_echo, reader, writer, l, x, m, n, j, u, q, p;
	long size;
	short header[HEADER_SIZE];
	short result;
	
	// Finds the size of the sourcewav file.
	fseek(sourcewav, 0, SEEK_END);
	size = ftell(sourcewav);
	fseek(sourcewav, 0, SEEK_SET);
	
	// Change the two int in header and writes the header into the destwav file.
    input_1 = fread(header, 1, HEADER_SIZE, sourcewav);
  	if(input_1 != HEADER_SIZE){
  	    fprintf(stderr, "\n fread error! The program exits!\n");
  	    exit(1);
  	    }
	unsigned int *sizeptr = (unsigned int *)(header + 2);
	*sizeptr += delay * 2;
	unsigned int *sizeptr_2 = (unsigned int *)(header + 20);
	*sizeptr_2 += delay * 2;
	output_1 = fwrite(header, 1, input_1, destwav);
  	if(output_1 != input_1){
  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
  	    exit(1);
  	    }
	
	// if delay is smaller than the #sample in orig
	if((delay-((size-44)/2))<=0){	
		short *echo_buf = (short *)malloc(delay*sizeof(short));
		short *buf = (short *)malloc(delay*sizeof(short));
	
	    //reads delay samples before mixing
		input_echo = fread(echo_buf, sizeof(short), delay, sourcewav);
	  	if(input_echo != delay){
	  	    fprintf(stderr, "\n fread error! The program exits!\n");
	  	    exit(1);
	  	    }
		output_echo = fwrite(echo_buf, sizeof(short), input_echo, destwav);
	  	if(output_echo != input_echo){
	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
	  	    exit(1);
	  	    }
		
		//Mixes the orig and delay samples and write to the destwav	
		while(!feof(sourcewav)){
			reader = fread(buf, sizeof(short), delay, sourcewav);
		  	if(reader == 0){
		  	    fprintf(stderr, "\n fread error! The program exits!\n");
		  	    exit(1);
		  	    }
			// When we reach the end of orig and there are no enough samples in orig, just mix reader times and treat the rest of samples in echo_buf as mixing with 0 samples.
			if(reader<delay){
		    	for(p=0; p < reader; p++){  
			   		 echo_buf[p] = (short)(echo_buf[p]/volume_scale);
					 result = (short)(buf[p] + echo_buf[p]);
					 writer = fwrite(&result, sizeof(result), 1, destwav);
			 	  	if(writer != 1){
			 	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
			 	  	    exit(1);
			 	  	    }
				 }
		    	for(q=reader; q < delay; q++){  
					 result = (short)(echo_buf[q]/volume_scale);
					 writer = fwrite(&result, sizeof(result), 1, destwav);
 			 	  	if(writer != 1){
 			 	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
 			 	  	    exit(1);
 			 	  	    }
				 } 
				 //Updates the echo buffer after each mixing.
				 for (u=0; u<reader; u++){
					 echo_buf[u] = buf[u]; 
				 }
			 }
		
			// Normal case when there are still enough delay samples in orig to mix with samples in echo_buf.
			else{
		    	for(j=0; j < delay; j++){  
			   		 echo_buf[j] = (short)(echo_buf[j]/volume_scale);
					 result = (short)(buf[j] + echo_buf[j]);
					 writer = fwrite(&result, sizeof(result), 1, destwav);	
 			 	  	if(writer != 1){
 			 	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
 			 	  	    exit(1);
 			 	  	    }                                                   
				 }
				 //Update the echo buffer after each mixing.
				 for (n=0; n<reader; n++){
					 echo_buf[n] = buf[n]; 
				 }
		 	}
			
		}
		//Write the rest of echo samples to the destwav file.	
		for(m=0; m<(reader);m++){
			echo_buf[m] = echo_buf[m]/volume_scale;
			writer = fwrite(&echo_buf[m], sizeof(short), 1 , destwav);
	 	  	if(writer != 1){
	 	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
	 	  	    exit(1);
	 	  	    }
		}
	
		free(buf);
		free(echo_buf);
	}
	
	//when delay is larger than #samples in orig.
	else{
		//Reads the orig into destwav file.
		short *echo_buf_2 = (short *)malloc(size-44);
		input_echo = fread(echo_buf_2, 1, size-44, sourcewav);
	  	if(input_echo != size-44){
	  	    fprintf(stderr, "\n fread error! The program exits!\n");
	  	    exit(1);
	  	    }
		output_echo = fwrite(echo_buf_2, 1, input_echo, destwav);
	  	if(output_echo != input_echo){
	  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
	  	    exit(1);
	  	    }
		
		// Adds delay-size(sample) times 0 samples.
		short *zero_buf = (short *)malloc((delay-(size-44)/sizeof(short))*sizeof(short));
		for (x=0; x<(delay-(size-44)/sizeof(short)); x++){
			zero_buf[x] = 0;
			writer = fwrite(&zero_buf[x], sizeof(short), 1, destwav);
		  	if(writer != 1){
		  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
		  	    exit(1);
		  	    }
		}
		 
		//Writes the echo at the end of the destwav file.
		for(l=0; l < (size-44)/2; l++){  
			result = (short)(echo_buf_2[l]/volume_scale);
			writer = fwrite(&result, sizeof(result), 1, destwav);	
		  	if(writer != 1){
		  	    fprintf(stderr, "\n fwrite error! The program exits!\n");
		  	    exit(1);
		  	    }                                                   
		}
		free(echo_buf_2);
		free(zero_buf);
	}	
}
	
int main(int argc, char **argv){
	FILE *sourcewav, *destwav;
	int delay = 8000;
	int volume_scale = 4;
	int option = 0;
	
	
   while ((option = getopt(argc, argv,"d:v:")) != -1) {
   		switch (option) {
			 case 'd' : 
			 delay = strtol(optarg, (char**)NULL, 10);
			 if (delay<=0 || optarg == NULL){
		   	    fprintf(stderr, "The input of option is invalid!\n");
		   	    exit(1);
			 }
			     break;
			 case 'v' : 
			 volume_scale = strtol(optarg, (char**)NULL, 10);
			 if (volume_scale<=0 || optarg == NULL){
		   	    fprintf(stderr, "The input of option is invalid!\n");
		   	    exit(1);
			 }
                 break;
			 }
		 }
		 
	  
	sourcewav = fopen(argv[argc-2], "rb");
	destwav = fopen(argv[argc-1], "wb");
  	
	
  	if(sourcewav == NULL){
  	    fprintf(stderr, "\n The input file doesn't exist or cannot be opened!\n");
  	    exit(1);
  	    }
		
  	if(destwav == NULL){
  		fprintf(stderr, "\n The output file cannot be opened!\n");
  		exit(1);
  		}
	  
		
	addecho(sourcewav, destwav, delay, volume_scale);
	
	fclose(sourcewav);
	fclose(destwav);
	
	return 0;
}