//CS360 Lab 7: Jsh
//By: David Gilson
//The purpose of this code is to create a shell program that can take and process programs correctly.
//This is part 3 of the program, which allows for redirection, as well as piping of the instructions.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <fcntl.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//Here are some function instantiations for the program.
void redirect_input(int fd);
void redirect_output(int fd2);
void pipe_fork(int fork_val1, int fd, int pipefd[2]);
void general_fork(int fd, int fd2);
void process_line(IS is, JRB jrb, int wait_check);

//The main function takes in the intial input arguments, as well as parses the inputs given while
//the program is running.
int main(int argc, char *argv[]){
	IS is;
	is = new_inputstruct(NULL);
	char *prompt;
	
	JRB jrb;
	
	//jsh3 is the default prompt unless specified by the user.
	if(argc == 1){
		prompt = "jsh3";
	}
	else if(argc == 2){
		prompt = argv[1];
	}
	else{
		fprintf(stderr, "Incorrect number of arguments given (None or a single one required)\n");
		exit(1);
	}

	//This while loop always runs, taking in user commands.  CTRL-D or "exit" will end the program.
	while(1){
		//Specifically for plank's gradescript
		if(strcmp(prompt, "-")){
			printf("%s: ", prompt);
		}
		//If getline >= 0, then it is a valid stream.  Otherwise, it is EOF.
		if(get_line(is) >= 0){
			jrb = make_jrb();
			int wait_check = 0;
			
			//If nothing is in the fields when enter is hit, just continue.
			if(is->fields[0] == NULL){
				continue;
			}
			if(strcmp(is->fields[0], "exit") == 0){
				break;
			}

			//If the last character is '&', null the position and set wait_check to 1.
			if(is->text1[strlen(is->text1) - 2] == '&'){
				is->text1[strlen(is->text1)-2] = '\0';
				is->NF -= 1;
				is->fields[is->NF] = (char*)NULL;
				wait_check = 1;
			}
			else{
				wait_check = 0;
				is->fields[is->NF] = (char*)NULL;
			}
			
			//Process the line in this function.
			process_line(is, jrb, wait_check);	
		}
		else{
			break;
		}
		jrb_free_tree(jrb);
	}
	
	//If ctrl-d or "exit" is given, end the program.
	jettison_inputstruct(is);
	return 0;
}

//Use this function when < redirect is called.  Dup2 creates a copy of fd 0 (stdin)
void redirect_input(int fd){
	if(dup2(fd, 0) != 0){
		perror("Error Input");
		exit(1);
	}
	close(fd);
}

//Use this function when > or >> is called.  Dup2 creates a copy of fd 1 (stdout)
void redirect_output(int fd2){ 
	if(dup2(fd2, 1) != 1){
		perror("Error Output");
		exit(1);
	}
	close(fd2);
}

//This function handles forking a process when | is read in
void pipe_fork(int fork_val1, int fd, int pipefd[2]){
	if(fd != -1){
		redirect_input(fd);
		fd = -1;
	}
	
	//Make a copy
	if(dup2(pipefd[1], 1) != 1){
		perror("pipefd[1]");
		exit(1);
	}
}

//This function is for general redirecting
void general_fork(int fd, int fd2){
	if(fd != -1){
		redirect_input(fd);
	}
	if(fd2 != -1){
		redirect_output(fd2);
	}
}

//This function processes the normal shell commands, as well as figures out the redirects and pipes
void process_line(IS is, JRB jrb, int wait_check){
	int fd = -1;
	int fd2 = -1;
	int i, pid, fork_val1, fork_val2, status;
	int pipefd[2];
	int prev_pipe;
	int pipe_val;
	int arg_count = 0;
	
	JRB temp;
	int size = is->NF +1;
	char **command = (char **)malloc(sizeof(char*)*size);
	
	//Set temporary line to null.
	memset(command, 0, size*sizeof(command[0]));
	
	//Go through the fields individually
	for(i = 0; i < is->NF; i++){

		//If > is found, open the file.  Check if it is a bad file, then NULL out the character locations.
		if(strcmp(is->fields[i], ">") == 0){
			
			fd2 = open(is->fields[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644); 			
			if(fd2 < 0){
				perror(is->fields[i+1]);
				exit(1);
			}
			is->fields[i] = (char*)NULL;
			i++;
			is->fields[i] = (char*)NULL;
		}

		//If < is found, open the file to read from it.  Check if it is a bad file, then NULL out the character locations.
		else if(strcmp(is->fields[i], "<") == 0){
				fd = open(is->fields[i+1], O_RDONLY, 0644);
				if(fd < 0){
					perror(is->fields[i+1]);
					exit(1);
				}
				is->fields[i] = (char*)NULL;
				i++;
				is->fields[i] = (char*)NULL;
			}

		//If >> is found, open the file to create, read/write, or append.  Check if it is a bad file, then NULL out the character locations.
		else if(strcmp(is->fields[i], ">>") == 0){
				
				fd2 = open(is->fields[i+1], O_CREAT | O_WRONLY | O_APPEND, 0644);
				if(fd2 < 0){
					perror(is->fields[i+1]);
					exit(1);
				}
				is->fields[i] = (char*)NULL;
				i++;
				is->fields[i] = (char*)NULL;
			}
		
		//If a pipe is given:
		else if(strcmp(is->fields[i], "|") == 0){
				command[arg_count] = (char*)NULL;
				arg_count = 0;

				//Connect the processes
				if(pipe(pipefd) < 0){
					perror("Pipe error");
					exit(1);
				}
	
				//Fork and insert into tree, check file descriptors and redirect the input
				fork_val1 = fork();
				jrb_insert_int(jrb, fork_val1, new_jval_i(fork_val1));

				if(fork_val1 == 0){
					pipe_fork(fork_val1, fd, pipefd);
					
					//Close pipe and call execvp
					close(pipefd[0]);
					close(pipefd[1]);
					execvp(command[0], command);
					perror(command[0]);
					exit(1);
				}
				else{
					if(fd != -1){
						close(fd);
					}
					fd = pipefd[0];
					close(pipefd[1]);
				}
			}
		//"Normal" process handling
		else{
			command[arg_count] = is->fields[i];
			arg_count++;
		}
	}
	//After IS stream is made, NUll out last character.
	command[arg_count] = (char*)NULL;
	
	//Insert forked process into tree and then redirect the input/output.  Then execvp them.
	fork_val2 = fork();
	jrb_insert_int(jrb, fork_val2, new_jval_i(fork_val2));

	if(fork_val2 == 0){
		general_fork(fd, fd2);
		execvp(command[0], command);
		perror(command[0]);
		exit(1);
	}

	//If there is no &, process the jrb and kill the processes
	else{
		if(wait_check == 0){
			pid = wait(&status);
			temp = jrb_find_int(jrb, pid);
			if(temp != NULL){
				jrb_delete_node(temp);
			}

			while( !(jrb_empty(jrb))){
				pid = wait(&status);
				temp = jrb_find_int(jrb, pid);
				if(temp != NULL){
					jrb_delete_node(temp);
				}
			}
		}
		close(fd);
		close(fd2);
	}
	//Free the malloc commands.
	free(command);
}
