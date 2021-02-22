#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_LEN 1024

char infile [1024]; 	/*input file name*/
char outfile [1024]; 	/*output file name*/
char * between_ticks; 	/*the command between the ticks*/
char* tick_var; 		/*the variable before the tick*/

extern char **environ;

void set_cmd(int argc, char *argv[])
{
	int i;
	if (argc != 1)
	printf("Extra args\n");
	else
	for (i = 0; environ[i] != NULL; i++)
	printf("%s\n", environ[i]);			/* print each entry in the environ as long as it is not NULL */
}

void asg(int argc, char *argv[])
{
	char *name, *val;
	if (argc != 1)
		printf("Extra args\n");
	else {
		name = (char *) malloc(1024); 	/*allocate space for the variable name*/
		val = (char *) malloc(2048);	/*allocate space for the value assigned to it*/
		name = strtok(argv[0], "=");	/*parse the name until the equal sign */
		val = strtok(NULL, "");			/*parse whatever comes after the equal sign */
		
		if (name == NULL || val == NULL)
			printf("Failed: Name or Value is NULL\n"); /*if either of them is NULL, then print this message*/
		else
			{
				char n[1024];
				char v[2048];
				strcpy(n, name);
				strcpy(v, val);
				setenv(n, v, true);	/*set the environment with overwrite enabled*/
			}

	}
return;
}
/*this function is from geeks for geeks I used it and I am referencing it here: 
https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
I could have changed the varibale name and reordered it, but this would be dishonet from me; However, I have read the code and understood it and cited it.
*/
char* replaceWord(const char* s, const char* oldW, 
                  const char* newW) 
{ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
            i += oldWlen - 1; 
        } 
    } 
  
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) { 
        if (strstr(s, oldW) == s) { 
            strcpy(&result[i], newW); /*copy the new word in the place of the old word if found*/
            i += newWlen; /*go over the new word*/
            s += oldWlen;  /*skip the old char*/
        } 
        else
            result[i++] = *s++;  /*copy one more character*/
    } 
  
    result[i] = '\0'; /*adds a null terminator at the end*/
    return result; 
} 


size_t read_command(char *cmd) 
{
	if(!fgets(cmd, BUFFER_LEN, stdin)) 
 		return 0; 
 	
 	size_t length = strlen(cmd); 
 	
 	if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; 
	
	return strlen(cmd); 
}

int build_args(char * cmd_o, char ** argv, int* in, int *out) {
 	char *token; 
 	char *cmd = (char *) malloc(BUFFER_LEN); /*a local copy of the command to work on it safely */
 
 	strcpy(cmd, cmd_o); /*copy it into the buffer*/

	token = strtok(cmd," "); /*parse the first token so that the while loop works*/
	int i=0;
	
	 while(token!=NULL){
	 
	 	if (strcmp(token, ">")==0) /*if found output redirection, parse file name and set the out var to true*/
	 	{
	 		token = strtok(NULL, " ");	
	 		char* t_out_file;
	 		t_out_file = (char *) malloc(BUFFER_LEN);
	 		strcpy(t_out_file, token);
	 		strcpy(outfile, t_out_file);
	 		*out = 1;
	 	}
	 	else if (strcmp(token, "<")==0) /*if found input redirection, parse file name and set the in var to true*/
	 	{
	 		token = strtok(NULL, " ");	
	 		char* t_in_file;
	 		t_in_file = (char *) malloc(BUFFER_LEN);
	 		strcpy(t_in_file, token);
	 		strcpy(infile, t_in_file);
	 		*in = 1;
	 	}
	 	else { /*otherwise, just copy the token into argv*/
	 		argv[i] = (char * ) malloc(1024);
	 		strcpy(argv[i], token);
	 		
	 		token = strtok(NULL," "); 
	 		i++;

	 	}
	 	
	 }
 argv[i]=NULL;  /*set the last one to null so that execvp works*/
	return i; 
}

void set_program_path (char * path, char * bin, char * prog) {
	memset (path,0,1024); 
	strcpy(path, bin); 
 	strcat(path, prog); 
 	int i=0;
	for(; i<strlen(path); i++) 
		if(path[i]=='\n') path[i]='\0'; 
}

void handle_spaces(char* cmd){
	int i = 0; 
	char *special[] = {
		  '<', 
		  '>', 
		  '|'
		};

	char* mod_cmd = (char*) malloc(1024);

	int special_c = 3;
	for (i = 0; i < strlen(cmd); i++){
		int j = 0;
		bool c = false;
		for ( ;j < special_c; j++)
		{
			/*loop over the special characters and if found add a space before and after it*/
			if (cmd[i] == special[j])
			{
				c = true;
				strncat(mod_cmd, " ", 1);
            	strncat(mod_cmd, &cmd[i], 1);
            	strncat(mod_cmd, " ", 1);				
			}
		}
		if (!c)
		{
			  strncat(mod_cmd, &cmd[i], 1); /*else, copy a char by char */
		}
	}
	strcpy(cmd, mod_cmd); /*return the modified string*/
	return;
}

