#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

int main()
{
	char input[255];
	char inputCopy[255];
	char* context = NULL;
	char* delims = " \t\n";
	char* tokens[20];
	char* token;
	int loc = 1;
	int pipecount = 0;
	int i = 0;
	int j = 0;
	int count = 0;
	int childstatus;
	int end;
	int runInBackground=0;
	int index=0,index2=0;
	for(j = 0; j < 20; j++)
	{
		tokens[j] = NULL;
	}

	
	while (1)
	{
		runInBackground=0;
		pipecount = 0;
		printf("$ ");
		fgets(input, 255, stdin);
		memcpy(inputCopy, input, 255);
		//tokenizes input string. Up to 20 will work
		context = NULL;
		token = strtok_r(input, delims, &context); //creates tokens
		tokens[0] = token;
		loc = 1;
		while (token != NULL) //iterates through the tokens looking for information
		{
			token = strtok_r(NULL, delims, &context);
			tokens[loc] = token;
			loc++;
		}
		for(j = 0; j < 20; j++)
		{
			if(tokens[j] != NULL)
			{
				if(strpbrk(tokens[j], "|") != NULL);
				{
					pipecount = pipecount + 1;
				}
			}
			if(tokens[j]==NULL){
				end=j-1;
				break;
			}
			if(strcmp(tokens[j],"exit")==0){
				printf("exiting...\n");
				exit(0);
				}
		}
		
		
		//attempt at file redirection
		
/* 		bool checker=true;
		index= strchr(input,"<")-input;
		index2= strchr(input,"<")-input;
		if(index>0 || index2>0){
			if(fork==0){
				if(checker)
				int out= open(tokens[0],STDFILENO);
				close(out);
				else{
					int out=open(first, OWRONLY|O_CREAT);
					dup2(out,STDOUT_FILENO);
					close(out);
				}
				execvp(tokens[0],tokens)
			}
			else{
				waitpid(pid,NULL,0);
			}
		} */
		
		//implements the background command
		if(strcmp(tokens[end],"&")==0){
			tokens[end]=NULL;
			runInBackground=1;
		}
		if(tokens[0]!=NULL)
		//used for cd
		if (strcmp(tokens[0], "cd") == 0)
		{
			printf("changing directory \n");
			chdir(tokens[1]);
		}
		//our attempt at one pipe
		

		
		
		
/* 		else if(pipecount=1){
		
			pipeTokens[0] = strtok(input, "|");
			token1[0] = strtok(&input[0], delims);
			token2[0] = strtok(&input[1], delims);

			pid1=fork();
			if(pid1==0){
				 dup2( pipe[1], STDOUT_FILENO );
				 close(pipe[0]);
				 execvp(token1[0], token1);
			}
			pid2=fork();
			if(pid2==0){
				 dup2( pipe[0], STDIN_FILENO );
				 close(pipe[1]);
				 execvp(token2[0],token2);
			}
			close( pipe[0] );
			close( pipe[1] );
			wait(&pid1);
			wait(&pid2);
		} */

		else{
			if(fork()==0)
			{
				execvp(tokens[0], tokens);
				printf("Exec failed\n");
			}
			else
			{
				if(runInBackground==0){
					wait(NULL);
				}
				else{
					waitpid(childstatus, NULL, WNOHANG); 
				
			}
		}
		}
	}
}
