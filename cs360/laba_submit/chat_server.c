//CS 360 LabA-Final: Threaded Chat Server
//By: David Gilson
//The purpose of this code is to create a chat server on the hydra machines that utilizes the pthreads library.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "sockettome.h"
#define STR_SIZE 1000
#define PRINT 1

//Node structure to help keep track of clients and outputs.
struct list_node{
	char *str;
	FILE *fout;
	struct list_node *flink;
	struct list_node *blink;
};

//Struct to hold client structures in a "Room".  It contains the name of the room, head and tail of the client list,
//head of output list, and the mutexes for the lists.
struct Chat_Room{
	char *name;
	struct list_node *user_head;
	struct list_node *user_tail;
	struct list_node *output;
	pthread_mutex_t user_lock;
	pthread_mutex_t output_lock;
	pthread_cond_t output_cond;
};

//Encompasing struct to help keep track of the Chat Rooms and File Descriptors.
struct Client{
	int fd;
	int num_rooms;
	struct Chat_Room *rooms;
};

//Couple of function instantiations that will handle most of the work.
void *client_thread(void *arg);
void *chat_room_thread(void *arg);

//Main handles parsing the inital command line instruction.  Main also maintains the looping nature of the
//client/server interactions.
int main(int argc, char **argv){
	int i, j, socket, port, num_rooms;
	char *s;
	struct Client *p_client;
	struct Chat_Room *rooms;
	pthread_t tid;

	if(argc < 3){
		fprintf(stderr, "Usage: chat_server port Chat-Room-Names ...\n");
		return 0;
	}
 
	//Initialize the socket with the given port.
	port = atoi(argv[1]);
	socket = serve_socket(port);

	//Create the list of chat rooms and put them in order.
	num_rooms = argc-2;
	rooms = (struct Chat_Room *) malloc(sizeof(struct Chat_Room) * num_rooms);
 
	for(i = 0; i < num_rooms; i++){
		rooms[i].name = argv[i+2];
		for(j = 0; j < i; j++){
			if(strcmp(rooms[j].name, rooms[i].name) == 0){
				break;
			}
			else if(strcmp(rooms[j].name, rooms[i].name) > 0){
				s = rooms[j].name;
				rooms[j].name = rooms[i].name;
				rooms[i].name = s;
			}
		}
   
		//Initialize nodes and mutexes, then create the room thread.
		rooms[i].user_head = NULL;
		rooms[i].user_tail = NULL;
   
		pthread_mutex_init(&(rooms[i].output_lock), NULL);
		pthread_mutex_init(&(rooms[i].user_lock), NULL);
		pthread_cond_init(&(rooms[i].output_cond), NULL);
		pthread_create(&tid, NULL, chat_room_thread, (void*) &(rooms[i]));
	}
 
	//Loop to establish connection to incoming clients.
	while(1){
		i = accept_connection(socket);
		if(PRINT){
			printf("Connection established\n");
		}	
		p_client = (struct Client *) malloc(sizeof(struct Client));
		p_client->fd = i;
		p_client->num_rooms = num_rooms;
		p_client->rooms = rooms;
		pthread_create(&tid, NULL, client_thread, (void*) p_client);
	}
 
	//Free up malloc'd memory and return.
	free(rooms);
	return 0;
}