bool check_ticks(char* cmd){
	if(strchr(cmd, '`') != NULL){
		char * x = strchr(cmd, '`'); /*the position of the first tick*/
		char * y = strrchr(cmd, '`'); /*the position of the last tick*/
		if (x==y) return false; /*they must point to two different locations*/
		else return true; /*if they are not equal then we have valid ticks*/
	}
	else return false; /*if no ticks are found then return false*/
}
void parse_ticks(char* cmd){
	char * parsed_cmd = (char *) calloc(1024, sizeof(char));
	strncat(parsed_cmd, strchr(cmd, '`')+1, (strrchr(cmd, '`')-strchr(cmd, '`')-sizeof(char))/sizeof(char)); /*parse starting from the location of the first tick till the character before the final tick*/
	parsed_cmd[(strrchr(cmd, '`')-strchr(cmd, '`'))] = '\0'; 
	strcpy(between_ticks, parsed_cmd);
}

int main(){

 char line[BUFFER_LEN]; 
 char line2[BUFFER_LEN];
 char* argv[100][100]; 

 char* bin= "/bin/";
 char path[1024]; 
 int argc; 
 int num_commands;
 

 int special_count = 3;
 char *commands[] = 
 	{
		  "cd", 
		  "clear", 
		  "set"
	};  /*special commands to handle*/

 const char *homedir;

 if ((homedir = getenv("HOME")) == NULL) 
       homedir = getpwuid(getuid())->pw_dir; /*gets the homedir if it is not in the environ */
		

 if (chdir(homedir)==-1) printf("Cannot start from %s \n", homedir);	/*cd into it and if not possible print the error*/

 while(true)
 {
 		char* piped_cmds[100]; /*array of commands separated by pipes */
		long size;
		char *buf;
		char *ptr;
		
		
 		int p[2]; /*array of file descriptors used in pipe()*/

		int in = 0; /*bool to indicate if we have input redirection*/
		int out = 0; /*for output redirection*/


 		size = pathconf(".", _PC_PATH_MAX);
		if ((buf = (char *)malloc((size_t)size)) != NULL)
    		ptr = getcwd(buf, (size_t)size);
		char * user_name = getenv("USER"); /*get the user name to print it in the prompt */

		printf("[%s@My shell]——[%s]\n$ ", user_name, ptr); /*print the prompt format*/
		if (read_command(line) == 0 ) /*read the command */
		{
			printf("\n"); /*print a new line after it*/
			exit(0);
		} 
		
		if (strcmp(line, "exit") == 0) exit(0); /*if he entered exit,then exit*/


		strcpy(line2, line); 
 		if(strchr(line2, '$') != NULL){ /*handle environment variables if you found the dollar sign*/
			char *bef, *var;
			bef = (char*) malloc(1024);
			char* content = (char*) malloc(1024);
			var = (char*) malloc(1024);
			bef = strtok(line2, "$"); /*whatever comes before the '$' */
			var = strtok(NULL, " "); /*the variable after the dollar sign */
			content = getenv(var); /*the content of the variable from the environ */
			char v[1024] = "$";
			strcat(v, var);
			strcpy( line2,replaceWord(line, v, content)); /*replace the $var with the value */
		
		}
		strcpy(line, line2);
		handle_spaces(line); /*add spaces before and after special characters for correct parsing and execution */
		strcpy(line2, line);
		bool ticks = check_ticks(line2); /*check if there are ticks `cat whatever`*/

		tick_var = (char * ) malloc(1024);
		if (ticks)
		{
			
			tick_var = strtok(line2, " ="); /*parse the var name before the equal sign if we have tickss*/
			char ticks_cmd [BUFFER_LEN];	
			between_ticks = (char *) calloc(1024, sizeof(char)); /*allocate memory for the command between the ticks*/
			parse_ticks(line);									/*parse the command between the ticks*/
			strcpy(line, between_ticks);						/*copy it into the line command which is going to be executed*/
		}
		char tick_var_cp [100];
		if (ticks)
		 	strcpy(tick_var_cp, tick_var);
		
		strcpy(line2, line);

		
 		num_commands = 0;	
 		char delim [] = "|";
 		char* parsed_cmd = strtok(line2, delim);
		char cp_parsed_cmd[1024];
		int k = 0;

		while(parsed_cmd != NULL){
		 	strcpy(cp_parsed_cmd, parsed_cmd);
			piped_cmds[k] = (char *) malloc(BUFFER_LEN); /*allocate size for each command in the pipe*/
			strcpy(piped_cmds[k], cp_parsed_cmd); /*copy each of them*/
			num_commands++;
		    parsed_cmd = strtok(NULL, delim); /*read until a pipe is found*/
		    ++k;
		}
		int c = 0;
		for (c = 0; c < k; c++)
			{
				/*build the args for each command */
				argc = build_args(piped_cmds[c], argv[c], &in, &out);
			}
		
	int temp_in_f = dup(0); /*duplicate STDIN*/
	int temp_out_f = dup(1); /*duplicate STDOUT*/
	int pid; /*save process id for checking later on in the fork*/
	int loop_cmd = 0;
			
for (loop_cmd = 0; loop_cmd < k; loop_cmd++)
{
	if (loop_cmd == 0)
	{
		p[0] = dup(temp_in_f); /*save the stdin into p[0]*/
		if (in){
					
					p[0] = open(infile, O_RDONLY);	/*if we have input redirection open the input file and save it as the input source in the pipe*/
					if (p[0] < 0)
					{
						perror("failed to open input file: ");
					}
					strcpy(infile, ""); /*reset it*/

				}

	}
	
	dup2(p[0], 0); 	/*duplicate it into the 0 file descriptor*/
	close(p[0]);	/*close the open descriptor*/
	
	if (loop_cmd == k - 1) /*if it was the last command*/
	{
		p[1] = dup(temp_out_f); /*set the pipe descriptor to the stdout*/
		if (out)
				{
					/*but if we have output redirection open the file and write to it*/
						p[1] = open(outfile,  O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						strcpy(outfile, ""); /*reset output file name*/
				}
				else if(ticks){
					int fdpipe[2]; 
					if(pipe(fdpipe)<0) perror("failure in piping: ");
					p[1] = fdpipe[1]; /*if there are ticks, write to it*/
					p[0] = fdpipe[0];

				}		
				
		if (p[1] == -1)
		{
			perror("failed to open output file");
		}				
		
	}
	else {
		int fdpipe[2];
		if(pipe(fdpipe)<0) perror("failure in piping: ");
		p[1] = fdpipe[1];
		p[0] = fdpipe[0];
	}
	if(dup2(p[1], 1)<0) perror("failure in dup2 of p[1]");
	close(p[1]);
			
 		int i = 0;
 		bool  set = false;
		for (i = 0; i < special_count; ++i)
		{
			if(strcmp(argv[loop_cmd][0],commands[i]) == 0){ /*compare the command with the special commands to check if a matach is found*/
				set = true;
				break;
			}
			
		}

		int x;
		if (set)
		{
			switch(i){
			case 0: 
				if (argv[loop_cmd][1] == NULL)
					x = chdir(homedir); /*if typed cd only, then cd into the home dir*/
				else
					x = chdir(argv[loop_cmd][1]); /*else, cd into the desired directory*/
				
					if (x != 0)
						perror("could not change directory\n");
				break;
				case 1:
					system("cls||clear"); /*clear screen*/
					break;
				case 2:
					set_cmd(argc, argv[loop_cmd]); /*print environ*/
					break;
			default:
				break;

		}
		}
		else{
		
		set_program_path (path,bin,argv[loop_cmd][0]); 
 		
		if (strchr(argv[loop_cmd][0], '=') != NULL)
			asg(argc, argv[loop_cmd]);
		
 		else{
 			pid= fork(); 	
 			if(pid==0)
 			{ 	
 				
 				execve(path,argv[loop_cmd],0); 
 				fprintf(stderr, "Child process could not do execve\n");
 				exit(15);
 			}
		}
	}
	
}	
			char buffer[2*BUFFER_LEN];
			/*char buf2[2*BUFFER_LEN];*/
			if (ticks){
				read(p[0], buffer, sizeof(buffer)); /*read from the pipe*/
				
			/*	strncat(buf2, tick_var_cp, strlen(tick_var_cp)*sizeof(char));
				strncat(buf2, "=", sizeof(char));
				strncat(buf2, buffer, strlen(buffer)*sizeof(char));*/
			
				if (setenv (tick_var_cp, buffer, true) < 0){
					perror("Error setting environment variable");
				}
			}

	if(dup2(temp_in_f, 0) == -1) perror("failed to restore stdin");
	if(dup2(temp_out_f,1) < 0) perror("failed to restore stdout");
	close(temp_in_f);
	close(temp_out_f);
	waitpid(pid, NULL, WUNTRACED);
	

}
	return 0;
}