//CS 360 Lab 1: Famtree
//By: David Gilson
//The point of this lab is to take in a list of people with specified relations between them and create
//a sort of family tree lineage to display on stdout.  It is also a lab to help us get familiarized 
//with Dr. Plank's C libraries for use in future projects.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//Person struct, along with functions to create and handle a Person
typedef struct{
	char* name;
	char* sex;
	char* father;
	char* mother;
	Dllist children;
	int visited;
	int printed;
}Person;

Person * new_person(char * name, JRB p_list);
char * get_name(IS is);
void insert_child(Person * p, Dllist children);
int check_cycle(Person * p);
void print_jrb(JRB p_list);

int main(int argc, char **argv)
{
  //We use IS to read input and use JRBs to form the tree lineage.
  IS is;
  JRB p_list, temp;
  Person *p;
  Person *t;
  char* name;
  int i;

  is = new_inputstruct(argv[1]);
  p_list = make_jrb();
  if (is == NULL){
    perror(argv[1]);
	exit(1);
  }

  while(get_line(is) >= 0){
    for(i = 0; i < is->NF; i++){

		//If reading in Person, check to see if they're in the tree.
		//If not create a new person
		if(strcmp(is->fields[i], "PERSON") == 0){
			name = strdup(get_name(is));
			temp = jrb_find_str(p_list, name);
			if(temp == NULL){
				p = new_person(name, p_list);
			}
			else{
				p = (Person *) temp->val.v;
			}
		}
		
		//If reading in SEX, fill in person's sex and check for errors.
		else if (strcmp(is->fields[i], "SEX") == 0){
			if(strcmp(is->fields[i+1], "M") == 0){
				if(strcmp(p->sex, "Female") == 0){
					fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
					exit(1);
				}
				p->sex = "Male";
			}
			else if(strcmp(is->fields[i+1], "F") == 0){
				if(strcmp(p->sex, "Male") == 0){
					fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
					exit(1);
				}
				p->sex = "Female";
			}
			else{
				fprintf(stderr, "SEX can only be M or F\n");
				exit(1);
			}
		}
		
		//If reading in FATHER, create links to initial person and check for errors.
		//Add them to tree if they are not already added.
		else if (strcmp(is->fields[i], "FATHER") == 0){
			name = strdup(get_name(is));
			temp = jrb_find_str(p_list, name);
			if(temp == NULL){
				t = new_person(name, p_list);
			}
			else{
				t = (Person *) temp->val.v;
			}
			
            if(((p->father != NULL) && (p->father != t)) || (p->mother == t)){
				fprintf(stderr, "Bad input -- child with two fathers on line %d\n", is->line);
				exit(1);
            }
			else if(strcmp(t->sex, "Female") == 0){
                fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                exit(1);
            }

			else if(p->father == NULL){
                p->father = t;
                insert_child(p, t->children);
                t->sex = "Male";
            }
		}
		
		//If reading in MOTHER, create links to initial person and check for errors.
		//Add them to tree if they are not already added.
		else if (strcmp(is->fields[i], "MOTHER") == 0){
            name = strdup(get_name(is));
            temp = jrb_find_str(p_list, name);
            if(temp == NULL){
                t = new_person(name, p_list);
            }
            else{
                t = (Person *) temp->val.v;
            }

            if(((p->mother != NULL) && (p->mother != t)) || (p->father == t)){
                fprintf(stderr, "Bad input -- child with two mothers on line %d\n", is->line);
                exit(1);
            }
			else if(strcmp(t->sex, "Male") == 0){
				fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
				exit(1);
			}
			else if(p->mother == NULL){
                p->mother = t;
                insert_child(p, t->children);
                t->sex = "Female";
            }
        }

		//If reading FATHER_OF, create links to initial person and check for errors.
		//Add them to tree if they are not already added.
		else if(strcmp(is->fields[i], "FATHER_OF") == 0){
			name = strdup(get_name(is));
			temp = jrb_find_str(p_list, name);

			if(temp == NULL){
				t = new_person(name, p_list);
			}
			else{ 
				t = (Person *) temp->val.v;
			}

			if(((t->father != NULL) && (t->father != p)) || (t->mother == p)){
				fprintf(stderr, "Bad input -- child with two fathers on line %d\n", is->line);	
				exit(1);
			}
			else if(strcmp(p->sex, "Female") == 0){
                fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                exit(1);
            }
			else if(t->father == NULL){
                t->father = p;
                insert_child(t, p->children);
                p->sex = "Male";
            }
		}
		
		//If reading MOTHER_OF, create links to initial person and check for errors.
		//Add them to tree if they are not already added.
		else if(strcmp(is->fields[i], "MOTHER_OF") == 0){
            name = strdup(get_name(is));
            temp = jrb_find_str(p_list, name);
            if(temp == NULL){
                t = new_person(name, p_list);
            }
            else{
                t = (Person *) temp->val.v;
            }
			
            if(((t->mother != NULL) && (t->mother != p)) || (t->father == p)){
                fprintf(stderr, "Bad input -- child with two mothers on line %d\n", is->line);
				exit(1);
            }
			else if(strcmp(p->sex, "Male") == 0){
                fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                exit(1);
            }
			else if(t->mother == NULL){
                t->mother = p;
                insert_child(t, p->children);
                p->sex = "Female";
            }
        }
	 }
  }

  //Traverse the created tree and check for cycles.
  jrb_traverse(temp, p_list){
	  p = (Person *) temp->val.v;

	  if(check_cycle(p) == 1){
		  fprintf(stderr, "Bad input -- cycle in specification\n");
		  exit(1);
	  }
  }

  //Print out the tree to stdout.
  print_jrb(p_list);


  //End program.
  jettison_inputstruct(is);
  exit(0);
}