//This function handles the client connection and interaction with the server.
void *client_thread(void *arg){
	char buf[STR_SIZE], output_str[STR_SIZE], *name;
	FILE *fsocket[2];
	struct Client *tmp;
	struct Chat_Room *room;
	struct list_node *node;
	int i, buflen;

	tmp = (struct Client*) arg;
	fsocket[0] = fdopen(tmp->fd, "r");
	fsocket[1] = fdopen(tmp->fd, "w");

	fflush(fsocket[1]);
	sprintf(buf, "Chat Rooms:\n\n");
	if(fputs(buf, fsocket[1]) == EOF || fflush(fsocket[1]) == EOF){
		fclose(fsocket[0]);
		fclose(fsocket[1]);
		if(PRINT){
			printf("Client_thread dies\n");
		}
		pthread_exit(NULL);
	}
 
	//Add names of the users
	for(i = 0; i < tmp->num_rooms; i++){
		sprintf(buf, "%s:", tmp->rooms[i].name);
		pthread_mutex_lock(&(tmp->rooms[i].user_lock));
		node = tmp->rooms[i].user_head;
		buflen = strlen(buf);
   
		while(node != NULL && buflen < STR_SIZE){
			strcat(buf, " ");
			strcat(buf, node->str);
			buflen += strlen(node->str)+1;
			node = node->flink;
		}
   
		strcat(buf, "\n");
		pthread_mutex_unlock(&(tmp->rooms[i].user_lock));
		if(fputs(buf, fsocket[1]) == EOF || fflush(fsocket[1]) == EOF){
			fclose(fsocket[0]);
			fclose(fsocket[1]);
			if(PRINT){
				printf("Client_thread dies\n");
			}
			pthread_exit(NULL);
		}
	}

	sprintf(buf, "\nEnter your chat name (no spaces):\n");
	if(fputs(buf, fsocket[1]) == EOF || fflush(fsocket[1]) == EOF){
		fclose(fsocket[0]);
		fclose(fsocket[1]);
		if(PRINT){
			printf("Client_thread dies\n");
		}
		pthread_exit(NULL);
	}
	if(fgets(buf, STR_SIZE, fsocket[0]) == NULL){
		fclose(fsocket[0]);
		fclose(fsocket[1]);
		if(PRINT){
			printf("Client_thread dies\n");
		}
		pthread_exit(NULL);
	}
  
	//This is to overwrite the '\n'
	buf[strlen(buf)-1] = '\0';
	name = strdup(buf);

	//Loop until given a valid room name.
	room = NULL;
	while(room == NULL){
		sprintf(buf, "Enter chat room:\n");
		if(fputs(buf, fsocket[1]) == EOF || fflush(fsocket[1]) == EOF){
			fclose(fsocket[0]);
			fclose(fsocket[1]);
			free(name);
			if(PRINT){
				printf("Client_thread dies\n");
			}
			pthread_exit(NULL);
		}
		if(fgets(buf, STR_SIZE, fsocket[0]) == NULL){
			fclose(fsocket[0]);
			fclose(fsocket[1]);
			free(name);
			if(PRINT){
				 printf("Client_thread dies\n");
			}
			pthread_exit(NULL);
		}
		buf[strlen(buf)-1] = '\0';
   
		//Try to find the given room in buf.  If found, break.
		for(i = 0; i < tmp->num_rooms; i++){
			if (strcmp(buf, tmp->rooms[i].name) == 0){
				room = &(tmp->rooms[i]);
				break;
			}
		}
		if(room == NULL){
			sprintf(buf, "Invalid chat room name.\n");
			if(fputs(buf, fsocket[1]) == EOF || fflush(fsocket[1]) == EOF){
				fclose(fsocket[0]);
				fclose(fsocket[1]);
				free(name);
				if(PRINT){
					printf("Client_thread dies\n");
				}
				pthread_exit(NULL);
			}
		}
	}

	//Now that client put it a valid room, we need to add them to the room.
	pthread_mutex_lock(&(room->user_lock));
	
  //Check if list is empty first.
  if(room->user_tail == NULL){
		room->user_head = (struct list_node *) malloc(sizeof(struct list_node));
		room->user_tail = room->user_head;
		room->user_tail->blink = NULL;
	}
	else{
		node = (struct list_node *) malloc(sizeof(struct list_node));
		room->user_tail->flink = node;
		node->blink = room->user_tail;
		room->user_tail = node;
	}
 
	room->user_tail->flink = NULL;
	room->user_tail->fout = fsocket[1];
	room->user_tail->str = strdup(name);
	pthread_mutex_unlock(&(room->user_lock));
	pthread_mutex_lock(&(room->output_lock));
 
	//Output that the given user has joined the room.
	if(room->output == NULL){
		room->output = (struct list_node*) malloc(sizeof(struct list_node));
		node = room->output;
	}
	else{
		node = room->output;
		while (node->flink != NULL) node = node->flink;
		node->flink = (struct list_node*) malloc(sizeof(struct list_node));
		node = node->flink;
	}

	//Output list is just a singly-linked list, so we don't need a blink
	node->blink = NULL;
	node->flink = NULL;
	node->fout = NULL;
	node->str = NULL;
	sprintf(buf, "%s has joined\n", name);
	node->str = strdup(buf);
	pthread_cond_signal(&(room->output_cond));
	pthread_mutex_unlock(&(room->output_lock));

	//Handle user input until they disconnect.
	while(fgets(buf, STR_SIZE, fsocket[0]) != NULL){
		pthread_mutex_lock(&(room->output_lock));
		if(room->output == NULL){
			node = (struct list_node*) malloc(sizeof(struct list_node));
			room->output = node;
		}
		else{
			node = room->output;
			while (node->flink != NULL) node = node->flink;
			node->flink = (struct list_node*) malloc(sizeof(struct list_node));
			node = node->flink;
		}

		node->blink = NULL; 
		node->flink = NULL;
		node->fout = NULL;
		node->str = NULL;
		sprintf(output_str, "%s: %s", name, buf);
		node->str = strdup(output_str);
		pthread_cond_signal(&(room->output_cond));
		pthread_mutex_unlock(&(room->output_lock));
		if(PRINT){
			printf("Read: %s", buf);
		}
	}

	//If we're here, the user has left.
	pthread_mutex_lock(&(room->output_lock));
	if (room->output == NULL){
		room->output = (struct list_node*) malloc(sizeof(struct list_node));
		node = room->output;
	}
	else{
		node = room->output;
		while (node->flink != NULL) node = node->flink;
		node->flink = (struct list_node*) malloc(sizeof(struct list_node));
		node = node->flink;
	}
 
	node->blink = NULL;
	node->flink = NULL;
	node->fout = NULL;
	node->str = NULL;
	sprintf(buf, "%s has left\n", name);
	node->str = strdup(buf);
	pthread_cond_signal(&(room->output_cond));
	pthread_mutex_unlock(&(room->output_lock));
	fclose(fsocket[0]);

	//Free any malloc'd data.
	free(name);
	if(PRINT){ 
		printf("Client_thread dies\n");
	}
	pthread_exit(NULL);
}

