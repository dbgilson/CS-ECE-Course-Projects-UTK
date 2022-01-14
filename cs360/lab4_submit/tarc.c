//CS 360: Lab 4 Jtar, tarc
//By: David Gilson
//This lab is meant to recreate the unix tar function to create a tarc file, a mimic of a 
//real tar file.  It takes in an initial directory and forms the tarc file that can be read by
//part 2 of the Jtar Lab, tarx.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//I make tarc a recursive function in order to properly traverse and keep track
//of the directories and files.
void tarc(char * fn, char * head, JRB inodes, int check, int slash);

int main(int argc, char ** argv){

	if((argc == 1) || (argc >= 3)){
		printf("Format: ./tarc [directory] < [tarc file]\n");
		exit(1);
	}

	JRB inodes;
	int check = 0;
	char * t;
	char * suffix;
	suffix = malloc(50);
	int slash = 0;
	int count = 0;

	//Here I parse the given directory to tarc into a suffix that
	//I use while formatting the tarc file.
	for(t = argv[1]; *t != '\0'; t++){
		if(*t == '/'){
			slash++;
		}
	}
	
	for(t = argv[1]; *t != '\0'; t++){
		if(count == slash){
			strcat(suffix, t);
			break;
		}
		if(*t == '/'){
			count++;
		}
	}
	
	//Keep the inodes in a jrb tree, then call the recursive function.
	inodes = make_jrb();

	tarc(argv[1], suffix, inodes, check, slash);

	//Free inodes tree and suffix variable.
	jrb_free_tree(inodes);
	free(suffix);
	return 0;
}

void tarc(char * fn, char * head, JRB inodes, int check, int slash){
	DIR *d;
	struct dirent *de;
	struct stat buf;
	int exists;
	char *s;
	char *p;
	Dllist directories, tmp;

	//Open directory
	d = opendir(fn);
	if (d == NULL){
		printf("Error opening directory %s\n", fn);
		exit(1);
	}
	
	//Malloc some chars * to be able to hold the directory names.
	s = (char *) malloc(sizeof(char) * (strlen(fn) + 25));
	p = (char *) malloc(sizeof(char) * (strlen(fn) + 25));
	directories = new_dllist();

	for(de = readdir(d); de != NULL; de = readdir(d)){
		//If this is the first directory, print only the head.
		//Otherwise, print the head route + the name of the file.
		if(check == 0){
			sprintf(s, "%s", head);
		}
		else{
			sprintf(s, "%s/%s", head, de->d_name);
		}
		
		sprintf(p, "%s/%s", fn, de->d_name);
		exists = lstat(p, &buf);

		if(exists < 0){
			fprintf(stderr, "Couldn't stat %s\n", s);
		}
		else{		
			if((strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".") != 0) || (check == 0)){
				int i;
				i = strlen(s);
				fwrite(&i, 4, 1, stdout);

                //Print File Name
                printf("%s", s);

                //Print Inode
				long j = buf.st_ino;
				fwrite(&j, 8, 1, stdout);

				if((jrb_find_int(inodes, buf.st_ino) == NULL)){	
					//First time we encountered a file or directory
					jrb_insert_int(inodes, buf.st_ino, JNULL);
			
					//Print File's Mode
					i = buf.st_mode;
					fwrite(&i, 4, 1, stdout);
				
					//Print File's Last modification time
					j = buf.st_mtime;
					fwrite(&j, 8, 1, stdout);
				
					//If the file is a file and not a directory
					if(!(S_ISDIR(buf.st_mode))){
						//Print File's Size
						j = buf.st_size;
						fwrite(&j, 8, 1, stdout);
		
						//Print File's Bytes
						FILE *fd;
						char *byte;
						byte = malloc(sizeof(char) * j);
						
						fd = fopen(p, "r");

						if(fd == NULL){
							fprintf(stderr, "Error opening %s\n", s);
							exit(1);
						}
						fread(byte, sizeof(char), j, fd);
						fwrite(byte, sizeof(char), j, stdout);
						fclose(fd);

						//Free the byte variable.
						free(byte);
					}
					//When this first becomes 1, we are past the inital directory.
					check = 1;
				}
			}
		}
		if(S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && check == 1){	
			//Add any found directories to a dllist to traverse later.
			dll_append(directories, new_jval_s(strdup(p)));
		}
	}

	closedir(d);

	//Traverse the directories dllist, and for each directory, recursively
	//call tarc on it.
	dll_traverse(tmp, directories){
		char * t;
    	int count = 0;

		//This correctly parses the proper route from the first
		//directory to put into the next tarc.
    	for(t = tmp->val.s; *t != '\0'; t++){
        	if(count == slash){
            	break;
        	}
        	if(*t == '/'){
            	count++;
        	}
    	}

		tarc(tmp->val.s, t, inodes, check, slash);
	}

	//Free the directory list and the s and p variables.
	free_dllist(directories);
	free(s);
	free(p);
	return;
}
