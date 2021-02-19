//word expansion

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

extern char **environ;


char* replaceWord(const char* s, const char* oldW, 
                  const char* newW) 
{ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) { 
        // compare the substring with the result 
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

void set_cmd(int argc, char *argv[])
{
	int i;
	if (argc != 1)
	printf("Extra args\n");
	else
	for (i = 0; environ[i] != NULL; i++)
	printf("%s\n", environ[i]);
}
void asg(int argc, char *argv[])
{
	char *name, *val;
	if (argc != 1)
		printf("Extra args\n");
	else {
		name = strtok(argv[0], "=");
		val = strtok(NULL, ""); /* get all that's left */
		if (name == NULL || val == NULL)
			printf("Bad command\n");
		else
			setenv(name, val, true);
	}
return;
}

size_t read_command(char *cmd) 
{
	if(!fgets(cmd, BUFFER_LEN, stdin)) 
 		return 0; 
 	
 	size_t length = strlen(cmd); 
 	
 	if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; 
	
	return strlen(cmd); 
}

int build_args(char * cmd, char ** argv) {
 char *token; 
 token = strtok(cmd," ");

 int i=0;
 while(token!=NULL){
 	argv[i]=token; 
 	token = strtok(NULL," "); 
 	//printf("in build_args argv[%d] = %s\n",i, argv[i]);
 	i++; 
 }
 argv[i]=NULL; 
	return i; 
}


void set_program_path (char * path, char * bin, char * prog) {
	
	memset (path,0,1024); 
	strcpy(path, bin); 
 	strcat(path, prog); 
	for(int i=0; i<strlen(path); i++) 
		if(path[i]=='\n') path[i]='\0';
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
 char *commands[] = {
  "cd", 
  "clear", 
  "set"
};
		const char *homedir;

		if ((homedir = getenv("HOME")) == NULL) {
    		homedir = getpwuid(getuid())->pw_dir;
		}

		if (chdir(homedir)==-1) printf("Cannot start from %s \n", homedir);		

	while(1){
		char* piped_cmds[100];
		long size;
		char *buf;
		char *ptr;
		
		int temp_in_f = dup(0);
		int temp_out_f = dup(1);

		int input_fd;
		int output_fd;


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
 	
 		strcpy(line2, line); 
 		if (strcmp(line, "exit") == 0) exit(0); 

 		//replace $var with its values and remove $var from the string 

 		if(strchr(line, '$') != NULL){
			char *bef, *var;
			char* content;
			bef = strtok(line2, "$");
			var = strtok(NULL, " "); 
			content = getenv(var);
			char v[] = "$";
			strcat(v, var);
			strcpy( line,replaceWord(line, v, content));
		
		}

		strcpy(line2, line);
		char *infile;
		bool input_red = false;
		if(strchr(line2, '<') != NULL){
			input_red = true;
			char* bef;
			bef = strtok(line2, "<");
			infile = strtok(NULL, " "); 		
			strcpy( line,replaceWord(line, "<", " "));
			strcpy( line,replaceWord(line, infile, " "));
		}
		
		if (input_red){
					input_fd = open(infile, O_RDONLY);
				}
		else input_fd = dup(temp_in_f);			

		char* outfile;
		strcpy(line2, line);
		bool output_red = false;
		bool trunc = false; 
		bool append = false;

		if(strstr(line2, ">>") != NULL){
			
			char* bef;
			append = true;
			trunc = false;
			output_red = true;
			outfile = strtok(strstr(line2, ">>")+2, " "); 
					
			strcpy( line,replaceWord(line, ">>", " "));
			strcpy( line,replaceWord(line, outfile, " "));
			
		}
		else if (strchr(line2, '>') != NULL){
			append = false;
			trunc = true;
			output_red = true;
			char* bef;
			bef = strtok(line2, ">");
			outfile = strtok(NULL, " "); 
			strcpy( line,replaceWord(line, ">", " "));
			strcpy( line,replaceWord(line, outfile, " "));
			
		}
		
		char f[100];
		strcpy(f, outfile);

		strcpy(line2, line); 
 		num_commands = 0;	
 		char delim [] = "|";
 		char* parsed_cmd = strtok(line2, delim);
		char cp_parsed_cmd[200];
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
			argc = build_args(piped_cmds[c], argv[c]);
		
		int loop_cmd = 0;
		
for (loop_cmd = 0; loop_cmd < k; loop_cmd++)
{
	dup2(input_fd, 0);
	close(input_fd);
	
	if (loop_cmd == k - 1)
	{
		if (output_red)
				{
					if (trunc)
					{
						printf("outfile: %s\n", f);
						output_fd = open(outfile, O_CREAT|O_TRUNC|O_WRONLY, 0644);
						
					}
					else if (append)
					{
						printf("outfile: %s\n", f);
						output_fd = open(outfile, O_CREAT|O_APPEND|O_WRONLY, 0644);

					}
					if (output_fd == -1)
						{
							perror("failed to open file");
						}
				}		
		else 
		{
			output_fd = dup(temp_out_f);
			if (output_fd == -1)
						{
							perror("failed to open file");
						}

		}
	}
	else {
		int fdpipe[2];
		if(pipe(fdpipe)<0) perror("failure in piping: ");
		output_fd = fdpipe[1];
		input_fd = fdpipe[0];
	}
	if(dup2(output_fd, 1)<0) perror("failure in line 295");
	close(output_fd);
			
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
 		
		if (strchr(argv[loop_cmd][0], '=') != NULL)
			asg(argc, argv[loop_cmd]);
		
 		else{
 			int pid= fork(); 	
 			if(pid==0)
 			{ 
 				execve(path,argv[loop_cmd],0); 
 				fprintf(stderr, "Child process could not do execve\n");
 			}
 			else wait(NULL); 
		}
	}
	
}
	if(dup2(temp_in_f, 0) == -1) perror("failed to restore stdin");
	if(dup2(temp_out_f,1) < 0) perror("failed to restore stdout");
	close(temp_in_f);
	close(temp_out_f);

}
	return 0;
}

