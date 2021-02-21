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

char infile [1024];
char outfile [1024];
char * between_ticks;
char* tick_var;

extern char **environ;

void set_cmd(int argc, char *argv[])
{
	int i;
	if (argc != 1)
	printf("Extra args\n");
	else
	for (i = 0; environ[i] != NULL; i++)
	printf("%s\n", environ[i]);
}
void setenv_mod(char* cmd){
	char cmd_cp[1024];
	strcpy(cmd_cp, cmd);
	char* var = (char *) malloc(1024);
	char* val = (char *) malloc(1024);
	var = strtok(cmd_cp, " =");
	printf("var:%s.\n", var);
	strcpy(cmd_cp, cmd);
	val = strtok(strchr(strchr(cmd_cp, '=')+1, ' ')+1, "");
	printf("val:%s.\n",val);
	if (var == NULL || val == NULL)
			printf("Bad command\n");
	else {
				char n[1024];
				char v[2048];
				strcpy(n, var);
				printf("n: %s\n",n);
				strcpy(v, val);
				printf("v: %s\n", v);
				setenv(n, v, true);
			}



}
void asg(int argc, char *argv[])
{
	char *name, *val;
	if (argc != 1)
		printf("Extra args\n");
	else {
		name = (char *) malloc(1024);
		val = (char *) malloc(2048);
		name = strtok(argv[0], "=");
		val = strtok(NULL, "");
		
		if (name == NULL || val == NULL)
			printf("Bad command\n");
		else
			{
				char n[1024];
				char v[2048];
				strcpy(n, name);
				strcpy(v, val);
				setenv(n, v, true);
			}

	}
return;
}
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
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
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
 	char cmd[BUFFER_LEN];
 	strcpy(cmd, cmd_o);

	token = strtok(cmd," ");
	int i=0;
	
	 while(token!=NULL){
	 
	 	if (strcmp(token, ">")==0)
	 	{
	 		token = strtok(NULL, " ");	
	 		char* t_out_file;
	 		t_out_file = (char *) malloc(BUFFER_LEN);
	 		strcpy(t_out_file, token);
	 		strcpy(outfile, t_out_file);
	 		*out = 1;
	 	}
	 	else if (strcmp(token, "<")==0)
	 	{
	 		token = strtok(NULL, " ");	
	 		char* t_in_file;
	 		t_in_file = (char *) malloc(BUFFER_LEN);
	 		strcpy(t_in_file, token);
	 		strcpy(infile, t_in_file);
	 		*in = 1;
	 	}
	 	else {
	 		argv[i]=strdup(token); 
	 		token = strtok(NULL," "); 
	 		i++;

	 	}
	 	
	 }
 argv[i]=NULL; 
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
			  strncat(mod_cmd, &cmd[i], 1);
		}
	}
	strcpy(cmd, mod_cmd);
	return;
}

bool check_ticks(char* cmd){
	if(strchr(cmd, '`') != NULL){
		char * x = strchr(cmd, '`');
		char * y = strrchr(cmd, '`');
		if (x==y) return false;
		else return true;
	}
	else return false;
}
void parse_ticks(char* cmd){
	char * parsed_cmd = (char *) calloc(1024, sizeof(char));
	strncat(parsed_cmd, strchr(cmd, '`')+1, (strrchr(cmd, '`')-strchr(cmd, '`')-sizeof(char))/sizeof(char));
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
	};

 const char *homedir;

 if ((homedir = getenv("HOME")) == NULL) 
       homedir = getpwuid(getuid())->pw_dir;
		

 if (chdir(homedir)==-1) printf("Cannot start from %s \n", homedir);	

 while(true)
 {
 		char* piped_cmds[100];
		long size;
		char *buf;
		char *ptr;
		
		
 		int p[2];

		int in = 0; 
		int out = 0;


 		size = pathconf(".", _PC_PATH_MAX);
		if ((buf = (char *)malloc((size_t)size)) != NULL)
    		ptr = getcwd(buf, (size_t)size);
		char * user_name = getenv("USER");

		printf("[%s@My shell]——[%s]\n$ ", user_name, ptr); 
		if (read_command(line) == 0 )
		{
			printf("\n"); 
			exit(0);
		} 
		
		if (strcmp(line, "exit") == 0) exit(0); 


		strcpy(line2, line); 
 		if(strchr(line2, '$') != NULL){
			char *bef, *var;
			bef = (char*) malloc(1024);
			char* content = (char*) malloc(1024);
			var = (char*) malloc(1024);
			bef = strtok(line2, "$");
			var = strtok(NULL, " `");
			
			content = getenv(var);
		
			char v[1024] = "$";
			strcat(v, var);
			strcpy( line2,replaceWord(line, v, content));
			
		
		}
		strcpy(line, line2);
		handle_spaces(line);
		strcpy(line2, line);
		
		bool ticks = check_ticks(line2);

		tick_var = (char * ) malloc(1024);
		if (ticks)
		{
			
			tick_var = strtok(line2, " =");
			char ticks_cmd [BUFFER_LEN];	
			between_ticks = (char *) calloc(1024, sizeof(char));
			parse_ticks(line);
			strcpy(line, between_ticks);
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
			piped_cmds[k] = strdup(cp_parsed_cmd);
			num_commands++;
		    parsed_cmd = strtok(NULL, delim);
		    ++k;
		}
		int c = 0;
		for (c = 0; c < k; c++)
			{
				argc = build_args(piped_cmds[c], argv[c], &in, &out);
			}
		
	int temp_in_f = dup(0);
	int temp_out_f = dup(1);
	int pid;
	int loop_cmd = 0;
			
for (loop_cmd = 0; loop_cmd < k; loop_cmd++)
{
	if (loop_cmd == 0)
	{
		p[0] = dup(temp_in_f);
		if (in){
					
					p[0] = open(infile, O_RDONLY);
					if (p[0] < 0)
					{
						perror("failed to open input file: ");
					}
					strcpy(infile, "");

				}

	}
	
	dup2(p[0], 0);
	close(p[0]);
	
	if (loop_cmd == k - 1)
	{
		p[1] = dup(temp_out_f);
		if (out)
				{
						p[1] = open(outfile,  O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						strcpy(outfile, "");
				}
				else if(ticks){
					int fdpipe[2];
					if(pipe(fdpipe)<0) perror("failure in piping: ");
					p[1] = fdpipe[1];
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
			if(strcmp(argv[loop_cmd][0],commands[i]) == 0){
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
					x = chdir(homedir);
				else
					x = chdir(argv[loop_cmd][1]);
				
					if (x != 0)
						perror("could not change directory\n");
				break;
				case 1:
					system("cls||clear");
					break;
				case 2:
					set_cmd(argc, argv[loop_cmd]);
					break;
			default:
				break;

		}
		}
		else{
		
		set_program_path (path,bin,argv[loop_cmd][0]); 
 		
		if (strchr(argv[loop_cmd][0], '=') != NULL && argc == 1)
			asg(argc, argv[loop_cmd]);
		else if (strchr(piped_cmds[loop_cmd], '=') != NULL){
			setenv_mod(piped_cmds[loop_cmd]);
		}
		
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
			char buf2[2*BUFFER_LEN];
			if (ticks){
				read(p[0], buffer, sizeof(buffer));
				strncat(buf2, tick_var_cp, strlen(tick_var_cp)*sizeof(char));
				strncat(buf2, "=", sizeof(char));
				strncat(buf2, buffer, strlen(buffer)*sizeof(char));
			
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