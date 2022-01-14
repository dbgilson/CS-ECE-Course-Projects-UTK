//CS 360 Lab 2: Buffering Part 3
//By David Gilson
//This is the third part of lab 2.  It does the same thing as part 1, but we are only allowed to use open, read, and close
//for reading the binary file.  Unlike part 2, we can only use the read function once, so we make a large char buffer
//to quickly read the file and read in the bytes individually from the buffer.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

typedef struct{
	char *name;
	char ip[3];
}Host;

void new_host(char ip[3], char * name, JRB host_t);

int main(int argc, char **argv){
	JRB host_t;
	int sz;
	char ipa[3];
	unsigned char num[3];
	char buffer_whole [350000];
	char buffer;
	char word[1000];

	host_t = make_jrb();

	//We still use open like normal.
	int fd;
	fd = open("converted", O_RDONLY);
	if(fd < 0) { perror("error opening converted"); exit(1);}
	
	//We read in the whole file in a 350000 byte size buffer.
	sz = read(fd, buffer_whole, 350000);

	//I use count to keep track of where I'm at in the buffer
	int count = 0;
	int j;
    while(count <= (sz - 2)){
		//Read in the ip address from the buffer.
		for(j = 0; j < 4; j++){
			ipa[j] = (char)buffer_whole[count];
			count++;
		}

		//Read in the number of names from the buffer.
		for(j = 0; j < 4; j++){
			num[j] = (unsigned char)buffer_whole[count];
			count++;
		}

		int test = 0;
		int abs = 0;
		int i = 0;
		int j = 0;
		int k = 0;
		while(1){
			char *local;
			char *absolute;
			char *temp;
			
			//Read in a single byte from the buffer
			buffer = (char)buffer_whole[count];
			word[i] = buffer;
			count++;
	
			//If we reach the EOF, break from this loop since
			//we're done reading and creating new hosts.
			if(count > sz){
				break;
			}
			if(buffer == '\0'){
				temp = (char *) malloc(sizeof(char) * (i+1)); 
				for(j = 0; j < i; j++){
					temp[j] = word[j];
				}
				if(abs == 1){
					local = strdup(temp);
					new_host(ipa, local, host_t);
					abs = 0;
				}
				else{
					local = strdup(temp);
					new_host(ipa, local, host_t);
				}
				i = 0;
				test++;
			}
			if((test == (int)num[3])){
			    break;
			}

			if((buffer == '.') && (abs == 0)){
				abs = 1;
				temp = (char *) malloc(sizeof(char) * (i+1));
				for(k = 0; k < i; k++){
					temp[k] = word[k];
				}
				absolute = strdup(temp);
				new_host(ipa, absolute, host_t);
			}
			i++;
			if(buffer == '\0'){
				i = 0;
			}
		}
	}
	printf("Hosts all read in\n\n");
	
	printf("Enter host name:  ");
	char str[30];
	
	while(scanf("%s", str) != EOF){
        JRB temp, temp2;
        Host *t;
        Host *t2;
        char *tip;

        temp = jrb_find_str(host_t, str);

        if(temp == NULL){
            printf("no key %s\n\n", str);
        }
        else{
            jrb_traverse(temp, host_t){
                t = (Host *) temp->val.v;
                if(strcmp(t->name, str) == 0){
                    tip = t->ip;
                    printf("%u.%u.%u.%u: ", (unsigned char)t->ip[0], (unsigned char)t->ip[1], (unsigned char)t->ip[2], (unsigned char)t->ip[3]);
                    jrb_traverse(temp2, host_t){
                        t2 = (Host *) temp2->val.v;
                        if((t2->ip[0] == tip[0]) && (t2->ip[1] == tip[1]) && (t2->ip[2] == tip[2]) && (t2->ip[3] == tip[3])){
                            printf(" %s", t2->name);
                        }
                    }
                    printf("\n\n");
                }
            }
        }
        printf("Enter host name:  ");
    }

	
    JRB temp3;
    Host *t3;
    jrb_traverse(temp3, host_t){
        t3 = (Host *) temp3->val.v;
        free(t3->name);
		//I tried to call free the ip as well, but the gradescript core dumps when
		//I do it only on this version of the lab.  I don't know why, so I just
		//keep it out, even though I know memory is still there ¯\_(ツ)_/¯
        //free(t3->ip);
		free(t3);
    }
    jrb_free_tree(host_t);

	//Close the file.
	close(fd);
	return 0;

}

void new_host(char ip[3], char *name, JRB host_t){
	Host *h;
	h = malloc(sizeof(Host));
	h->ip[0] = ip[0];
	h->ip[1] = ip[1];
	h->ip[2] = ip[2];
	h->ip[3] = ip[3];
	h->name = strdup(name);
	jrb_insert_str(host_t, name, new_jval_v((void *) h));
}
