//CS 360 Lab2: Buffering Part 1
//By David Gilson
//In this lab, we read in a binary file and convert the data into a ip/host format for the user to 
//lookup a host's ip.  In part one, we use buffered I/O commands like fread to read from the 
//binary file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//Here's the struct I created to go into a JRB tree.  Holds the IP and the host name.
typedef struct{
	char *name;
	char ip[3];
}Host;

//A function to create a new host and insert them into the JRB tree.
void new_host(char ip[3], char * name, JRB host_t);

int main(int argc, char **argv){
	JRB host_t;
	char ipa[3];
	unsigned char num[3];
	char buffer;
	char word[1000];

	host_t = make_jrb();

	FILE *fd;
	fd = fopen("converted", "r");

	if(fopen == NULL){
		perror("error opening converted");
		exit(1);
	}
	
	//While able to read the next IP address, do the logic to create a new host.
	while(fread(ipa, sizeof(char), 4, fd)){
		
		//Read the number of host names
		fread(num, sizeof(unsigned char), 4, fd);

		int test = 0;
		int abs = 0;
		int i = 0;
		int j = 0;
		int k = 0;
		while(1){
			char *local;
			char *absolute;
			char *temp;
			
			//Read in one byte at a type, counting the number of '\0' characters
			fread(&buffer, sizeof(char), 1, fd);

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
	int check = 0;
	
	//Use scanf to look at input and if EOF, break.
	//Otherwise, traverse the tree, look for the name. If name found, take ip, and
	//look through tree again for hosts with that ip and print them out.
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

	fclose(fd);
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