//Allocation of Person variables and insertion into the JRB family tree.
Person *new_person(char * name, JRB p_list){
	Person *p;
	p = malloc(sizeof(Person));
	p->name = strdup(name);
	p->sex = "Unknown";
	p->children = new_dllist();
	p->visited = 0;
	p->printed = 0;
	jrb_insert_str(p_list, name, new_jval_v((void *) p));
	return p;
}

//Get the name of the person after reading in PERSON, FATHER, MOTHER, etc.
char *get_name(IS is) {
    char *name;
    int nsize, i;

    nsize = strlen(is->fields[1]);

    for (i = 2; i < is->NF; i++){
        nsize += (strlen(is->fields[i] + 1));
	}
	
	//Allocate memory now, will free it later.
    name = (char *) malloc(sizeof(char) *(nsize + 10));
    strcpy(name, is->fields[1]);
    nsize = strlen(is->fields[1]);

    for (i = 2; i < is->NF; i++) {
        name[nsize] = ' ';
        strcpy(name + nsize + 1, is->fields[i]);
        nsize += strlen(name + nsize);
    }

    return name;
}

//Insert a child into a Person's children Dllist if they are not already in it.
void insert_child(Person * p, Dllist children){
	Person *temp;
	Dllist Csearch;
	int find = 0;
 
	if(dll_empty(children) == 1){
		dll_append(children, new_jval_v(p));
	}
	else{
		dll_traverse(Csearch, children){
			temp = (Person *) Csearch->val.v;
			if(strcmp(p->name, temp->name) == 0){
				find = 1;
			}
		}
		if(find == 0){
			dll_append(children, new_jval_v(p));
		}
	}
}

//Recursive cycle check function that starts at a node and sees if a Person is a distant
//child of themself.
int check_cycle(Person * p){
	Person *child;
	Dllist temp;
	if(p->visited == 1) return 0;
	
	if(p->visited == 2) return 1;

	p->visited = 2;
	dll_traverse(temp, p->children){
		child = (Person *) temp->val.v;
		if(check_cycle(child)){ 
			return 1;
		}
	}
	p->visited = 1;
	return 0;
}

//Function to print out the JRB tree to stdout.
void print_jrb(JRB p_list){
	Dllist toprint, dtemp, children;
	Person *p;

	Person *father;
	Person *mother;
	Person *child;
	JRB temp;
	char *fname;
	char *mname;
	int fp, mp, parcheck;

	toprint = new_dllist();
	
	//Add people with unknown parents to a Dllist queue.
	jrb_traverse(temp, p_list){
		p = (Person *) temp->val.v;
		if(p->father == NULL && p->mother == NULL){
			dll_append(toprint, new_jval_v(p));
		}
	}
	
	//Traverse the Dllist to print them if they have not already been printed, adding 
	//a Person's children to the end of the queue.
	dll_traverse(dtemp, toprint){
		fp = 0;
		mp = 0;
		parcheck = 0;
		p = (Person *) dtemp->val.v;
		if(p->printed == 0){
			father = (Person *) p->father;
			if(father == NULL){
				fname = "Unknown";
			}
			else{
				fname = father->name;
				parcheck++;
				if(father->printed == 1){
					fp = 1;
				}
			}

			mother = (Person *) p->mother;
			if(mother == NULL){
				mname = "Unknown";
			}
			else{
				mname = mother->name;
				parcheck++;
				if(mother->printed == 1){
					mp = 1;
				}
			}
			
			if((parcheck == 0) || (fp == 1 && mp == 1) || (fp == 1 && mother == NULL) || (father == NULL && mp == 1)){
				printf("%s \n", p->name);
				printf("  Sex: %s\n", p->sex);
				printf("  Father: %s\n", fname);
				printf("  Mother: %s\n", mname);
				printf("  Children:");
				if(dll_empty(p->children) != 1){
					dll_traverse(children, p->children){
						child = (Person *) children->val.v;
						dll_append(toprint, new_jval_v(child));
						printf("\n    %s", child->name);
					}
					printf("\n\n");
				}
				else{
					printf(" None\n\n");
				}
				p->printed = 1;
			}
		}
	}
	
	//Free each person created and then free the tree itself.
	//p->name and p itself are the only things that malloc has been
	//called on, so that is what I use free on.
    jrb_traverse(temp, p_list){
        p = (Person *) temp->val.v;
		free(p->name);
		free(p);
    }
	jrb_free_tree(p_list);
}

