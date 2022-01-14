//CS 360 Lab 2: Buffering Part 2
//By David Gilson
//This is the second part of Lab 2.  It does the same thing as part 1, but it doesn't use 
//buffered I/O commands.  We are only aloud to use open, read, and close to handle reading
//the binary file.  I will only comment the changed code in this one.
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
	char buffer;
	char word[1000];

	host_t = make_jrb();

	//Now we use open, which returns an int.  If the int is -1, there is an error, 
	//otherwise it is opened correctly.
	int fd;
	fd = open("converted", O_RDONLY);
	if(fd < 0) { perror("error opening converted"); exit(1);}
	
	//Similar to part 1, while we can read in the next ip address, we'll continue making new hosts.
    while(sz = read(fd, ipa, 4)){

		//Read in the number of names using read instead of fread.
		sz = read(fd, num, 4);

		int test = 0;
		int abs = 0;
		int i = 0;
		int j = 0;
		int k = 0;
		while(1){
			char *local;
			char *absolute;
			char *temp;
			
			//Read in a single char and determine logic from that byte.
			sz = read(fd, &buffer, 1);
			word[i] = buffer;

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
			if(test == (int)num[3]){
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
        free(t3->ip);
		free(t3);
    }
    jrb_free_tree(host_t);
	
	//Close file using close
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
