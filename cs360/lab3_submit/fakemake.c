//CS360 Lab 3: Fakemake
//By: David Gilson
//The purpose of this lab is to create a simplified version of the Linux "make" program.  There are specific files
//with the tag .fm that contain c file, h file, an executable name, gcc flags, and library links, and this program
//reads and calls the system to make the executable the way the .fm file needs it.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//I made 2 helper functions to make the object files and the executable.
void make_compile(char *name, Dllist f);
void make_exec(char *ex, Dllist f, Dllist c, Dllist l);

int main(int argc, char ** argv){	
	IS is;
	int i, state, echeck;
	Dllist c, h, l, f;
	char *ex;

	//Put the .fm info dllists to read from later.
	c = new_dllist();
	h = new_dllist();
	l = new_dllist();
	f = new_dllist();

	//Command line error checking 
	if(argc == 1){
		is = new_inputstruct("fmakefile");
		if(is == NULL){
			printf("fmakefile not made\n");
			exit(1);
		}
	}
	else if (argc == 2){
		is = new_inputstruct(argv[1]);
		if(is == NULL){
			printf("Error Opening %s\n", argv[1]);
			exit(1);
		}
	}
	else{
		printf("Format: ./fmakefile || ./fmakefile [file-name]\n");
		exit(1);
	}
	
	echeck = 0;
	state = 0;
	
	//Go through the .fm file, and put the program in a state when reading "E", "F", etc.
	//When in a state, read whatever is left in that line and put it in the corresponding 
	//dllist.  Check for executable line and make sure there isn't more than one name.
	while(get_line(is) >= 0){
		for(i = 0; i < is->NF; i++){
			if(strcmp(is->fields[i], "C") == 0){
				state = 1;
			}
			else if(strcmp(is->fields[i], "H") == 0){
				state = 2;
			}
			else if(strcmp(is->fields[i], "L") == 0){
				state = 3;
			}
			else if(strcmp(is->fields[i], "F") == 0){
				state = 4;
			}
			else if(strcmp(is->fields[i], "E") == 0){
				state = 5;
				echeck++;
				if(echeck > 1){
        			fprintf(stderr, "fmakefile (%d) cannot have more than one E line\n", is->line);
        			exit(1);
    			}
			}
			else{
				if(state == 1){
					dll_append(c, new_jval_s(strdup(is->fields[i])));
				}
				if(state == 2){
                    dll_append(h, new_jval_s(strdup(is->fields[i])));
                }
				if(state == 3){
                    dll_append(l, new_jval_s(strdup(is->fields[i])));
                }
				if(state == 4){
                    dll_append(f, new_jval_s(strdup(is->fields[i])));
                }
				if(state == 5){
					ex = strdup(is->fields[i]);
				}
				if(i == ((is->NF) - 1)){
					state = 0;
				}
			}
		}
	}
	
	//Check if there is an executable
	if(echeck == 0){
		fprintf(stderr, "No executable specified\n");
		exit(1);
	}

	//Check the header files with stat, find time of latest header change
	Dllist tmp1;
	int exist, o_exist, l_exist;
	int o_create = 0;
	struct stat buf;
	long h_max_time = 0;
	long c_max_time = 0;
	long o_max_time = 0;
	dll_traverse(tmp1, h){
		exist = stat(tmp1->val.v, &buf);
		if(exist < 0){
			fprintf(stderr, "fmakefile: %s: No such file or directory\n", tmp1->val.v);
			exit(1);
		}
		else{
			if(buf.st_mtime > h_max_time){
				h_max_time = buf.st_mtime;
			}
		}
	}
		
	char *object;

	//Check the C and .o files with stat.  Make the object files if needed.
	dll_traverse(tmp1, c){
		exist = stat(tmp1->val.v, &buf);
		c_max_time = buf.st_mtime;
		if(exist < 0){
			fprintf(stderr, "fmakefile: %s: No such file or directory\n", tmp1->val.v);
			exit(1);
		}
		else{
			object = strdup(tmp1->val.v);
			object[strlen(object) - 2] = '\0';
			strcat(object, ".o");
			o_exist = stat(object, &buf);
			if(o_exist < 0){
				o_create = 1;
				make_compile((char *)tmp1->val.v, f);
			}
			else{
				if(buf.st_mtime > o_max_time){
					o_max_time = buf.st_mtime;
				}
				if((buf.st_mtime < c_max_time) || (buf.st_mtime < h_max_time)){
					o_create = 1;
					make_compile((char *)tmp1->val.v, f);
				}
			}
		}
	}
	
	exist = stat(ex, &buf);

	//If an executable doesn't exist or one of the header files, c files, or o files have been updated, create the executable.
	//Otherwise, the executable is up to date.
	if((exist < 0) || (buf.st_mtime < h_max_time) || (buf.st_mtime < c_max_time) || (buf.st_mtime < o_max_time) || (o_create == 1)){
		make_exec(ex, f, c, l);
	}
	else{
		printf("%s up to date\n", ex);
		exit(1);
	}
	
	//Free inputstruct, dllists, and corresponding strings.
	jettison_inputstruct(is);
	
	free(ex);
	free(object);

	dll_traverse(tmp1, c){
		free((char *)tmp1->val.v);
	}
	dll_traverse(tmp1, h){
		free((char *)tmp1->val.v);
    }
	dll_traverse(tmp1, l){
		free((char *)tmp1->val.v);
	}
	dll_traverse(tmp1, f){
		free((char *)tmp1->val.v);
	}

	free_dllist(c);
	free_dllist(h);
	free_dllist(l);
	free_dllist(f);

	return 0;
}

//Function to make the object file given a c file and header files.
void make_compile(char *name, Dllist f){
	int systest;
	Dllist tmp;
	char *compile = malloc(50);
	strcat(compile, "gcc -c");
	dll_traverse(tmp, f){
		strcat(compile, " ");
		strcat(compile, tmp->val.v);
	}
	strcat(compile, " ");
	strcat(compile,name);
	printf("%s\n", compile);
	systest = system(compile);
	if(systest != 0){
		fprintf(stderr, "Command failed.  Exiting\n");
		exit(1);
	}	

	free(compile);
	return;
}

//Function to make the executable.  Given an executable name, list of flags, c files, and library files.
void make_exec(char *ex, Dllist f, Dllist c, Dllist l){
	Dllist tmp, tmp1, tmp2;
	char *link = malloc(100);
	char *ob = malloc(50);
	char *lib = malloc(50);
	char *flags = malloc(50);
	int systest;
	strcat(link, "gcc -o");
	strcat(link, " ");
	strcat(link, ex);

	//Add flags to link
	dll_traverse(tmp, f){
		flags = (char *)tmp->val.v;
		strcat(link, " ");
		strcat(link, flags);
	}
	free(flags);

	//Make object file list to put on end
	dll_traverse(tmp1, c){
		ob = (char *)tmp1->val.v;
		ob[strlen(ob) - 2] = '\0';
		strcat(ob, ".o");
		strcat(link, " ");
		strcat(link, ob);
	}
	free(ob);

	dll_traverse(tmp2, l){
		lib = (char *)tmp2->val.v;
		strcat(link, " ");
		strcat(link, lib);
	}
	free(lib);

	printf("%s\n", link);
	systest = system(link);
	if(systest != 0){
		fprintf(stderr, "Command failed.  Fakemake exiting\n");
		exit(1);
	}
	free(link);
	return;
}
