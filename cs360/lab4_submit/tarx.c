//CS 360: Lab 4, Jtar, tarx
//By: David Gilson
//This is the second part of the Jtar lab.  This code mimic opening a tar file by using the 
//function tarx to open a tarc file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//I use a struct called File to hold file data in order to reference
//when I used chmod and utimes on the file.
typedef struct{
	char *name;
	int mode;
	long modtime;
	long inode;
}File;

//I have a new file function to properly create and allocate the File.
void new_file(char *name, int mode, long modtime, long inode, JRB files);
	
int main(int argc, char **argv){
	int nsize;
	int systest;
	int bc = 0;
	int rtest = 0;

	//I use 2 JRBs, one for the inodes, and one for the Files
	JRB inodes;
	JRB files;
	inodes = make_jrb();
	files = make_jrb();

	//Reading from standard in.  Start with reading name size.
	while(fread(&nsize, sizeof(int), 1, stdin)){
		if(nsize <= 0){
			fprintf(stderr, "Bad tarc file at byte %d.  File name size is %d, which I can't malloc.\n", bc, nsize);
			exit(1);
		}

		//bc variable is used to keep track of the current byte
		//we are at in the tarc stream.
		bc = bc + 4;

		//Reading name
		char *name;
		name = malloc(sizeof(char) * (nsize) + 1);
		rtest = fread(name, sizeof(char), nsize, stdin);
		bc = bc + rtest;
		if(rtest != nsize){
			fprintf(stderr, "Bad tarc file at byte %d.  File name size is %d, but bytes read = %d.\n", bc, nsize, rtest);
			exit(1);
		}
		
		//Manually add null character to end of name since
		//the tarc file doesn't store them.
		name[nsize] = '\0';
		
		//Reading inode
		long inode;
		rtest = fread(&inode, sizeof(long), 1, stdin);
		if(rtest < 1){
			fprintf(stderr, "Bad tarc file for %s.  Couldn't read inode\n", name);
			exit(1);
		}
		bc = bc + 8;

		if(jrb_find_int(inodes, inode) == NULL){
			//First interaction with inode
			jrb_insert_int(inodes, inode, JNULL);
			
			//Reading mode
			int mode;
			if(fread(&mode, sizeof(int), 1, stdin) < 1){
				fprintf(stderr, "Bad tarc file for %s.  Couldn't read mode\n", name);
				exit(1);
			}
			bc = bc + 4;

			//Reading last modification time
			long modtime;
			if(fread(&modtime, sizeof(long), 1, stdin) < 1){
				fprintf(stderr, "Bad tarc file for %s.  Couldn't read mtime\n", name);
				exit(1);
			}
			bc = bc + 8;

			//Make a new File
			new_file(name, mode, modtime, inode, files);

			//Check if directory or file
			if(S_ISDIR(mode)){
				//If Directory, call mkdir.
				char *dir;
				dir = malloc(sizeof(char) * (nsize) + 6);
				sprintf(dir, "mkdir %s", name);
				systest = system(dir);
				if(systest != 0){
					fprintf(stderr, "Command failed.  tarx exiting\n");
					exit(1);
				}
				free(dir);
			}
			else{
				//Not a directory, so must be file
				long fsize;
				char *byte;
				
				//Read in file size.
				fread(&fsize, sizeof(long), 1, stdin);
				bc = bc + 8;
				byte = malloc(sizeof(char) * fsize);

				//Read in file data from tarc, then write it to an open file.
				FILE *t;
				t = fopen(name, "w");
				if(t == NULL){
					fprintf(stderr, "%s: No such file or directory\n", name);
					exit(1);
				}
				if(fread(byte, sizeof(char), fsize, stdin) < fsize){
					fprintf(stderr, "Bad tarc file for %s.  Trying to read %ld bytes of the file, and got EOF\n", name, fsize);
					exit(1);
				}
				bc = bc + fsize;
				fwrite(byte, sizeof(char), fsize, t);
				fclose(t);
				free(byte);
			}
		}
		else{
			//Hard link, Repeated inode
			JRB temp;
			File *f;
			jrb_traverse(temp, files){
				f = (File *) temp->val.v;
				if(f->inode == inode){
					//Use system with ln call to create link
					//Also add the file to the JRB tree.
					new_file(name, f->mode, f->modtime, f->inode, files);
					char *link;
					link = malloc(sizeof(char) * (strlen(f->name) + strlen(name)) + 5);
					sprintf(link, "ln %s %s", f->name, name);
					systest = system(link);
					if(systest != 0){
						fprintf(stderr, "Command failed.  tarx exiting\n");
						exit(1);
					}
					break;
					free(link);
				}
			}
		}
		free(name);
	}
	
	JRB tmp;
	File *tfile;
	int check;

	//Reverse traverse the files tree, calling utimes() and chmod()
	//with the proper modtime and mode of that file (found in file struct).
	jrb_rtraverse(tmp, files){
		tfile = (File *) tmp->val.v;

		struct timeval times[2];
		gettimeofday(&times[0], NULL);
		times[1].tv_sec = tfile->modtime;
		times[1].tv_usec = 0;

		check = utimes(tfile->name, times);
		if(check != 0){
			printf("Error calling utimes on %s\n", tfile->name);
			exit(1);
		}

		check = chmod(tfile->name, tfile->mode);
		if(check != 0){
			printf("Error calling chmod on %s\n", tfile->name);
		}

		free(tfile->name);
		free(tfile);
	}

	//Free the jrb trees and return
	jrb_free_tree(files);	
	jrb_free_tree(inodes);
	return 0;
}

//Function to create new file, malloc the size of the file and the char * name variable,
//and insert the newly created file into the JRB files tree.
void new_file(char *name, int mode, long modtime, long inode, JRB files){
	File *f;
	f = malloc(sizeof(File));
	f->name = malloc(sizeof(char) * strlen(name) + 1);
	sprintf(f->name, "%s", name);
	f->mode = mode;
	f->modtime = modtime;
	f->inode = inode;
	jrb_insert_int(files, inode, new_jval_v((void *) f));
	return;
}