//This function handles the user-nodes of the server, as well as sending signals to the users.
void *chat_room_thread(void *arg){
	struct Chat_Room *tmp;
	struct list_node *user_node, *temp_node;

	tmp = (struct Chat_Room*) arg;
	pthread_mutex_lock(&(tmp->output_lock));
	while (1){
		pthread_cond_wait(&(tmp->output_cond), &(tmp->output_lock));
		if(PRINT){
			printf("%s has recieved a signal\n", tmp->name);
		}

		//Send the strings on the output lists to the room clients.
		pthread_mutex_lock(&(tmp->user_lock));
		while (tmp->output != NULL){
			if(PRINT){
				printf("Sending %s to all users in %s\n", tmp->output->str, tmp->name);
			}
			user_node = tmp->user_head;
			while (user_node != NULL){
				if(PRINT){
					printf("Sending %s to %s\n", tmp->output->str, user_node->str);
				}
				if(fputs(tmp->output->str, user_node->fout) == EOF || fflush(user_node->fout) == EOF){
        
					//Remove client from users and close their fd
					if(user_node->blink != NULL){
						user_node->blink->flink = user_node->flink;
					}
					else{
						tmp->user_head = user_node->flink;
						if(tmp->user_head != NULL){
							tmp->user_head->blink = NULL;
						}
					}
					if(user_node->flink != NULL){
						user_node->flink->blink = user_node->blink;
					}
					else{
						tmp->user_tail = user_node->blink;
						if(tmp->user_tail != NULL){
							tmp->user_tail->flink = NULL;
						}
					}

					//Remove the user's node and close their output.
					if(PRINT){
						printf("%s has closed %s's_output\n", tmp->name, user_node->str);
					}
					temp_node = user_node;
					user_node = user_node->flink;
					fclose(temp_node->fout);
					free(temp_node->str);
					free(temp_node);
				}
				else{
					user_node = user_node->flink;
				}
			}
			temp_node = tmp->output;
			tmp->output = tmp->output->flink;
			free(temp_node->str);
			free(temp_node);
		}
		pthread_mutex_unlock(&(tmp->user_lock));
	}
	pthread_mutex_unlock(&(tmp->output_lock));

	//Remove clients from the user list, close their fd, and free the strings.
	pthread_mutex_lock(&(tmp->user_lock));
	user_node = tmp->user_head;
	while (user_node != NULL){
		temp_node = user_node;
		user_node = user_node->flink;
		fclose(temp_node->fout);
		free(temp_node->str);
		free(temp_node);
	}
	pthread_mutex_unlock(&(tmp->user_lock));

	//Finally, empty the output list
	pthread_mutex_lock(&(tmp->output_lock));
	while (tmp->output != NULL){
		temp_node = tmp->output;
		tmp->output = tmp->output->flink;
		free(temp_node->str);
		free(temp_node);
	}
	pthread_mutex_unlock(&(tmp->output_lock));
	if(PRINT){
		printf("%s has died\n", tmp->name);
	}
	pthread_exit(NULL);
}
