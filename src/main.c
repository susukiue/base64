#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "base64.h"

#define UNKNOWN_OPTION_EXCEPTION -19
#define SYNC_ACTION_EXCEPTION -20
#define NOT_ACTION_EXCEPTION -21
#define WRITE_EXCEPTION -22

#define BUFFER_SIZE 301

extern char *optarg;
extern int optind, optopt, opterr;

int readin(char *buffer, FILE *stream, int newline){
	char ch;
	int size = 0;
	while(size < BUFFER_SIZE-1){
		ch = fgetc(stream);
		if(ch == EOF){
			break;
		}
		if(newline || ch != '\n'){
			buffer[size++] = ch;
		}
		else{
			break;
		}
	}
	return size;
}

void writeres(char *program, char *outfile, size_t size, const char *buffer, int newline){
	if(buffer != NULL){
		if(program == NULL){
			program = "[ERROR]";
		}
		FILE *fp = fopen(outfile, "a");
		if(fp != NULL){
			if(size != fwrite(buffer, 1, size, fp)){
				fprintf(stderr, "%s: An exception occurred while writing to the file %s !\n", program, outfile);
				exit(WRITE_EXCEPTION);
			}
			if(!newline){
				putc('\n', fp);
			}
			fclose(fp);
		}
		else{
			if(size != fwrite(buffer, 1, size, stdout)){
				fprintf(stderr, "%s: An exception occurred while writing to STDOUT !\n", program);
				exit(WRITE_EXCEPTION);
			}
			if(!newline){
				putc('\n', stdout);
			}
		}
	}
}

void* warpbuffer(void* another, int size){
	return another;
}

char* warpaction(char* program, char de, char en, const char *str, char *buffer){
	char *result = buffer;
	if(program == NULL){
		program = "[ERROR]";
	}
	if(de && en){
		fprintf(stderr, "%s: Cannot use both actions at the same time !\n", program);
		exit(SYNC_ACTION_EXCEPTION);
	}
	else{
		if((de || en) && buffer == NULL){
			buffer = calloc(1, BUFFER_SIZE);
			result = buffer;
		}
		if(de){
			_base64_comple(1, str, warpbuffer, buffer);
		}
		else if(en){
			_base64_comple(0, str, warpbuffer, buffer);
		}
		else{
			fprintf(stderr, "%s: It takes an action to continue !\n", program);
			exit(NOT_ACTION_EXCEPTION);
		}
	}
	return result;
}

int main(int argc, char *argv[]){
	int append = 0, block = 0, newline = 0;
	char opt = 0, decode = 0, encode = 0, buffer[BUFFER_SIZE] = {0}, *temptr = NULL, *infile = NULL, *outfile = NULL;

	while((opt = getopt(argc, argv, "hvabndcf:o:")) != -1){
		switch(opt){
			case 'h':
				printf("Usage: %s [-hvabndc] [-fo file] [text]...\n", argv[0]);
			case 'v':
				printf("Version 1.0 issue https://github.com/susukiue/base64\n");
				exit(EXIT_SUCCESS);
			case 'a':
				append = 1; break;
			case 'b':
				block = 1; break;
			case 'n':
				newline = 1; break;
			case 'd':
				decode = 1; break;
			case 'c':
				encode = 1; break;
			case 'f':
				infile = optarg; break;
			case 'o':
				outfile = optarg; break;
			case '?':
			default:
				if(optarg == NULL){
					break;
				}
				exit(EXIT_FAILURE);
		}
	}

	if(!append && outfile != NULL){
		FILE *fp = fopen(outfile, "w");
		if(fp != NULL){
			fclose(fp);
		}
	}

	if(block){
		while(readin(buffer, stdin, newline)){
			temptr = warpaction(argv[0], decode, encode, buffer, temptr);
			writeres(argv[0], outfile, base64_overlen(temptr, 0), temptr, newline);
            base64_init_zero(temptr, 0);
		}
	}
	if(infile != NULL){
		FILE *fp = fopen(infile, "r");
        if(fp != NULL){
            while(readin(buffer, fp, newline)){
                temptr = warpaction(argv[0], decode, encode, buffer, temptr);
                writeres(argv[0], outfile, base64_overlen(temptr, 0), temptr, newline);
                base64_init_zero(temptr, 0);
            }
            fclose(fp);
        }
        else{
			fprintf(stderr, "%s: The input file '%s' does not exist !\n", argv[0], infile);
			exit(EXIT_FAILURE);
		}
	}
	free(temptr);
	while(optind < argc){
		while(base64_overlen(argv[optind], 0)){
			warpaction(argv[0], decode, encode, argv[optind++], buffer);
			writeres(argv[0], outfile, base64_overlen(buffer, 0), buffer, newline);
            base64_init_zero(buffer, 0);
		}
	}

	return 0;
}
